#include "system_monitor.h"
#include "esp_heap_caps.h"
#include "esp_spiffs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <cstring>

static uint32_t s_last_idle_ticks = 0;
static uint32_t s_last_total_ticks = 0;
static uint8_t s_cpu_percent = 0;

namespace monitor {

void init() {
    get_status();
}

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

SystemStatus get_status() {
    SystemStatus status = {};

    uint32_t total_ticks = 0;
    uint32_t idle_ticks = get_idle_ticks(total_ticks);

    if (s_last_total_ticks != 0 && total_ticks > s_last_total_ticks) {
        uint32_t delta_total = total_ticks - s_last_total_ticks;
        uint32_t delta_idle = idle_ticks - s_last_idle_ticks;
        if (delta_total > 0) {
            s_cpu_percent = 100 - (delta_idle * 100 / delta_total);
            if (s_cpu_percent > 100) s_cpu_percent = 100;
        }
    }

    s_last_idle_ticks = idle_ticks;
    s_last_total_ticks = total_ticks;
    status.cpu_percent = s_cpu_percent;

    status.mem_total = heap_caps_get_total_size(MALLOC_CAP_INTERNAL);
    status.mem_free = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    if (status.mem_total > 0) {
        status.mem_percent = ((status.mem_total - status.mem_free) * 100) / status.mem_total;
    }

    size_t flash_total = 0, flash_used = 0;
    if (esp_spiffs_info(nullptr, &flash_total, &flash_used) == ESP_OK && flash_total > 0) {
        status.flash_total = flash_total;
        status.flash_used = flash_used;
        status.flash_percent = (status.flash_used * 100) / status.flash_total;
    }

    return status;
}

}
