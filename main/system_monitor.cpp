#include "system_monitor.h"
#include "esp_heap_caps.h"
#include "esp_littlefs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/temperature_sensor.h"
#include "esp_log.h"
#include <cstring>

static float s_cpu_percent = 0.0f;
static temperature_sensor_handle_t s_temp_sensor = nullptr;
static const char* TAG = "sys_mon";

namespace monitor {

static uint32_t get_idle_ticks(uint32_t& total_ticks) {
    UBaseType_t uxArraySize = uxTaskGetSystemState(nullptr, 0, &total_ticks);
    if (uxArraySize == 0) return 0;

    TaskStatus_t* tasks = (TaskStatus_t*)pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));
    if (!tasks) return 0;

    uxArraySize = uxTaskGetSystemState(tasks, uxArraySize, &total_ticks);

    uint32_t idle_ticks = 0;
    for (UBaseType_t i = 0; i < uxArraySize; i++) {
        if (tasks[i].xTaskNumber && tasks[i].pcTaskName && strncmp(tasks[i].pcTaskName, "IDLE", 4) == 0) {
            idle_ticks += tasks[i].ulRunTimeCounter;
        }
    }

    vPortFree(tasks);
    return idle_ticks;
}

static void monitor_task(void* arg) {
    ESP_LOGI(TAG, "monitor task started");
    uint32_t last_idle = 0;
    uint32_t last_total = 0;
    bool first = true;
    uint32_t dbg_cnt = 0;

    while (true) {
        uint32_t total = 0;
        uint32_t idle = get_idle_ticks(total);

        if (total == 0) {
            ESP_LOGW(TAG, "total=0, run time stats may be disabled");
        }

        if (!first && total > last_total) {
            uint32_t delta_total = total - last_total;
            uint32_t delta_idle = idle - last_idle;
            if (delta_total > 0) {
                float instant = 100.0f - ((float)delta_idle * 100.0f / (float)delta_total);
                if (instant < 0.0f) instant = 0.0f;
                if (instant > 100.0f) instant = 100.0f;
                // 指数移动平均，让数值更平滑且能反映突发负载
                s_cpu_percent = s_cpu_percent * 0.7f + instant * 0.3f;

                if (++dbg_cnt >= 10) {
                    ESP_LOGI(TAG, "dt=%u di=%u instant=%.2f cpu=%.2f", delta_total, delta_idle, instant, s_cpu_percent);
                    dbg_cnt = 0;
                }
            }
        } else {
            first = false;
        }

        last_idle = idle;
        last_total = total;
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void init() {
    temperature_sensor_config_t cfg = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-10, 80);
    temperature_sensor_install(&cfg, &s_temp_sensor);
    temperature_sensor_enable(s_temp_sensor);

    BaseType_t rc = xTaskCreate(monitor_task, "sys_mon", 3072, nullptr, tskIDLE_PRIORITY + 1, nullptr);
    ESP_LOGI(TAG, "task create rc=%d", rc);
}

SystemStatus get_status() {
    SystemStatus status = {};

    status.cpu_percent = s_cpu_percent;

    status.mem_total = heap_caps_get_total_size(MALLOC_CAP_INTERNAL);
    status.mem_free = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    if (status.mem_total > 0) {
        status.mem_percent = ((float)(status.mem_total - status.mem_free) * 100.0f) / (float)status.mem_total;
    }

    size_t flash_total = 0, flash_used = 0;
    if (esp_littlefs_info("littlefs", &flash_total, &flash_used) == ESP_OK && flash_total > 0) {
        status.flash_total = flash_total;
        status.flash_used = flash_used;
        status.flash_percent = ((float)status.flash_used * 100.0f) / (float)status.flash_total;
    }

    if (s_temp_sensor) {
        float temp = 0.0f;
        if (temperature_sensor_get_celsius(s_temp_sensor, &temp) == ESP_OK) {
            status.temp_celsius = temp;
        }
    }

    return status;
}

}
