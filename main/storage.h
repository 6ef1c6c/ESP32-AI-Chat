#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace storage {

struct WiFiCredential {
    std::string ssid;
    std::string password;
};

struct APConfig {
    std::string ssid = "ESP32 AI Chat";
    std::string password = "ESP123456";
};

struct AIConfig {
    std::string api_url;
    std::string api_key;
    std::string personality;
};

enum class LogLevel {
    OFF = 0,
    ERROR = 1,
    INFO = 2,
    DEBUG = 3
};

struct RGBConfig {
    int mode = 0;
    int pin = 8;
};

bool init();
bool save_language(const std::string& lang);
std::string load_language();
bool save_ai_config(const AIConfig& cfg);
AIConfig load_ai_config();
bool save_log_level(LogLevel level);
LogLevel load_log_level();
bool save_rgb_config(const RGBConfig& cfg);
RGBConfig load_rgb_config();
bool save_wifi_creds(const std::vector<WiFiCredential>& creds);
std::vector<WiFiCredential> load_wifi_creds();
bool add_wifi_cred(const std::string& ssid, const std::string& password);
bool remove_wifi_cred(const std::string& ssid);
bool save_ap_config(const APConfig& cfg);
APConfig load_ap_config();
bool chat_save(const std::string& chat_id, const std::string& json_data);
std::string chat_load(const std::string& chat_id);
std::vector<std::string> chat_list();
bool chat_delete(const std::string& chat_id);

}
