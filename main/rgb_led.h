#pragma once

namespace rgb {

enum class Mode {
    OFF = 0,
    RED = 1,
    GREEN = 2,
    BLUE = 3,
    GRADIENT = 4
};

struct RGBConfig {
    Mode mode = Mode::OFF;
    int pin = 8;
};

bool init(const RGBConfig& cfg);
void set_config(const RGBConfig& cfg);
RGBConfig get_config();
void stop();

}
