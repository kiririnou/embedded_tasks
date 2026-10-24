#pragma once

#include <cstdint>
#include "HardwareSerial.h"

extern uint32_t g_time_now;

#define INTERVAL_DEBUG(interval, fmt, ...)                  \
    do {                                                    \
        static uint32_t time_prev_print = 0U;               \
        if ((g_time_now - time_prev_print) >= interval) {   \
            Serial.printf("[%u] " fmt, g_time_now __VA_OPT__(,) __VA_ARGS__);   \
            time_prev_print = g_time_now;                   \
        }                                                   \
    } while (0)

