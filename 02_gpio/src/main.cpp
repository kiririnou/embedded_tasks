#include "HardwareSerial.h"
#include "esp32-hal-gpio.h"
#include "esp32-hal.h"
#include <Arduino.h>
#include <cstdarg>
#include <cstdint>

constexpr uint8_t PIN_LED_RED = 40;
constexpr uint8_t PIN_LED_BLUE = 41;

constexpr uint8_t PIN_BUTTON_BUILTIN = 0;
constexpr uint8_t PIN_BUTTON_CUSTOM = 42;

constexpr uint32_t DEBOUNCE_DELAY_MS = 30;
constexpr uint32_t BAUD_RATE = 115200;

constexpr uint32_t BLINK_INTERVAL_FAST = 200;
constexpr uint32_t BLINK_INTERVAL_SLOW = 1000;

static uint32_t time_now = 0U;

// Macro for interval based logging
#define INTERVAL_DEBUG(interval, fmt, ...)                                  \
    do {                                                                    \
        static uint32_t time_prev_print = 0U;                               \
        if ((time_now - time_prev_print) >= interval) {                     \
            Serial.printf(fmt __VA_OPT__(,) __VA_ARGS__);                   \
            time_prev_print = time_now;                                     \
        }                                                                   \
    } while(0)

enum class BlinkStyle : uint8_t {
    FastSync,
    SlowAlter,
    Off
};

static BlinkStyle blink_style;

struct ButtonState {
    uint8_t raw_value;
    uint8_t stable_value;
    uint32_t raw_time;
};

static ButtonState button_custom_state = { 0 };
static ButtonState button_builtin_state = { 0 };

void button_read(const uint8_t pin, ButtonState& button_state) {
    const uint8_t raw = digitalRead(pin) == LOW;

    if (raw != button_state.raw_value) {
        button_state.raw_value = raw;
        button_state.raw_time = time_now;
    }

    if ((button_state.stable_value != button_state.raw_value) &&
        (time_now - button_state.raw_time) >= DEBOUNCE_DELAY_MS) {
        button_state.stable_value = button_state.raw_value;
    }
}

void led_blink_fast_sync() {
    static uint8_t state = HIGH;
    static uint32_t prev_time = 0U;

    INTERVAL_DEBUG(BLINK_INTERVAL_FAST,
                "==============================\n"
                "led_blink_fast_sync()\n"
                "state = %u\n"
                "prev_time = %u\n"
                "now = %u\n"
                "==============================\n",
                state, prev_time, time_now);

    if ((time_now - prev_time) >= BLINK_INTERVAL_FAST) {
        digitalWrite(PIN_LED_RED, state);
        digitalWrite(PIN_LED_BLUE, state);
        state = !state;
        prev_time = time_now;
    }
}

void led_blink_slow_alter() {
    static uint8_t state = HIGH;
    static uint32_t prev_time = 0U;

    INTERVAL_DEBUG(BLINK_INTERVAL_SLOW,
                "==============================\n"
                "led_blink_slow_alter()\n"
                "state = %u\n"
                "prev_time = %u\n"
                "now = %u\n"
                "==============================\n",
                state, prev_time, time_now);

    if ((time_now - prev_time) >= BLINK_INTERVAL_SLOW) {
        digitalWrite(PIN_LED_RED, state);
        digitalWrite(PIN_LED_BLUE, !state);
        state = !state;
        prev_time = time_now;
    }
}

void setup() {
    Serial.begin(BAUD_RATE);

    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_BLUE, OUTPUT);

    pinMode(PIN_BUTTON_BUILTIN, INPUT_PULLUP);
    pinMode(PIN_BUTTON_CUSTOM, INPUT_PULLUP);

    blink_style = BlinkStyle::FastSync;

    Serial.print("Board started...\n");
}

void loop() {
    time_now = static_cast<uint32_t>(millis());

    button_read(PIN_BUTTON_CUSTOM, button_custom_state);
    button_read(PIN_BUTTON_BUILTIN, button_builtin_state);

    INTERVAL_DEBUG(1000,
                "=====================================\n"
                "button_custom_state.raw_value: %u\n"
                "button_custom_state.raw_time: %u\n"
                "button_custom_state.stable_value: %u\n"
                "time_diff: %u\n"
                "=====================================\n",
                button_custom_state.raw_value,
                button_custom_state.raw_time,
                button_custom_state.stable_value,
                (time_now - button_custom_state.raw_time));

    INTERVAL_DEBUG(1000,
                "=====================================\n"
                "button_builtin_state.raw_value: %u\n"
                "button_builtin_state.raw_time: %u\n"
                "button_builtin_state.stable_value: %u\n"
                "time_diff: %u\n"
                "=====================================\n",
                button_builtin_state.raw_value,
                button_builtin_state.raw_time,
                button_builtin_state.stable_value,
                (time_now - button_builtin_state.raw_time));


    if (button_custom_state.stable_value
        && button_builtin_state.stable_value
        && blink_style != BlinkStyle::Off) {
        INTERVAL_DEBUG(500, "Warning: both buttons are updated\n");
        blink_style = BlinkStyle::Off;
    } else if (button_custom_state.stable_value && blink_style != BlinkStyle::FastSync) {
        INTERVAL_DEBUG(500, "Custom button was pressed\n");
        blink_style = BlinkStyle::FastSync;
    } else if (button_builtin_state.stable_value && blink_style != BlinkStyle::SlowAlter) {
        INTERVAL_DEBUG(500, "Builtin button was pressed\n");
        blink_style = BlinkStyle::SlowAlter;
    }

    switch (blink_style) {
    case BlinkStyle::FastSync:
        led_blink_fast_sync();
        break;

    case BlinkStyle::SlowAlter:
        led_blink_slow_alter();
        break;

    case BlinkStyle::Off:
        INTERVAL_DEBUG(1000, "LEDs are off\n");
        digitalWrite(PIN_LED_RED, LOW);
        digitalWrite(PIN_LED_BLUE, LOW);
        break;
    }
}

