#include "web_server.h"
#include "storage.h"
#include "wifi_manager.h"
#include "system_monitor.h"
#include "rgb_led.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "cJSON.h"
#include <cstring>
#include <string>
#include <algorithm>

static const char* TAG = "web_server";

extern const char _binary_index_html_start[];
extern const char _binary_index_html_end[];
extern const char _binary_style_css_start[];
extern const char _binary_style_css_end[];
extern const char _binary_app_js_start[];
extern const char _binary_app_js_end[];

static httpd_handle_t s_server = nullptr;
static web::LogCallback s_log_cb = nullptr;

static void log(const std::string& msg) {
    ESP_LOGI(TAG, "%s", msg.c_str());
    if (s_log_cb) s_log_cb(msg);
}

static esp_err_t send_response(httpd_req_t* req, const char* content_type, const char* data, size_t len) {
    httpd_resp_set_type(req, content_type);
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store, no-cache, must-revalidate, proxy-revalidate");
    return httpd_resp_send(req, data, len);
}

static esp_err_t send_json(httpd_req_t* req, cJSON* json) {
    char* str = cJSON_PrintUnformatted(json);
    esp_err_t ret = send_response(req, "application/json", str, strlen(str));
    free(str);
    return ret;
}

static esp_err_t send_success(httpd_req_t* req, bool ok) {
    cJSON* res = cJSON_CreateObject();
    cJSON_AddBoolToObject(res, "success", ok);
    esp_err_t ret = send_json(req, res);
    cJSON_Delete(res);
    return ret;
}

static std::string read_body(httpd_req_t* req) {
    char buf[512];
    int total = req->content_len;
    std::string body;
    while (total > 0) {
        int received = httpd_req_recv(req, buf, std::min(total, (int)sizeof(buf)));
        if (received <= 0) break;
        body.append(buf, received);
        total -= received;
    }
    return body;
}

static cJSON* get_json_body(httpd_req_t* req) {
    std::string body = read_body(req);
    return body.empty() ? nullptr : cJSON_Parse(body.c_str());
}

static esp_err_t serve_embedded(httpd_req_t* req, const char* start, const char* end, const char* ctype) {
    return send_response(req, ctype, start, end - start);
}

static esp_err_t handler_index(httpd_req_t* req) {
    return serve_embedded(req, _binary_index_html_start, _binary_index_html_end, "text/html");
}

static esp_err_t handler_css(httpd_req_t* req) {
    return serve_embedded(req, _binary_style_css_start, _binary_style_css_end, "text/css");
}

static esp_err_t handler_js(httpd_req_t* req) {
    return serve_embedded(req, _binary_app_js_start, _binary_app_js_end, "application/javascript");
}

static esp_err_t handler_options(httpd_req_t* req) {
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type");
    return httpd_resp_send(req, nullptr, 0);
}

static esp_err_t api_status(httpd_req_t* req) {
    cJSON* root = cJSON_CreateObject();
    cJSON_AddBoolToObject(root, "sta_connected", wifi::is_sta_connected());
    cJSON_AddStringToObject(root, "sta_ip", wifi::get_sta_ip().c_str());
    cJSON_AddStringToObject(root, "language", storage::load_language().c_str());

    auto apcfg = storage::load_ap_config();
    cJSON* ap_obj = cJSON_CreateObject();
    cJSON_AddStringToObject(ap_obj, "ssid", apcfg.ssid.c_str());
    cJSON_AddStringToObject(ap_obj, "password", apcfg.password.c_str());
    cJSON_AddItemToObject(root, "ap", ap_obj);

    esp_err_t ret = send_json(req, root);
    cJSON_Delete(root);
    return ret;
}

static esp_err_t api_lang_get(httpd_req_t* req) {
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "language", storage::load_language().c_str());
    esp_err_t ret = send_json(req, root);
    cJSON_Delete(root);
    return ret;
}

static esp_err_t api_lang_set(httpd_req_t* req) {
    cJSON* root = get_json_body(req);
    if (!root) return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");

    cJSON* lang = cJSON_GetObjectItem(root, "language");
    bool ok = (lang && cJSON_IsString(lang)) && (lang->valuestring == std::string("zh") || lang->valuestring == std::string("en"));
    if (ok) {
        ok = storage::save_language(lang->valuestring);
        if (ok) log("Language switched: " + std::string(lang->valuestring));
    }
    cJSON_Delete(root);
    return send_success(req, ok);
}

