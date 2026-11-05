#pragma once

#include <cstdint>

namespace monitor {

struct SystemStatus {
    uint8_t cpu_percent;
    uint8_t mem_percent;
    uint8_t flash_percent;
    uint32_t mem_total;
    uint32_t mem_free;
    uint32_t flash_total;
    uint32_t flash_used;
};

void init();
SystemStatus get_status();

}
