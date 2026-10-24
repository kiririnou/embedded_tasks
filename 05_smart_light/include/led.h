#pragma once

#include <cstdint>

struct Led {
    void setup();
    void update(const uint32_t brightness);
    void request_max_brightness();
    void set_brightness(const uint32_t value);

private:
    uint32_t brightness;
    uint32_t update_interval;
    uint32_t last_update_time;
    bool is_max_brightness_requested;

    uint16_t convert_input_brightness(const uint16_t brightness) const;
};