static esp_err_t api_ai_get(httpd_req_t* req) {
    auto cfg = storage::load_ai_config();
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "api_url", cfg.api_url.c_str());
    cJSON_AddStringToObject(root, "api_key", cfg.api_key.c_str());
    cJSON_AddStringToObject(root, "personality", cfg.personality.c_str());
    esp_err_t ret = send_json(req, root);
    cJSON_Delete(root);
    return ret;
}

static esp_err_t api_ai_set(httpd_req_t* req) {
    cJSON* root = get_json_body(req);
    if (!root) return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");

    storage::AIConfig cfg;
    cJSON* url = cJSON_GetObjectItem(root, "api_url");
    cJSON* key = cJSON_GetObjectItem(root, "api_key");
    cJSON* per = cJSON_GetObjectItem(root, "personality");
    if (url && cJSON_IsString(url)) cfg.api_url = url->valuestring;
    if (key && cJSON_IsString(key)) cfg.api_key = key->valuestring;
    if (per && cJSON_IsString(per)) cfg.personality = per->valuestring;

    bool ok = storage::save_ai_config(cfg);
    if (ok) log("AI config saved: " + cfg.api_url);
    cJSON_Delete(root);
    return send_success(req, ok);
}

static int log_level_to_esp(storage::LogLevel level) {
    switch (level) {
        case storage::LogLevel::OFF: return ESP_LOG_NONE;
        case storage::LogLevel::ERROR: return ESP_LOG_ERROR;
        case storage::LogLevel::INFO: return ESP_LOG_INFO;
        case storage::LogLevel::DEBUG: return ESP_LOG_DEBUG;
    }
    return ESP_LOG_INFO;
}

static storage::LogLevel int_to_log_level(int value) {
    if (value < 0 || value > 3) return storage::LogLevel::INFO;
    return static_cast<storage::LogLevel>(value);
}

static esp_err_t api_log_level_get(httpd_req_t* req) {
    cJSON* root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "level", static_cast<int>(storage::load_log_level()));
    esp_err_t ret = send_json(req, root);
    cJSON_Delete(root);
    return ret;
}

static esp_err_t api_log_level_set(httpd_req_t* req) {
    cJSON* root = get_json_body(req);
    if (!root) return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");

    cJSON* level_json = cJSON_GetObjectItem(root, "level");
    bool ok = false;
    if (level_json && cJSON_IsNumber(level_json)) {
        storage::LogLevel level = int_to_log_level(level_json->valueint);
        ok = storage::save_log_level(level);
        if (ok) {
            esp_log_level_set("*", static_cast<esp_log_level_t>(log_level_to_esp(level)));
            log("Log level switched: " + std::to_string(level_json->valueint));
        }
    }
    cJSON_Delete(root);
    return send_success(req, ok);
}

static esp_err_t api_system_status(httpd_req_t* req) {
    auto status = monitor::get_status();
    cJSON* root = cJSON_CreateObject();

    cJSON* cpu = cJSON_CreateObject();
    cJSON_AddNumberToObject(cpu, "percent", status.cpu_percent);
    cJSON_AddItemToObject(root, "cpu", cpu);

    cJSON* mem = cJSON_CreateObject();
    cJSON_AddNumberToObject(mem, "percent", status.mem_percent);
    cJSON_AddNumberToObject(mem, "total", status.mem_total);
    cJSON_AddNumberToObject(mem, "free", status.mem_free);
    cJSON_AddItemToObject(root, "mem", mem);

    cJSON* flash = cJSON_CreateObject();
    cJSON_AddNumberToObject(flash, "percent", status.flash_percent);
    cJSON_AddNumberToObject(flash, "total", status.flash_total);
    cJSON_AddNumberToObject(flash, "used", status.flash_used);
    cJSON_AddItemToObject(root, "flash", flash);

    esp_err_t ret = send_json(req, root);
    cJSON_Delete(root);
    return ret;
}

static esp_err_t api_rgb_get(httpd_req_t* req) {
    auto cfg = rgb::get_config();
    cJSON* root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "mode", static_cast<int>(cfg.mode));
    cJSON_AddNumberToObject(root, "pin", cfg.pin);
    esp_err_t ret = send_json(req, root);
    cJSON_Delete(root);
    return ret;
}

