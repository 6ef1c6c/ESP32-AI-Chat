#include "rgb_led.h"
#include "esp_log.h"
#include "led_strip.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "rgb_led";

static led_strip_handle_t s_strip = nullptr;
static rgb::RGBConfig s_config = {};
static int s_current_pin = -1;
static TaskHandle_t s_task = nullptr;
static bool s_running = false;

static uint8_t hue_to_rgb(float p, float q, float t) {
    if (t < 0) t += 1;
    if (t > 1) t -= 1;
    if (t < 1.0f / 6.0f) return (uint8_t)((p + (q - p) * 6.0f * t) * 255);
    if (t < 1.0f / 2.0f) return (uint8_t)(q * 255);
    if (t < 2.0f / 3.0f) return (uint8_t)((p + (q - p) * (2.0f / 3.0f - t) * 6.0f) * 255);
    return (uint8_t)(p * 255);
}

static void hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v, uint8_t& r, uint8_t& g, uint8_t& b) {
    float hf = h / 360.0f;
    float sf = s / 255.0f;
    float vf = v / 255.0f;
    float q = vf < 0.5f ? vf * (1.0f + sf) : vf + sf - vf * sf;
    float p = 2.0f * vf - q;

    r = hue_to_rgb(p, q, hf + 1.0f / 3.0f);
    g = hue_to_rgb(p, q, hf);
    b = hue_to_rgb(p, q, hf - 1.0f / 3.0f);
}

static void update_led(uint8_t r, uint8_t g, uint8_t b) {
    if (!s_strip) return;
    led_strip_set_pixel(s_strip, 0, r, g, b);
    led_strip_refresh(s_strip);
}

static void rgb_task(void* arg) {
    uint16_t hue = 0;
    while (s_running) {
        switch (s_config.mode) {
            case rgb::Mode::OFF:
                update_led(0, 0, 0);
                break;
            case rgb::Mode::RED:
                update_led(255, 0, 0);
                break;
            case rgb::Mode::GREEN:
                update_led(0, 255, 0);
                break;
            case rgb::Mode::BLUE:
                update_led(0, 0, 255);
                break;
            case rgb::Mode::GRADIENT: {
                uint8_t r, g, b;
                hsv_to_rgb(hue, 255, 255, r, g, b);
                update_led(r, g, b);
                hue += 2;
                if (hue >= 360) hue = 0;
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    update_led(0, 0, 0);
    vTaskDelete(nullptr);
}

static bool create_strip(int pin) {
    if (s_strip) {
        led_strip_del(s_strip);
        s_strip = nullptr;
    }

    led_strip_config_t strip_config = {
        .strip_gpio_num = pin,
        .max_leds = 1,
        .led_model = LED_MODEL_WS2812,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
        .timings = {},
        .flags = {},
    };

    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000,
        .mem_block_symbols = 64,
        .flags = { .with_dma = false }
    };

    if (led_strip_new_rmt_device(&strip_config, &rmt_config, &s_strip) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create LED strip on pin %d", pin);
        s_current_pin = -1;
        return false;
    }
    s_current_pin = pin;
    return true;
}

static void restart_task() {
    if (s_task) {
        s_running = false;
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    s_running = true;
    xTaskCreate(rgb_task, "rgb_task", 2048, nullptr, 5, &s_task);
}

namespace rgb {

bool init(const RGBConfig& cfg) {
    s_config = cfg;
    if (!create_strip(cfg.pin)) return false;
    restart_task();
    ESP_LOGI(TAG, "RGB LED initialized on pin %d, mode %d", cfg.pin, (int)cfg.mode);
    return true;
}

void set_config(const RGBConfig& cfg) {
    s_config = cfg;
    if (cfg.pin != s_current_pin) {
        create_strip(cfg.pin);
        restart_task();
    }
}

RGBConfig get_config() {
    return s_config;
}

void stop() {
    s_running = false;
    vTaskDelay(pdMS_TO_TICKS(100));
    if (s_strip) {
        update_led(0, 0, 0);
        led_strip_del(s_strip);
        s_strip = nullptr;
        s_current_pin = -1;
    }
}

}
