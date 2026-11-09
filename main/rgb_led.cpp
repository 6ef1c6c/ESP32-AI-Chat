#include "rgb_led.h"
#include "esp_log.h"
#include "led_strip.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "rgb_led";

static led_strip_handle_t s_strip = nullptr;
static rgb::RGBConfig s_config = {};
static int s_current_pin = -1;
static TaskHandle_t s_task = nullptr;
static bool s_running = false;

static void update_led(uint8_t r, uint8_t g, uint8_t b) {
    if (!s_strip) return;
    led_strip_set_pixel(s_strip, 0, r, g, b);
    led_strip_refresh(s_strip);
}

static void rgb_task(void* arg) {
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
                const uint32_t ms_per_phase = 3000;
                uint32_t ms = xTaskGetTickCount() * portTICK_PERIOD_MS;
                uint32_t t = ms % (ms_per_phase * 3);
                uint8_t phase = t / ms_per_phase;
                float ratio = (float)(t % ms_per_phase) / ms_per_phase;

                uint8_t r = 0, g = 0, b = 0;
                switch (phase) {
                    case 0: r = (uint8_t)(255 * (1.0f - ratio)); g = (uint8_t)(255 * ratio); break;
                    case 1: g = (uint8_t)(255 * (1.0f - ratio)); b = (uint8_t)(255 * ratio); break;
                    case 2: b = (uint8_t)(255 * (1.0f - ratio)); r = (uint8_t)(255 * ratio); break;
                }
                update_led(r, g, b);
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
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_RGB,
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
