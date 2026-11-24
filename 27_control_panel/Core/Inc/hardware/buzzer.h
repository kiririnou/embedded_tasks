#ifndef HARDWARE_BUZZER_H_
#define HARDWARE_BUZZER_H_

#include "hardware/hardware.h"
#include "stm32f4xx_hal.h"

#include <stdint.h>

typedef enum buzzer_state
{
    HW_BUZZER_STATE_OFF = 0u,
    HW_BUZZER_STATE_ON,
    HW_BUZZER_STATE_MAX,
} hw_buzzer_state_t;

typedef struct buzzer
{
    TIM_HandleTypeDef* htim;
    uint32_t timer_freq;
    uint32_t channel;
    uint32_t freq;
    uint8_t volume;
    hw_buzzer_state_t state;
} hw_buzzer_t;

hw_result_t hw_buzzer_init(hw_buzzer_t* buzzer, TIM_HandleTypeDef* htim, uint32_t timer_freq,
                           uint32_t channel);
hw_result_t hw_buzzer_set_freq(hw_buzzer_t* buzzer, const uint32_t freq);
hw_result_t hw_buzzer_set_volume(hw_buzzer_t* buzzer, const uint32_t volume);
hw_result_t hw_buzzer_set_state(hw_buzzer_t* buzzer, hw_buzzer_state_t state);

#endif /* HARDWARE_BUZZER_H_ */
