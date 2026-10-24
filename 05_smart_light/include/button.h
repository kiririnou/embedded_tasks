#pragma once

#include <cstdint>
#include <functional>

using ButtonCallback = std::function<void(void)>;

struct Button {
    uint8_t raw_value;
    uint8_t stable_value;
    bool is_pressed;

    void setup(ButtonCallback btn_callback);
    void update();
    void read();

private:
    uint32_t press_time;
    uint8_t last_stable_value;
    uint32_t raw_time;
    ButtonCallback callback;
};
