#pragma once

#include <string>
#include <functional>
#include "storage.h"

namespace wifi {

using OnStatusChange = std::function<void(const std::string& status)>;

struct ScannedAP {
    std::string ssid;
    int rssi;
    std::string auth;
};

bool init();
void register_callbacks(OnStatusChange status_cb);
bool start_ap(const storage::APConfig& cfg);
bool connect_sta(const std::string& ssid, const std::string& password);
void disconnect_sta();
std::vector<ScannedAP> scan_networks();
bool scan_and_connect_saved();
bool is_sta_connected();
std::string get_sta_ip();
void apply_dns();
void enable_nat();
void run_connectivity_test();

}
