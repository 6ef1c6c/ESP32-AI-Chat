#include "wifi_manager.h"
#include "storage.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_random.h"
#include "nvs_flash.h"
#include "ping/ping_sock.h"
#include "lwip/inet.h"
#include "lwip/ip_addr.h"
#include "lwip/ip4_addr.h"
#include "lwip/ip6_addr.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/lwip_napt.h"
#include <cstring>
#include <vector>
#include <string>
#include <algorithm>

static const char* TAG = "wifi_mgr";

static esp_netif_t* s_sta_netif = nullptr;
static esp_netif_t* s_ap_netif = nullptr;
static wifi::OnStatusChange s_status_cb = nullptr;
static bool s_sta_connected = false;
static std::string s_sta_ssid;

static void notify_status(const std::string& status) {
    ESP_LOGI(TAG, "%s", status.c_str());
    if (s_status_cb) s_status_cb(status);
}

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        notify_status("STA started");
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        s_sta_connected = false;
        notify_status("STA disconnected");
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
        notify_status("AP client connected: " + std::to_string(((wifi_event_ap_staconnected_t*)event_data)->aid));
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        notify_status("AP client disconnected: " + std::to_string(((wifi_event_ap_stadisconnected_t*)event_data)->aid));
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        char ip_str[16];
        esp_ip4addr_ntoa(&((ip_event_got_ip_t*)event_data)->ip_info.ip, ip_str, sizeof(ip_str));
        s_sta_connected = true;
        notify_status("STA connected: " + s_sta_ssid + " IP: " + ip_str);
        wifi::apply_dns();
        wifi::enable_nat();
        wifi::run_connectivity_test();
    }
}

static void setup_random_mac_and_hostname() {
    uint8_t sta_mac[6];
    esp_fill_random(sta_mac, sizeof(sta_mac));
    sta_mac[0] = (sta_mac[0] & ~0x01) | 0x02; // locally administered, unicast
    ESP_ERROR_CHECK(esp_wifi_set_mac(WIFI_IF_STA, sta_mac));

    uint8_t ap_mac[6];
    esp_fill_random(ap_mac, sizeof(ap_mac));
    ap_mac[0] = (ap_mac[0] & ~0x01) | 0x02;
    if (memcmp(ap_mac, sta_mac, sizeof(ap_mac)) == 0) {
        ap_mac[5] ^= 0x01;
    }
    ESP_ERROR_CHECK(esp_wifi_set_mac(WIFI_IF_AP, ap_mac));
}

namespace wifi {

bool init() {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    s_sta_netif = esp_netif_create_default_wifi_sta();
    s_ap_netif = esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    setup_random_mac_and_hostname();
    ESP_ERROR_CHECK(esp_netif_set_hostname(s_sta_netif, "ESP32-AI-Chat"));
    ESP_ERROR_CHECK(esp_netif_set_hostname(s_ap_netif, "ESP32-AI-Chat"));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, nullptr));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, nullptr));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_start());

    notify_status("WiFi initialized");
    return true;
}

void register_callbacks(OnStatusChange status_cb) {
    s_status_cb = status_cb;
}

bool start_ap(const storage::APConfig& cfg) {
    esp_netif_ip_info_t ip_info;
    IP4_ADDR(&ip_info.ip, 192, 168, 4, 1);
    IP4_ADDR(&ip_info.gw, 192, 168, 4, 1);
    IP4_ADDR(&ip_info.netmask, 255, 255, 255, 0);
    esp_netif_dhcps_stop(s_ap_netif);
    esp_netif_set_ip_info(s_ap_netif, &ip_info);

    uint32_t dns = ip_info.gw.addr;
    esp_netif_dhcps_option(s_ap_netif, ESP_NETIF_OP_SET, ESP_NETIF_DOMAIN_NAME_SERVER, &dns, sizeof(dns));
    esp_netif_dhcps_start(s_ap_netif);

    wifi_config_t ap_config = {};
    std::string ssid = cfg.ssid.empty() ? "ESP32 AI Chat" : cfg.ssid;
    std::string pass = cfg.password.empty() ? "ESP123456" : cfg.password;
    strncpy((char*)ap_config.ap.ssid, ssid.c_str(), sizeof(ap_config.ap.ssid) - 1);
    strncpy((char*)ap_config.ap.password, pass.c_str(), sizeof(ap_config.ap.password) - 1);
    ap_config.ap.ssid_len = ssid.length();
    ap_config.ap.channel = 6;
    ap_config.ap.authmode = pass.length() < 8 ? WIFI_AUTH_OPEN : WIFI_AUTH_WPA2_PSK;
    ap_config.ap.max_connection = 4;

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    notify_status("AP started: " + ssid);
    return true;
}

