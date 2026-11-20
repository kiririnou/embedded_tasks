#ifndef CONTROL_PANEL_UTILS_ROLLING_AVG_SAMPLER_H_
#define CONTROL_PANEL_UTILS_ROLLING_AVG_SAMPLER_H_

#include "config.h"

#include <stddef.h>
#include <stdint.h>

typedef struct rolling_avg_sampler
{
    uint16_t samples[CONFIG_ROLLING_AVG_SAMPLER_SAMPLES_SIZE];
    uint32_t sum;
    size_t write_index;
    size_t count;
} rolling_avg_sampler_t;

uint16_t rolling_avg_sampler_sample(rolling_avg_sampler_t* s, const uint16_t value);

#endif /* CONTROL_PANEL_UTILS_ROLLING_AVG_SAMPLER_H_ */
