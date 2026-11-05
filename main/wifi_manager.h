#pragma once

#include <string>
#include <functional>
#include "storage.h"

namespace wifi {

struct ConnectionResult {
    bool connected;
    std::string ssid;
    std::string ip;
    int rssi;
    std::string ping_baidu;
    std::string ping_google;
};

using OnStatusChange = std::function<void(const std::string& status)>;
using OnConnected = std::function<void(const ConnectionResult& result)>;

struct ScannedAP {
    std::string ssid;
    int rssi;
    std::string auth;
};

bool init();
void register_callbacks(OnStatusChange status_cb, OnConnected conn_cb);
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