bool connect_sta(const std::string& ssid, const std::string& password) {
    if (ssid.empty()) return false;
    s_sta_ssid = ssid;
    esp_wifi_disconnect();

    wifi_config_t sta_config = {};
    strncpy((char*)sta_config.sta.ssid, ssid.c_str(), sizeof(sta_config.sta.ssid) - 1);
    strncpy((char*)sta_config.sta.password, password.c_str(), sizeof(sta_config.sta.password) - 1);
    sta_config.sta.threshold.authmode = password.empty() ? WIFI_AUTH_OPEN : WIFI_AUTH_WPA2_PSK;

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
    ESP_ERROR_CHECK(esp_wifi_connect());
    notify_status("Connecting: " + ssid);
    return true;
}

static std::string auth_mode_to_str(wifi_auth_mode_t auth) {
    switch (auth) {
        case WIFI_AUTH_OPEN: return "OPEN";
        case WIFI_AUTH_WEP: return "WEP";
        case WIFI_AUTH_WPA_PSK: return "WPA";
        case WIFI_AUTH_WPA2_PSK: return "WPA2";
        case WIFI_AUTH_WPA_WPA2_PSK: return "WPA/WPA2";
        case WIFI_AUTH_ENTERPRISE: return "Enterprise";
        case WIFI_AUTH_WPA3_PSK: return "WPA3";
        case WIFI_AUTH_WPA2_WPA3_PSK: return "WPA2/WPA3";
        case WIFI_AUTH_WAPI_PSK: return "WAPI";
        case WIFI_AUTH_OWE: return "OWE";
        default: return "Unknown";
    }
}

static std::vector<wifi_ap_record_t> do_scan() {
    wifi_scan_config_t scan_conf = {};
    scan_conf.scan_type = WIFI_SCAN_TYPE_ACTIVE;
    scan_conf.scan_time.active.min = 100;
    scan_conf.scan_time.active.max = 300;

    if (esp_wifi_scan_start(&scan_conf, true) != ESP_OK) {
        notify_status("WiFi scan failed");
        return {};
    }

    uint16_t num = 0;
    esp_wifi_scan_get_ap_num(&num);
    if (num == 0) {
        esp_wifi_scan_get_ap_records(&num, nullptr);
        return {};
    }

    std::vector<wifi_ap_record_t> records(num);
    if (esp_wifi_scan_get_ap_records(&num, records.data()) != ESP_OK) {
        return {};
    }
    return records;
}

std::vector<ScannedAP> scan_networks() {
    std::vector<ScannedAP> result;
    auto records = do_scan();

    for (const auto& r : records) {
        std::string ssid((const char*)r.ssid);
        if (ssid.empty()) continue;
        auto it = std::find_if(result.begin(), result.end(), [&](const ScannedAP& ap) { return ap.ssid == ssid; });
        if (it == result.end()) {
            result.push_back({ssid, r.rssi, auth_mode_to_str(r.authmode)});
        } else if (r.rssi > it->rssi) {
            it->rssi = r.rssi;
            it->auth = auth_mode_to_str(r.authmode);
        }
    }
    std::sort(result.begin(), result.end(), [](const ScannedAP& a, const ScannedAP& b) { return a.rssi > b.rssi; });
    notify_status("WiFi scan found " + std::to_string(result.size()) + " networks");
    return result;
}

void disconnect_sta() {
    esp_wifi_disconnect();
    s_sta_connected = false;
}

bool scan_and_connect_saved() {
    notify_status("Scanning WiFi...");
    auto records = do_scan();
    if (records.empty()) {
        notify_status("No WiFi found");
        return false;
    }

    auto saved = storage::load_wifi_creds();
    for (const auto& ap : records) {
        std::string found_ssid((char*)ap.ssid);
        for (const auto& cred : saved) {
            if (cred.ssid == found_ssid) {
                notify_status("Found saved network: " + found_ssid + " RSSI: " + std::to_string(ap.rssi));
                return connect_sta(cred.ssid, cred.password);
            }
        }
    }

    notify_status("No saved WiFi matched");
    return false;
}

