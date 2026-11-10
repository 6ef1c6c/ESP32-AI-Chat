#include "web_server.h"
#include "storage.h"
#include "wifi_manager.h"
#include "system_monitor.h"
#include "rgb_led.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "esp_http_client.h"
#include "esp_system.h"
#include "cJSON.h"
#include <string>
#include <cstring>
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

static cJSON* profile_to_json(const storage::AIProfile& p, bool mask_key) {
    cJSON* obj = cJSON_CreateObject();
    cJSON_AddStringToObject(obj, "id", p.id.c_str());
    cJSON_AddStringToObject(obj, "name", p.name.c_str());
    cJSON_AddStringToObject(obj, "api_url", p.api_url.c_str());
    if (mask_key && p.api_key.length() > 12) {
        std::string masked = p.api_key.substr(0, 6) + std::string(10, '*') + p.api_key.substr(p.api_key.length() - 5);
        cJSON_AddStringToObject(obj, "api_key", masked.c_str());
    } else {
        cJSON_AddStringToObject(obj, "api_key", p.api_key.c_str());
    }
    cJSON_AddStringToObject(obj, "model", p.model.c_str());
    cJSON_AddStringToObject(obj, "personality", p.personality.c_str());
    return obj;
}

static esp_err_t api_ai_get(httpd_req_t* req) {
    auto profiles = storage::load_ai_profiles();
    std::string active = storage::load_ai_active();
    cJSON* root = cJSON_CreateObject();
    cJSON* arr = cJSON_CreateArray();
    for (const auto& p : profiles) cJSON_AddItemToArray(arr, profile_to_json(p, false));
    cJSON_AddItemToObject(root, "profiles", arr);
    cJSON_AddStringToObject(root, "active", active.c_str());
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
    cJSON* model = cJSON_GetObjectItem(root, "model");
    cJSON* per = cJSON_GetObjectItem(root, "personality");
    if (url && cJSON_IsString(url)) cfg.api_url = url->valuestring;
    if (key && cJSON_IsString(key)) cfg.api_key = key->valuestring;
    if (model && cJSON_IsString(model)) cfg.model = model->valuestring;
    if (per && cJSON_IsString(per)) cfg.personality = per->valuestring;

    bool ok = storage::save_ai_config(cfg);
    if (ok) log("AI config saved: " + cfg.api_url);
    cJSON_Delete(root);
    return send_success(req, ok);
}

static esp_err_t api_ai_profiles_get(httpd_req_t* req) {
    auto profiles = storage::load_ai_profiles();
    std::string active = storage::load_ai_active();
    cJSON* root = cJSON_CreateObject();
    cJSON* arr = cJSON_CreateArray();
    for (const auto& p : profiles) cJSON_AddItemToArray(arr, profile_to_json(p, true));
    cJSON_AddItemToObject(root, "profiles", arr);
    cJSON_AddStringToObject(root, "active", active.c_str());
    esp_err_t ret = send_json(req, root);
    cJSON_Delete(root);
    return ret;
}

static esp_err_t api_ai_profiles_set(httpd_req_t* req) {
    cJSON* root = get_json_body(req);
    if (!root) return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");

    cJSON* profiles_json = cJSON_GetObjectItem(root, "profiles");
    if (!profiles_json || !cJSON_IsArray(profiles_json)) {
        cJSON_Delete(root);
        return send_success(req, false);
    }

    auto existing = storage::load_ai_profiles();
    std::vector<storage::AIProfile> profiles;
    int n = cJSON_GetArraySize(profiles_json);
    for (int i = 0; i < n; i++) {
        cJSON* obj = cJSON_GetArrayItem(profiles_json, i);
        storage::AIProfile p;
        cJSON* id = cJSON_GetObjectItem(obj, "id");
        cJSON* name = cJSON_GetObjectItem(obj, "name");
        cJSON* url = cJSON_GetObjectItem(obj, "api_url");
        cJSON* key = cJSON_GetObjectItem(obj, "api_key");
        cJSON* model = cJSON_GetObjectItem(obj, "model");
        cJSON* per = cJSON_GetObjectItem(obj, "personality");
        if (id && cJSON_IsString(id)) p.id = id->valuestring;
        if (name && cJSON_IsString(name)) p.name = name->valuestring;
        if (url && cJSON_IsString(url)) p.api_url = url->valuestring;
        if (key && cJSON_IsString(key)) {
            std::string key_val = key->valuestring;
            if (key_val.find("******") != std::string::npos) {
                for (const auto& e : existing) {
                    if (e.id == p.id) { key_val = e.api_key; break; }
                }
            }
            p.api_key = key_val;
        }
        if (model && cJSON_IsString(model)) p.model = model->valuestring;
        if (per && cJSON_IsString(per)) p.personality = per->valuestring;
        if (p.id.empty()) p.id = std::to_string(esp_random());
        if (p.name.empty()) p.name = p.model.empty() ? "Profile" : p.model;
        profiles.push_back(p);
    }

    bool ok = storage::save_ai_profiles(profiles);
    if (ok) log("AI profiles saved: " + std::to_string(profiles.size()));
    cJSON_Delete(root);
    return send_success(req, ok);
}

