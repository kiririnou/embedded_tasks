#pragma once

#include <cstdint>

namespace Config {
constexpr uint32_t BAUD_RATE = 115200;

constexpr uint8_t GPIO_LDR = 4;
constexpr uint16_t LDR_SAMPLES = 8;
constexpr uint8_t LDR_READ_RESOLUTION = 12;

constexpr uint8_t GPIO_LED = 16;
constexpr uint8_t LED_PWM_CHANNEL = 0;
constexpr uint32_t LED_PWM_FREQ = 5000;
constexpr uint32_t LED_PWM_RESOLUTION_BIT = 12;
constexpr uint32_t LED_PWM_UPDATE_INTERVAL_MS = 200;
constexpr uint32_t LED_MAX_BRIGHTNESS_HOLD_MS = 5000;

constexpr uint32_t LED_PWM_MIN_THRESHOLD = 500;
constexpr uint32_t LED_PWM_MAX_THRESHOLD = 3500;
constexpr uint32_t LED_PWM_MIN_BRIGHTNESS = 0;
constexpr uint32_t LED_PWM_MAX_BRIGHTNESS = 4090;

constexpr uint8_t GPIO_BUTTON = 40;
constexpr uint32_t BUTTON_DEBOUNCE_DELAY_MS = 30;
constexpr uint32_t BUTTON_UPDATE_INTERVAL_MS = 500;
} ;