static esp_err_t api_rgb_set(httpd_req_t* req) {
    cJSON* root = get_json_body(req);
    if (!root) return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");

    cJSON* mode_json = cJSON_GetObjectItem(root, "mode");
    cJSON* pin_json = cJSON_GetObjectItem(root, "pin");

    storage::RGBConfig cfg = storage::load_rgb_config();
    if (mode_json && cJSON_IsNumber(mode_json)) {
        int mode = mode_json->valueint;
        if (mode >= 0 && mode <= 4) cfg.mode = mode;
    }
    if (pin_json && cJSON_IsNumber(pin_json)) cfg.pin = pin_json->valueint;

    bool ok = storage::save_rgb_config(cfg);
    if (ok) {
        rgb::set_config({static_cast<rgb::Mode>(cfg.mode), cfg.pin});
        log("RGB config updated: mode=" + std::to_string(cfg.mode) + " pin=" + std::to_string(cfg.pin));
    }
    cJSON_Delete(root);
    return send_success(req, ok);
}

static esp_err_t api_wifi_list(httpd_req_t* req) {
    auto creds = storage::load_wifi_creds();
    cJSON* arr = cJSON_CreateArray();
    for (const auto& c : creds) {
        cJSON* obj = cJSON_CreateObject();
        cJSON_AddStringToObject(obj, "ssid", c.ssid.c_str());
        cJSON_AddStringToObject(obj, "password", c.password.c_str());
        cJSON_AddItemToArray(arr, obj);
    }
    esp_err_t ret = send_json(req, arr);
    cJSON_Delete(arr);
    return ret;
}

static esp_err_t api_wifi_add(httpd_req_t* req) {
    cJSON* root = get_json_body(req);
    if (!root) return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");

    cJSON* ssid = cJSON_GetObjectItem(root, "ssid");
    cJSON* password = cJSON_GetObjectItem(root, "password");
    bool ok = (ssid && cJSON_IsString(ssid));
    if (ok) {
        const char* pass = (password && cJSON_IsString(password)) ? password->valuestring : "";
        ok = storage::add_wifi_cred(ssid->valuestring, pass);
        if (ok) log("WiFi saved: " + std::string(ssid->valuestring));
    }
    cJSON_Delete(root);
    return send_success(req, ok);
}

static esp_err_t api_wifi_delete(httpd_req_t* req) {
    cJSON* root = get_json_body(req);
    if (!root) return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");

    cJSON* ssid = cJSON_GetObjectItem(root, "ssid");
    bool ok = (ssid && cJSON_IsString(ssid));
    if (ok) {
        ok = storage::remove_wifi_cred(ssid->valuestring);
        if (ok) log("WiFi deleted: " + std::string(ssid->valuestring));
    }
    cJSON_Delete(root);
    return send_success(req, ok);
}

static esp_err_t api_wifi_connect(httpd_req_t* req) {
    cJSON* root = get_json_body(req);
    if (!root) return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");

    cJSON* ssid = cJSON_GetObjectItem(root, "ssid");
    cJSON* password = cJSON_GetObjectItem(root, "password");
    bool ok = (ssid && cJSON_IsString(ssid));
    if (ok) {
        const char* pass = (password && cJSON_IsString(password)) ? password->valuestring : "";
        ok = wifi::connect_sta(ssid->valuestring, pass);
        if (ok) log("Connecting WiFi: " + std::string(ssid->valuestring));
    }
    cJSON_Delete(root);
    return send_success(req, ok);
}

static esp_err_t api_wifi_scan(httpd_req_t* req) {
    auto aps = wifi::scan_networks();
    cJSON* arr = cJSON_CreateArray();
    for (const auto& ap : aps) {
        cJSON* obj = cJSON_CreateObject();
        cJSON_AddStringToObject(obj, "ssid", ap.ssid.c_str());
        cJSON_AddNumberToObject(obj, "rssi", ap.rssi);
        cJSON_AddStringToObject(obj, "auth", ap.auth.c_str());
        cJSON_AddItemToArray(arr, obj);
    }
    esp_err_t ret = send_json(req, arr);
    cJSON_Delete(arr);
    return ret;
}

static esp_err_t api_ap_set(httpd_req_t* req) {
    cJSON* root = get_json_body(req);
    if (!root) return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");

    cJSON* ssid = cJSON_GetObjectItem(root, "ssid");
    cJSON* password = cJSON_GetObjectItem(root, "password");
    bool ok = (ssid && password && cJSON_IsString(ssid) && cJSON_IsString(password));
    if (ok) {
        storage::APConfig cfg{ssid->valuestring, password->valuestring};
        ok = storage::save_ap_config(cfg);
        if (ok) {
            log("AP config saved: " + cfg.ssid);
            wifi::start_ap(cfg);
        }
    }
    cJSON_Delete(root);
    return send_success(req, ok);
}

