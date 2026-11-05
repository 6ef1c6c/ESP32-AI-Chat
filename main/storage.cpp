#include "storage.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_spiffs.h"
#include "cJSON.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <algorithm>

static const char* TAG = "storage";
static const char* NVS_NS = "esp_c6_ai";
static const char* KEY_WIFI_CREDS = "wifi_creds";
static const char* KEY_AP_CONFIG = "ap_config";
static const char* KEY_LANGUAGE = "language";
static const char* KEY_AI_CONFIG = "ai_config";
static const char* KEY_LOG_LEVEL = "log_level";
static const char* KEY_RGB_CONFIG = "rgb_config";

namespace storage {

bool init() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = nullptr,
        .max_files = 5,
        .format_if_mount_failed = true,
    };

    ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPIFFS mount failed: %s", esp_err_to_name(ret));
        return false;
    }

    size_t total = 0, used = 0;
    if (esp_spiffs_info(nullptr, &total, &used) == ESP_OK) {
        ESP_LOGI(TAG, "SPIFFS: total=%d, used=%d", total, used);
    }
    return true;
}

static nvs_handle_t open_nvs() {
    nvs_handle_t handle;
    if (nvs_open(NVS_NS, NVS_READWRITE, &handle) != ESP_OK) return 0;
    return handle;
}

static bool nvs_save_str(const char* key, const char* value) {
    nvs_handle_t h = open_nvs();
    if (!h) return false;
    esp_err_t ret = nvs_set_str(h, key, value);
    if (ret == ESP_OK) ret = nvs_commit(h);
    nvs_close(h);
    return ret == ESP_OK;
}

static std::string nvs_load_str(const char* key) {
    nvs_handle_t h = open_nvs();
    if (!h) return "";

    size_t len = 0;
    esp_err_t ret = nvs_get_str(h, key, nullptr, &len);
    if (ret != ESP_OK || len == 0) {
        nvs_close(h);
        return "";
    }

    std::string result(len, '\0');
    ret = nvs_get_str(h, key, &result[0], &len);
    nvs_close(h);
    if (ret != ESP_OK) return "";
    result.resize(strlen(result.c_str()));
    return result;
}

static bool save_json(const char* key, cJSON* obj) {
    char* str = cJSON_PrintUnformatted(obj);
    bool ok = nvs_save_str(key, str);
    free(str);
    return ok;
}

static cJSON* load_json(const char* key) {
    std::string data = nvs_load_str(key);
    if (data.empty()) return nullptr;
    return cJSON_Parse(data.c_str());
}

bool save_wifi_creds(const std::vector<WiFiCredential>& creds) {
    cJSON* arr = cJSON_CreateArray();
    for (const auto& c : creds) {
        cJSON* obj = cJSON_CreateObject();
        cJSON_AddStringToObject(obj, "ssid", c.ssid.c_str());
        cJSON_AddStringToObject(obj, "password", c.password.c_str());
        cJSON_AddItemToArray(arr, obj);
    }
    bool ok = save_json(KEY_WIFI_CREDS, arr);
    cJSON_Delete(arr);
    return ok;
}

std::vector<WiFiCredential> load_wifi_creds() {
    std::vector<WiFiCredential> result;
    cJSON* arr = load_json(KEY_WIFI_CREDS);
    if (!arr) return result;

    int n = cJSON_GetArraySize(arr);
    for (int i = 0; i < n; i++) {
        cJSON* obj = cJSON_GetArrayItem(arr, i);
        cJSON* ssid = cJSON_GetObjectItem(obj, "ssid");
        cJSON* pass = cJSON_GetObjectItem(obj, "password");
        if (ssid && pass) result.push_back({ssid->valuestring, pass->valuestring});
    }
    cJSON_Delete(arr);
    return result;
}

bool add_wifi_cred(const std::string& ssid, const std::string& password) {
    auto creds = load_wifi_creds();
    creds.erase(std::remove_if(creds.begin(), creds.end(),
        [&](const WiFiCredential& c){ return c.ssid == ssid; }), creds.end());
    creds.push_back({ssid, password});
    return save_wifi_creds(creds);
}

bool remove_wifi_cred(const std::string& ssid) {
    auto creds = load_wifi_creds();
    creds.erase(std::remove_if(creds.begin(), creds.end(),
        [&](const WiFiCredential& c){ return c.ssid == ssid; }), creds.end());
    return save_wifi_creds(creds);
}

bool save_ap_config(const APConfig& cfg) {
    cJSON* obj = cJSON_CreateObject();
    cJSON_AddStringToObject(obj, "ssid", cfg.ssid.c_str());
    cJSON_AddStringToObject(obj, "password", cfg.password.c_str());
    bool ok = save_json(KEY_AP_CONFIG, obj);
    cJSON_Delete(obj);
    return ok;
}

