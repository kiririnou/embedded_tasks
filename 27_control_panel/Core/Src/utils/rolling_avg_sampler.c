#include "utils/rolling_avg_sampler.h"
#include "config.h"

uint16_t rolling_avg_sampler_sample(rolling_avg_sampler_t* sampler, const uint16_t value)
{
    if (sampler->count == CONFIG_ROLLING_AVG_SAMPLER_SAMPLES_SIZE)
    {
        sampler->sum -= sampler->samples[sampler->write_index];
    }
    else
    {
        ++sampler->count;
    }

    sampler->samples[sampler->write_index] = value;
    sampler->sum += value;
    sampler->write_index = (sampler->write_index + 1) % CONFIG_ROLLING_AVG_SAMPLER_SAMPLES_SIZE;

    uint16_t avg_value = (uint16_t)(sampler->sum / sampler->count);
    return avg_value;
}