static esp_err_t api_ai_profiles_delete(httpd_req_t* req) {
    cJSON* root = get_json_body(req);
    if (!root) return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");

    cJSON* id = cJSON_GetObjectItem(root, "id");
    bool ok = (id && cJSON_IsString(id));
    if (ok) {
        auto profiles = storage::load_ai_profiles();
        profiles.erase(std::remove_if(profiles.begin(), profiles.end(),
            [&](const storage::AIProfile& p) { return p.id == id->valuestring; }), profiles.end());
        ok = storage::save_ai_profiles(profiles);
        if (ok) log("AI profile deleted: " + std::string(id->valuestring));
    }
    cJSON_Delete(root);
    return send_success(req, ok);
}

static esp_err_t api_ai_active_get(httpd_req_t* req) {
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "id", storage::load_ai_active().c_str());
    esp_err_t ret = send_json(req, root);
    cJSON_Delete(root);
    return ret;
}

static esp_err_t api_ai_active_set(httpd_req_t* req) {
    cJSON* root = get_json_body(req);
    if (!root) return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");

    cJSON* id = cJSON_GetObjectItem(root, "id");
    bool ok = (id && cJSON_IsString(id));
    if (ok) {
        ok = storage::save_ai_active(id->valuestring);
        if (ok) log("AI active profile: " + std::string(id->valuestring));
    }
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
    cJSON_AddNumberToObject(cpu, "temp", status.temp_celsius);
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
    auto chats = storage::chat_list();
    cJSON* arr = cJSON_CreateArray();
    for (const auto& c : chats) {
        cJSON* obj = cJSON_CreateObject();
        cJSON_AddStringToObject(obj, "id", c.id.c_str());
        cJSON_AddStringToObject(obj, "title", c.title.c_str());
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
    cJSON* title = cJSON_GetObjectItem(root, "title");
    cJSON* messages = cJSON_GetObjectItem(root, "messages");
    bool ok = (id && messages && cJSON_IsString(id));
    if (ok) {
        cJSON* data = cJSON_CreateObject();
        cJSON_AddStringToObject(data, "title", (title && cJSON_IsString(title)) ? title->valuestring : id->valuestring);
        cJSON_AddItemReferenceToObject(data, "messages", messages);
        char* str = cJSON_PrintUnformatted(data);
        ok = storage::chat_save(id->valuestring, str);
        free(str);
        cJSON_Delete(data);
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

static esp_err_t api_reboot(httpd_req_t* req) {
    log("Reboot requested");
    send_success(req, true);
    vTaskDelay(pdMS_TO_TICKS(500));
    esp_restart();
    return ESP_OK;
}

static esp_err_t api_factory_reset(httpd_req_t* req) {
    log("Factory reset requested");
    bool ok = storage::factory_reset();
    send_success(req, ok);
    if (ok) {
        vTaskDelay(pdMS_TO_TICKS(500));
        esp_restart();
    }
    return ESP_OK;
}

static esp_err_t api_chat_stream(httpd_req_t* req) {
    cJSON* root = get_json_body(req);
    if (!root) return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");

    cJSON* id = cJSON_GetObjectItem(root, "id");
    cJSON* messages = cJSON_GetObjectItem(root, "messages");
    if (!id || !cJSON_IsString(id) || !messages || !cJSON_IsArray(messages)) {
        cJSON_Delete(root);
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Missing fields");
    }

    auto profiles = storage::load_ai_profiles();
    std::string active_id = storage::load_ai_active();
    storage::AIProfile profile;
    bool found = false;
    for (const auto& p : profiles) {
        if (p.id == active_id) { profile = p; found = true; break; }
    }
    if (!found && !profiles.empty()) { profile = profiles[0]; active_id = profile.id; }
    if (!found && profiles.empty()) {
        cJSON_Delete(root);
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "AI not configured");
    }
    if (profile.api_url.empty() || profile.api_key.empty() || profile.model.empty()) {
        cJSON_Delete(root);
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "AI not configured");
    }

    cJSON* body = cJSON_CreateObject();
    cJSON_AddStringToObject(body, "model", profile.model.c_str());
    cJSON_AddNumberToObject(body, "temperature", 0.7);
    cJSON_AddItemReferenceToObject(body, "messages", messages);
    cJSON_AddBoolToObject(body, "stream", true);
    char* post_data = cJSON_PrintUnformatted(body);
    cJSON_Delete(body);

    int post_len = strlen(post_data);
    std::string auth_header = "Bearer " + profile.api_key;

    esp_http_client_config_t client_cfg = {};
    client_cfg.url = profile.api_url.c_str();
    client_cfg.method = HTTP_METHOD_POST;
    client_cfg.timeout_ms = 60000;
    client_cfg.buffer_size = 2048;
    client_cfg.buffer_size_tx = 4096;

    esp_http_client_handle_t client = esp_http_client_init(&client_cfg);
    if (!client) {
        free(post_data);
        cJSON_Delete(root);
        return httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "HTTP client init failed");
    }

    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_header(client, "Authorization", auth_header.c_str());
    esp_http_client_set_post_field(client, post_data, post_len);

    esp_err_t err = esp_http_client_open(client, -1);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "AI connection failed: %s", esp_err_to_name(err));
        free(post_data);
        cJSON_Delete(root);
        esp_http_client_cleanup(client);
        return httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to connect to AI");
    }

    esp_http_client_fetch_headers(client);
    int status = esp_http_client_get_status_code(client);
    if (status != 200) {
        ESP_LOGE(TAG, "AI API returned %d", status);
        free(post_data);
        cJSON_Delete(root);
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        return httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "AI API error");
    }

    httpd_resp_set_type(req, "text/event-stream");
    httpd_resp_set_hdr(req, "Cache-Control", "no-cache, no-transform");
    httpd_resp_set_hdr(req, "Connection", "keep-alive");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type");

    char buffer[512];
    std::string line_buffer;
    int read_len;

    while ((read_len = esp_http_client_read(client, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[read_len] = '\0';
        line_buffer += buffer;

        size_t pos;
        while ((pos = line_buffer.find('\n')) != std::string::npos) {
            std::string line = line_buffer.substr(0, pos);
            line_buffer.erase(0, pos + 1);
            if (!line.empty() && line.back() == '\r') line.pop_back();

            if (line.substr(0, 6) == "data: ") {
                std::string data = line.substr(6);
                if (data == "[DONE]") {
                    httpd_resp_sendstr_chunk(req, "data: [DONE]\n\n");
                    continue;
                }

                cJSON* chunk = cJSON_Parse(data.c_str());
                if (chunk) {
                    cJSON* choices = cJSON_GetObjectItem(chunk, "choices");
                    if (choices && cJSON_GetArraySize(choices) > 0) {
                        cJSON* choice = cJSON_GetArrayItem(choices, 0);
                        cJSON* delta = cJSON_GetObjectItem(choice, "delta");
                        if (delta) {
                            cJSON* content = cJSON_GetObjectItem(delta, "content");
                            if (content && cJSON_IsString(content) && content->valuestring) {
                                std::string sse = "data: " + std::string(content->valuestring) + "\n\n";
                                httpd_resp_sendstr_chunk(req, sse.c_str());
                            }
                        }
                    }
                    cJSON_Delete(chunk);
                }
            }
        }
    }

    httpd_resp_sendstr_chunk(req, nullptr);
    free(post_data);
    cJSON_Delete(root);
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    return ESP_OK;
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
    { "/api/ai/profiles", HTTP_GET, api_ai_profiles_get, nullptr },
    { "/api/ai/profiles", HTTP_POST, api_ai_profiles_set, nullptr },
    { "/api/ai/profiles/delete", HTTP_POST, api_ai_profiles_delete, nullptr },
    { "/api/ai/active", HTTP_GET, api_ai_active_get, nullptr },
    { "/api/ai/active", HTTP_POST, api_ai_active_set, nullptr },
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
    { "/api/chat/stream", HTTP_POST, api_chat_stream, nullptr },
    { "/api/reboot", HTTP_POST, api_reboot, nullptr },
    { "/api/factory_reset", HTTP_POST, api_factory_reset, nullptr },
};

namespace web {

bool start(LogCallback log_cb) {
    s_log_cb = log_cb;

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 16384;
    config.max_uri_handlers = 32;
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
