#include "led.h"
#include "config.h"
#include "utils.h"
#include "Arduino.h"

#include <cstdint>
#include "esp32-hal-ledc.h"

void Led::setup() {
    brightness = 0;
    update_interval = Config::LED_PWM_UPDATE_INTERVAL_MS;
    last_update_time = 0;
    is_max_brightness_requested = false;

    ledcSetup(Config::LED_PWM_CHANNEL, Config::LED_PWM_FREQ, Config::LED_PWM_RESOLUTION_BIT);
    ledcAttachPin(Config::GPIO_LED, Config::LED_PWM_CHANNEL);
    set_brightness(0);
}

void Led::update(const uint32_t brightness) {
    INTERVAL_DEBUG(1000,
                   "Led: input: %u | "
                   "brightness: %u | "
                   "update_interval: %u | "
                   "is_max_brightness_requested: %u\n",
                   brightness, this->brightness, update_interval, is_max_brightness_requested);


    if (is_max_brightness_requested) {
        set_brightness(Config::LED_PWM_MAX_BRIGHTNESS);
    }

    if ((g_time_now - last_update_time) >= update_interval) {
        if (is_max_brightness_requested) {
            is_max_brightness_requested = false;
            update_interval = Config::LED_PWM_UPDATE_INTERVAL_MS;
        }

        set_brightness(convert_input_brightness(brightness));
        last_update_time = g_time_now;
    }
}

void Led::request_max_brightness() {
    INTERVAL_DEBUG(1000, "Led: requested max brightness\n");
    is_max_brightness_requested = true; 
    last_update_time = g_time_now;
    update_interval = Config::LED_MAX_BRIGHTNESS_HOLD_MS;
}

void Led::set_brightness(const uint32_t value) {
    if (value == brightness) {
        return;
    }
    brightness = value;
    ledcWrite(Config::LED_PWM_CHANNEL, brightness);
}

uint16_t Led::convert_input_brightness(const uint16_t input_brightness) const {
    uint16_t brightness = 0;

    if (input_brightness <= Config::LED_PWM_MIN_THRESHOLD) {
        brightness = Config::LED_PWM_MAX_BRIGHTNESS;
    } else if (input_brightness >= Config::LED_PWM_MAX_THRESHOLD) {
        brightness = Config::LED_PWM_MIN_BRIGHTNESS;
    } else {
        brightness = constrain(
            map(
                input_brightness,
                Config::LED_PWM_MIN_THRESHOLD,
                Config::LED_PWM_MAX_THRESHOLD,
                Config::LED_PWM_MAX_BRIGHTNESS,
                Config::LED_PWM_MIN_BRIGHTNESS),
            Config::LED_PWM_MIN_BRIGHTNESS,
            Config::LED_PWM_MAX_BRIGHTNESS);
    }

    return brightness;
}
