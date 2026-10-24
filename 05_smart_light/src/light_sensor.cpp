#include "light_sensor.h"
#include "config.h"
#include "esp32-hal-adc.h"

#include <cstdint>

void LightSensor::setup() {
    raw_value = 0;
    sampled_value = 0;

    for (auto& sample : samples) {
        sample = 0;
    }
    samples_sum = 0;
    samples_write_index = 0;
    samples_count = 0;

    analogReadResolution(Config::LDR_READ_RESOLUTION);
    // TODO: if pin is configurable,
    //       then adc should be matched or wrong one will be used
    analogSetPinAttenuation(Config::GPIO_LDR, ADC_11db);
}

void LightSensor::update() {
    raw_value = static_cast<uint16_t>(analogRead(Config::GPIO_LDR));
    sample();
}

void LightSensor::sample() {
    if (samples_count == Config::LDR_SAMPLES) {
        samples_sum -= samples[samples_write_index];
    } else {
        ++samples_count;
    }

    samples[samples_write_index] = raw_value;
    samples_sum += raw_value;
    samples_write_index = (samples_write_index + 1) % Config::LDR_SAMPLES;
    sampled_value = static_cast<uint16_t>(samples_sum / samples_count);
}

