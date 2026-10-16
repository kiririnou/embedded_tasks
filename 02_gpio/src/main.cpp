#include "HardwareSerial.h"
#include "esp32-hal-gpio.h"
#include "esp32-hal.h"
#include <Arduino.h>
#include <cstdint>

#define PIN_LED_RED  40
#define PIN_LED_BLUE 41

#define PIN_BUTTON_BUILTIN 0
#define PIN_BUTTON_CUSTOM 42

#define DEBOUNCE_DELAY_MS 30

typedef enum {
    BLINK_STYLE_FAST_SYNC,
    BLINK_STYLE_SLOW_ALTER,
} blink_style_t;

static blink_style_t blink_style;

typedef struct {
    uint32_t last_value;
    uint32_t last_press_time;
    uint32_t current_value;
    uint32_t current_press_time;
} button_state_t;

static button_state_t button_custom_state = { 0 };
static button_state_t button_builtin_state = { 0 };

bool button_is_updated(const uint8_t pin, button_state_t* const button_state) {
    button_state->last_press_time = button_state->current_press_time;
    button_state->current_press_time = millis();

    uint32_t state = digitalRead(pin);
    // Serial.printf("state = %d\n", state);

    if ((button_state->current_press_time - button_state->last_press_time) > DEBOUNCE_DELAY_MS) {
        button_state->last_value = button_state->current_value;
        button_state->current_value = !state;

        // Serial.print("Inside time diff\n");

        if (button_state->last_value != button_state->current_value) {
            return true;
        }
    }

    return false;
}

uint32_t button_read(const uint8_t pin, button_state_t* const button_state) {
    button_state->last_press_time = button_state->current_press_time;
    button_state->current_press_time = millis();

    uint32_t state = digitalRead(pin);
    // Serial.printf("state = %d\n", state);

    if ((button_state->current_press_time - button_state->last_press_time) > DEBOUNCE_DELAY_MS) {
        button_state->last_value = button_state->current_value;
        button_state->current_value = !state;

        // Serial.print("Inside time diff\n");

        // if (button_state->last_value != button_state->current_value) {
        //     return state;
        // }
    }
}

void led_blink(const uint8_t pin, const uint32_t delay_high_ms, const uint32_t delay_low_ms) {
    digitalWrite(pin, HIGH);
    delay(delay_high_ms);
    digitalWrite(pin, LOW);
    delay(delay_low_ms);
}

void led_blink_fast_sync() {
    led_blink(PIN_LED_RED, 200, 0);
    led_blink(PIN_LED_BLUE, 200, 0);
}

void led_blink_slow_alter() {
    led_blink(PIN_LED_RED, 1000, 1000);
    led_blink(PIN_LED_BLUE, 1000, 1000);
}

// Interrupts
// void button_custom_handler() {
//     blink_style = BLINK_STYLE_FAST_SYNC;
// }
//
// void button_builtin_handler() {
//     blink_style = BLINK_STYLE_SLOW_ALTER;
// }

void setup() {
    Serial.begin(115200);

    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_BLUE, OUTPUT);

    pinMode(PIN_BUTTON_BUILTIN, INPUT_PULLUP);
    // attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_BUILTIN), button_builtin_handler, CHANGE);
    pinMode(PIN_BUTTON_CUSTOM, INPUT_PULLUP);
    // attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_CUSTOM), button_custom_handler, CHANGE);

    blink_style = BLINK_STYLE_FAST_SYNC;

    Serial.print("Board started...\n");
}

void loop() {
    // Serial.print("In loop\n");
    bool button_custom_is_updated = button_is_updated(PIN_BUTTON_CUSTOM, &button_custom_state);
    bool button_builtin_is_updated = button_is_updated(PIN_BUTTON_BUILTIN, &button_builtin_state);

    Serial.printf("Custom button: previous(%d) | current(%d) | updated(%d)\n",
                  button_custom_state.last_value, button_custom_state.current_value, button_custom_is_updated);

    Serial.printf("Builtin button: previous(%d) | current(%d) | updated(%d)\n",
                  button_builtin_state.last_value, button_builtin_state.current_value, button_builtin_is_updated);

    if (button_custom_is_updated && button_builtin_is_updated) {
        Serial.print("Warning: both buttons are updated\n");
        digitalWrite(PIN_LED_RED, LOW);
        digitalWrite(PIN_LED_BLUE, LOW);
        return;
    }

    if (/* button_custom_is_updated && */ button_custom_state.current_value == HIGH) {
        Serial.print("Custom button was pressed\n");
        blink_style = BLINK_STYLE_FAST_SYNC;
    }

    if (/* button_builtin_is_updated && */ button_builtin_state.current_value == HIGH) {
        Serial.print("Builtin button was pressed\n");
        blink_style = BLINK_STYLE_SLOW_ALTER;
    }

    switch (blink_style) {
    case BLINK_STYLE_FAST_SYNC:
        led_blink_fast_sync();
        break;

    case BLINK_STYLE_SLOW_ALTER:
        led_blink_slow_alter();
        break;
    }

    delay(100);
}

