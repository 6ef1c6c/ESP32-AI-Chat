#include "storage.h"
#include "wifi_manager.h"
#include "web_server.h"
#include "system_monitor.h"
#include "rgb_led.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "main";

static void log_to_storage(const std::string& msg) {
    ESP_LOGI(TAG, "[UI-LOG] %s", msg.c_str());
}

static void on_wifi_status(const std::string& status) {
    ESP_LOGI(TAG, "WiFi status: %s", status.c_str());
}

static void apply_log_level() {
    switch (storage::load_log_level()) {
        case storage::LogLevel::OFF: esp_log_level_set("*", ESP_LOG_NONE); break;
        case storage::LogLevel::ERROR: esp_log_level_set("*", ESP_LOG_ERROR); break;
        case storage::LogLevel::INFO: esp_log_level_set("*", ESP_LOG_INFO); break;
        case storage::LogLevel::DEBUG: esp_log_level_set("*", ESP_LOG_DEBUG); break;
    }
}

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "ESP32-C6 AI starting...");

    if (!storage::init()) {
        ESP_LOGE(TAG, "Storage init failed");
        return;
    }

    apply_log_level();
    monitor::init();

    auto rgb_cfg = storage::load_rgb_config();
    rgb::init({static_cast<rgb::Mode>(rgb_cfg.mode), rgb_cfg.pin});

    auto apcfg = storage::load_ap_config();
    if (apcfg.ssid.empty()) {
        apcfg.ssid = "ESP32 AI Chat";
        apcfg.password = "ESP123456";
        storage::save_ap_config(apcfg);
    }

    if (!wifi::init()) {
        ESP_LOGE(TAG, "WiFi init failed");
        return;
    }

    wifi::register_callbacks(on_wifi_status);
    wifi::start_ap(apcfg);

    vTaskDelay(pdMS_TO_TICKS(1000));
    wifi::scan_and_connect_saved();

    if (!web::start(log_to_storage)) {
        ESP_LOGE(TAG, "Web server start failed");
        return;
    }

    ESP_LOGI(TAG, "System ready");

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(5000));
        ESP_LOGI(TAG, "STA connected: %s, IP: %s",
                 wifi::is_sta_connected() ? "yes" : "no",
                 wifi::get_sta_ip().c_str());
    }
}
