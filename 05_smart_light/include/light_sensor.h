#pragma once

#include "config.h"
#include <cstddef>
#include <cstdint>

struct LightSensor {
    uint16_t raw_value;
    uint16_t sampled_value;

    void setup();
    void update();

private:
    void sample();
    uint16_t samples[Config::LDR_SAMPLES];
    uint32_t samples_sum;
    size_t samples_write_index;
    size_t samples_count;
};
