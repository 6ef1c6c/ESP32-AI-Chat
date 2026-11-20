#include "system_monitor.h"
#include "esp_heap_caps.h"
#include "esp_littlefs.h"
#include "esp_timer.h"
#include "driver/temperature_sensor.h"

static temperature_sensor_handle_t s_temp_sensor = nullptr;

static size_t s_flash_total = 0;
static size_t s_flash_used = 0;
static int64_t s_last_flash_us = 0;

namespace monitor {

void init() {
    temperature_sensor_config_t cfg = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-10, 80);
    temperature_sensor_install(&cfg, &s_temp_sensor);
    temperature_sensor_enable(s_temp_sensor);
}

SystemStatus get_status() {
    SystemStatus status = {};

    // 内存
    status.mem_total = heap_caps_get_total_size(MALLOC_CAP_INTERNAL);
    status.mem_free = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    if (status.mem_total > 0) {
        status.mem_percent = ((float)(status.mem_total - status.mem_free) * 100.0f) / (float)status.mem_total;
    }

    // 温度
    if (s_temp_sensor) {
        float temp = 0.0f;
        if (temperature_sensor_get_celsius(s_temp_sensor, &temp) == ESP_OK) {
            status.temp_celsius = temp;
        }
    }

    // Flash：每 3 分钟更新一次
    int64_t now = esp_timer_get_time();
    if (s_flash_total == 0 || (now - s_last_flash_us) >= 180000000LL) {
        esp_littlefs_info("littlefs", &s_flash_total, &s_flash_used);
        s_last_flash_us = now;
    }
    status.flash_total = s_flash_total;
    status.flash_used = s_flash_used;
    if (s_flash_total > 0) {
        status.flash_percent = ((float)s_flash_used * 100.0f) / (float)s_flash_total;
    }

    return status;
}

} // namespace monitor
