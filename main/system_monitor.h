#pragma once

#include <cstdint>

namespace monitor {

struct SystemStatus {
    float cpu_percent;
    float mem_percent;
    float flash_percent;
    float temp_celsius;
    uint32_t mem_total;
    uint32_t mem_free;
    uint32_t flash_total;
    uint32_t flash_used;
};

void init();
SystemStatus get_status();

}
