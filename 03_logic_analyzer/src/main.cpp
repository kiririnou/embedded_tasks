// DEMO CODE

#include "esp32-hal-gpio.h"
#include "esp_attr.h"
#include <Arduino.h>
#include <cstdint>

#define BUTTON_IN 16

volatile uint32_t button_counter = 0;

uint32_t last_count = 0;

void IRAM_ATTR button_isr() {
    button_counter++;
}

void setup() {
    pinMode(BUTTON_IN, INPUT_PULLUP);
    Serial.begin(115200);

    attachInterrupt(digitalPinToInterrupt(BUTTON_IN), button_isr, FALLING);
    Serial.println("Board started");
}

void loop() {
    if (button_counter != last_count) {
        last_count = button_counter;
        Serial.print("Button Pressed! Count: ");
        Serial.println(last_count);
    }

    delay(10);
}

