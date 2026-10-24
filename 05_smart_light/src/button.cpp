#include "button.h"

#include "esp32-hal-gpio.h"
#include "utils.h"
#include "config.h"

void Button::setup(ButtonCallback btn_callback) {
    raw_value = 0;
    stable_value = 0;
    is_pressed = false;

    press_time = 0;
    last_stable_value = 0;
    raw_time = 0;
    callback = btn_callback;

    pinMode(Config::GPIO_BUTTON, INPUT_PULLUP);
}

void Button::update() {
    read();

    INTERVAL_DEBUG(1000, "Button: stable_value: %u | last_stable_value: %u | is_pressed: %u\n",
                   stable_value, last_stable_value, is_pressed);

    if ((g_time_now - press_time) >= Config::BUTTON_UPDATE_INTERVAL_MS) {
        if (!is_pressed && stable_value && !last_stable_value) {
            is_pressed = true;
            press_time = g_time_now;
            if (callback) callback();
        }
    } else {
        is_pressed = false;
    }
}

void Button::read() {
    const uint8_t raw = (digitalRead(Config::GPIO_BUTTON) == LOW);

    if (raw != raw_value) {
        raw_value = raw;
        raw_time = g_time_now;
    }

    if ((stable_value != raw_value) &&
        (g_time_now - raw_time) >= Config::BUTTON_DEBOUNCE_DELAY_MS) {
        last_stable_value = stable_value;
        stable_value = raw_value;
    }
}