APConfig load_ap_config() {
    APConfig cfg;
    cJSON* obj = load_json(KEY_AP_CONFIG);
    if (obj) {
        cJSON* ssid = cJSON_GetObjectItem(obj, "ssid");
        cJSON* pass = cJSON_GetObjectItem(obj, "password");
        if (ssid) cfg.ssid = ssid->valuestring;
        if (pass) cfg.password = pass->valuestring;
        cJSON_Delete(obj);
    }
    return cfg;
}

bool save_language(const std::string& lang) {
    return nvs_save_str(KEY_LANGUAGE, lang.c_str());
}

std::string load_language() {
    std::string lang = nvs_load_str(KEY_LANGUAGE);
    return lang.empty() ? "zh" : lang;
}

bool save_ai_config(const AIConfig& cfg) {
    cJSON* obj = cJSON_CreateObject();
    cJSON_AddStringToObject(obj, "api_url", cfg.api_url.c_str());
    cJSON_AddStringToObject(obj, "api_key", cfg.api_key.c_str());
    cJSON_AddStringToObject(obj, "personality", cfg.personality.c_str());
    bool ok = save_json(KEY_AI_CONFIG, obj);
    cJSON_Delete(obj);
    return ok;
}

AIConfig load_ai_config() {
    AIConfig cfg;
    cJSON* obj = load_json(KEY_AI_CONFIG);
    if (obj) {
        cJSON* url = cJSON_GetObjectItem(obj, "api_url");
        cJSON* key = cJSON_GetObjectItem(obj, "api_key");
        cJSON* per = cJSON_GetObjectItem(obj, "personality");
        if (url) cfg.api_url = url->valuestring;
        if (key) cfg.api_key = key->valuestring;
        if (per) cfg.personality = per->valuestring;
        cJSON_Delete(obj);
    }
    return cfg;
}

bool save_log_level(LogLevel level) {
    nvs_handle_t h = open_nvs();
    if (!h) return false;
    esp_err_t ret = nvs_set_i8(h, KEY_LOG_LEVEL, static_cast<int8_t>(level));
    if (ret == ESP_OK) ret = nvs_commit(h);
    nvs_close(h);
    return ret == ESP_OK;
}

LogLevel load_log_level() {
    nvs_handle_t h = open_nvs();
    if (!h) return LogLevel::INFO;

    int8_t value = 2;
    esp_err_t ret = nvs_get_i8(h, KEY_LOG_LEVEL, &value);
    nvs_close(h);

    if (ret != ESP_OK || value < 0 || value > 3) return LogLevel::INFO;
    return static_cast<LogLevel>(value);
}

bool save_rgb_config(const RGBConfig& cfg) {
    cJSON* obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(obj, "mode", cfg.mode);
    cJSON_AddNumberToObject(obj, "pin", cfg.pin);
    bool ok = save_json(KEY_RGB_CONFIG, obj);
    cJSON_Delete(obj);
    return ok;
}

RGBConfig load_rgb_config() {
    RGBConfig cfg;
    cJSON* obj = load_json(KEY_RGB_CONFIG);
    if (obj) {
        cJSON* mode = cJSON_GetObjectItem(obj, "mode");
        cJSON* pin = cJSON_GetObjectItem(obj, "pin");
        if (mode && cJSON_IsNumber(mode)) cfg.mode = mode->valueint;
        if (pin && cJSON_IsNumber(pin)) cfg.pin = pin->valueint;
        cJSON_Delete(obj);
    }
    return cfg;
}

static std::string chat_path(const std::string& chat_id) {
    return "/spiffs/chat_" + chat_id + ".json";
}

bool chat_save(const std::string& chat_id, const std::string& json_data) {
    FILE* f = fopen(chat_path(chat_id).c_str(), "w");
    if (!f) return false;
    fwrite(json_data.c_str(), 1, json_data.length(), f);
    fclose(f);
    return true;
}

std::string chat_load(const std::string& chat_id) {
    FILE* f = fopen(chat_path(chat_id).c_str(), "r");
    if (!f) return "[]";
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    std::string data;
    data.resize(size);
    fread(&data[0], 1, size, f);
    fclose(f);
    return data.empty() ? "[]" : data;
}

std::vector<std::string> chat_list() {
    std::vector<std::string> result;
    DIR* dir = opendir("/spiffs");
    if (!dir) return result;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name.find("chat_") == 0 && name.find(".json") != std::string::npos) {
            result.push_back(name.substr(5, name.length() - 10));
        }
    }
    closedir(dir);
    return result;
}

bool chat_delete(const std::string& chat_id) {
    return remove(chat_path(chat_id).c_str()) == 0;
}

}