bool is_sta_connected() {
    return s_sta_connected;
}

std::string get_sta_ip() {
    if (!s_sta_connected || !s_sta_netif) return "";
    esp_netif_ip_info_t info;
    if (esp_netif_get_ip_info(s_sta_netif, &info) != ESP_OK) return "";
    char ip_str[16];
    esp_ip4addr_ntoa(&info.ip, ip_str, sizeof(ip_str));
    return ip_str;
}

void apply_dns() {
    esp_netif_dns_info_t dns_v6 = { .ip = ESP_IP6ADDR_INIT(0x24003200, 0, 0, 1) };
    esp_netif_set_dns_info(s_sta_netif, ESP_NETIF_DNS_MAIN, &dns_v6);

    esp_netif_dns_info_t dns_v4 = { .ip = ESP_IP4ADDR_INIT(223, 5, 5, 5) };
    esp_netif_set_dns_info(s_sta_netif, ESP_NETIF_DNS_BACKUP, &dns_v4);

    notify_status("DNS set: IPv6 2400:3200::1, IPv4 223.5.5.5");
}

void enable_nat() {
    if (!s_sta_netif || !s_ap_netif) return;
    esp_netif_ip_info_t ap_ip;
    if (esp_netif_get_ip_info(s_ap_netif, &ap_ip) != ESP_OK) return;
    ip_napt_enable(ap_ip.ip.addr, 1);
    notify_status("AP clients share STA internet");
}

struct PingCtx {
    bool is_baidu;
    esp_ping_handle_t handle;
};

static void ping_success(esp_ping_handle_t hdl, void* args) {
    PingCtx* ctx = (PingCtx*)args;
    uint32_t elapsed, seq;
    ip_addr_t target_addr;
    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seq, sizeof(seq));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed, sizeof(elapsed));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));

    std::string result = std::string(ipaddr_ntoa(&target_addr)) + " seq=" + std::to_string(seq) + " time=" + std::to_string(elapsed) + "ms";
    notify_status(std::string(ctx->is_baidu ? "Baidu" : "Google") + ": " + result);
}

static void ping_end(esp_ping_handle_t hdl, void* args) {
    PingCtx* ctx = (PingCtx*)args;
    esp_ping_delete_session(ctx->handle);
    delete ctx;
}

static bool resolve_host_prefer_ipv6(const char* host, ip_addr_t* target) {
    struct addrinfo hints = {};
    struct addrinfo* res = nullptr;
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host, nullptr, &hints, &res) == 0 && res) {
        struct sockaddr_in6* sin6 = (struct sockaddr_in6*)res->ai_addr;
        memcpy(target->u_addr.ip6.addr, &sin6->sin6_addr, sizeof(target->u_addr.ip6.addr));
        IP_SET_TYPE(target, IPADDR_TYPE_V6);
        ip6_addr_clear_zone(&target->u_addr.ip6);
        freeaddrinfo(res);
        return true;
    }

    hints.ai_family = AF_INET;
    if (getaddrinfo(host, nullptr, &hints, &res) == 0 && res) {
        struct sockaddr_in* sin = (struct sockaddr_in*)res->ai_addr;
        inet_addr_to_ip4addr(&target->u_addr.ip4, &sin->sin_addr);
        IP_SET_TYPE(target, IPADDR_TYPE_V4);
        freeaddrinfo(res);
        return true;
    }
    return false;
}

static void run_ping(const char* host, bool is_baidu) {
    ip_addr_t target_addr;
    if (!resolve_host_prefer_ipv6(host, &target_addr)) {
        notify_status(std::string(host) + " resolve failed");
        return;
    }

    PingCtx* ctx = new PingCtx{is_baidu, nullptr};
    esp_ping_config_t config = ESP_PING_DEFAULT_CONFIG();
    config.target_addr = target_addr;
    config.count = 3;
    config.interval_ms = 1000;
    config.timeout_ms = 3000;

    esp_ping_callbacks_t cbs = {
        .cb_args = ctx,
        .on_ping_success = ping_success,
        .on_ping_timeout = nullptr,
        .on_ping_end = ping_end,
    };

    esp_ping_new_session(&config, &cbs, &ctx->handle);
    esp_ping_start(ctx->handle);
}

void run_connectivity_test() {
    notify_status("Running connectivity test...");
    run_ping("www.baidu.com", true);
    run_ping("www.google.com", false);
}

}
