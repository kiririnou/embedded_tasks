#include "HardwareSerial.h"
#include "esp32-hal-adc.h"
#include "esp32-hal.h"
#include <Arduino.h>
#include <cstdint>
#include <cstdlib>

static uint32_t time_now = 0U;

#define INTERVAL_DEBUG(interval, fmt, ...)                                  \
    {                                                                       \
        static uint32_t time_prev_print = 0U;                               \
        if ((time_now - time_prev_print) >= interval) {                     \
            Serial.printf(fmt __VA_OPT__(,) __VA_ARGS__);                   \
            time_prev_print = time_now;                                     \
        }                                                                   \
    }

constexpr uint8_t PIN_LDR = 4;
constexpr uint32_t BAUD_RATE = 115200;
constexpr double ADC_MAX = 4095.0;
constexpr double U_REF = 3100.0;

void setup() {
    Serial.begin(BAUD_RATE);

    analogReadResolution(12);
    analogSetPinAttenuation(PIN_LDR, ADC_11db);

    Serial.println("Board started");
}

void loop() {
    time_now = millis();

    const uint16_t analog_raw = static_cast<uint16_t>(analogRead(PIN_LDR));

    const double calculated_milivolts = (analog_raw / ADC_MAX) * U_REF;
    const uint32_t analog_milivolts = analogReadMilliVolts(PIN_LDR);

    const double observational_error = ((abs(calculated_milivolts - analog_milivolts)) / analog_milivolts) * 100.0;

    INTERVAL_DEBUG(1000, "analog_raw: %u | calculated_milivolts: %.2f | analog_milivolts: %u | obs_error: %.2f%%\n",
                   analog_raw, calculated_milivolts, analog_milivolts, observational_error);

    delay(100);
}