static esp_err_t api_chat_list(httpd_req_t* req) {
    auto ids = storage::chat_list();
    cJSON* arr = cJSON_CreateArray();
    for (const auto& id : ids) {
        cJSON* obj = cJSON_CreateObject();
        cJSON_AddStringToObject(obj, "id", id.c_str());
        cJSON_AddStringToObject(obj, "title", ("Chat " + id).c_str());
        cJSON_AddItemToArray(arr, obj);
    }
    esp_err_t ret = send_json(req, arr);
    cJSON_Delete(arr);
    return ret;
}

static esp_err_t api_chat_get(httpd_req_t* req) {
    char id_buf[64] = {};
    if (httpd_req_get_url_query_str(req, id_buf, sizeof(id_buf)) == ESP_OK) {
        char val[64] = {};
        if (httpd_query_key_value(id_buf, "id", val, sizeof(val)) == ESP_OK) {
            std::string data = storage::chat_load(val);
            return send_response(req, "application/json", data.c_str(), data.length());
        }
    }
    return send_response(req, "application/json", "[]", 2);
}

static esp_err_t api_chat_save(httpd_req_t* req) {
    cJSON* root = get_json_body(req);
    if (!root) return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");

    cJSON* id = cJSON_GetObjectItem(root, "id");
    cJSON* messages = cJSON_GetObjectItem(root, "messages");
    bool ok = (id && messages && cJSON_IsString(id));
    if (ok) {
        char* str = cJSON_PrintUnformatted(messages);
        ok = storage::chat_save(id->valuestring, str);
        free(str);
    }
    cJSON_Delete(root);
    return send_success(req, ok);
}

static esp_err_t api_chat_delete(httpd_req_t* req) {
    cJSON* root = get_json_body(req);
    if (!root) return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");

    cJSON* id = cJSON_GetObjectItem(root, "id");
    bool ok = (id && cJSON_IsString(id));
    if (ok) {
        ok = storage::chat_delete(id->valuestring);
        if (ok) log("Chat deleted: " + std::string(id->valuestring));
    }
    cJSON_Delete(root);
    return send_success(req, ok);
}

static const httpd_uri_t routes[] = {
    { "/", HTTP_GET, handler_index, nullptr },
    { "/style.css", HTTP_GET, handler_css, nullptr },
    { "/app.js", HTTP_GET, handler_js, nullptr },
    { "/api/status", HTTP_GET, api_status, nullptr },
    { "/api/lang/get", HTTP_GET, api_lang_get, nullptr },
    { "/api/lang/set", HTTP_POST, api_lang_set, nullptr },
    { "/api/ai", HTTP_GET, api_ai_get, nullptr },
    { "/api/ai", HTTP_POST, api_ai_set, nullptr },
    { "/api/log_level", HTTP_GET, api_log_level_get, nullptr },
    { "/api/log_level", HTTP_POST, api_log_level_set, nullptr },
    { "/api/system_status", HTTP_GET, api_system_status, nullptr },
    { "/api/rgb", HTTP_GET, api_rgb_get, nullptr },
    { "/api/rgb", HTTP_POST, api_rgb_set, nullptr },
    { "/api/wifi/list", HTTP_GET, api_wifi_list, nullptr },
    { "/api/wifi/add", HTTP_POST, api_wifi_add, nullptr },
    { "/api/wifi/delete", HTTP_POST, api_wifi_delete, nullptr },
    { "/api/wifi/connect", HTTP_POST, api_wifi_connect, nullptr },
    { "/api/wifi/scan", HTTP_GET, api_wifi_scan, nullptr },
    { "/api/ap/set", HTTP_POST, api_ap_set, nullptr },
    { "/api/chat/list", HTTP_GET, api_chat_list, nullptr },
    { "/api/chat/get", HTTP_GET, api_chat_get, nullptr },
    { "/api/chat/save", HTTP_POST, api_chat_save, nullptr },
    { "/api/chat/delete", HTTP_POST, api_chat_delete, nullptr },
};

namespace web {

bool start(LogCallback log_cb) {
    s_log_cb = log_cb;

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 8192;
    config.max_uri_handlers = 24;
    config.uri_match_fn = httpd_uri_match_wildcard;

    if (httpd_start(&s_server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start server");
        return false;
    }

    for (const auto& r : routes) {
        httpd_register_uri_handler(s_server, &r);
    }

    httpd_uri_t options_uri = { "*", HTTP_OPTIONS, handler_options, nullptr };
    httpd_register_uri_handler(s_server, &options_uri);

    log("Web server started");
    return true;
}

}
