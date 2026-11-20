#include "hardware/buzzer.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_tim.h"

void hw_buzzer_init(hw_buzzer_t* buzzer, TIM_HandleTypeDef* htim, uint32_t timer_freq,
                    uint32_t channel)
{
    buzzer->htim = htim;
    buzzer->timer_freq = timer_freq;
    buzzer->channel = channel;
    buzzer->state = HW_BUZZER_STATE_OFF;
    buzzer->volume = 50;
    buzzer->freq = 500;
}

void hw_buzzer_set_freq(hw_buzzer_t* buzzer, const uint32_t freq)
{
    if (buzzer == NULL)
        return;

    if (buzzer->freq == freq)
        return;

    buzzer->freq = freq;
    uint32_t prescaler = (buzzer->timer_freq / (1000 * freq)) - 1;
    __HAL_TIM_SET_PRESCALER(buzzer->htim, prescaler);
}

void hw_buzzer_set_volume(hw_buzzer_t* buzzer, const uint32_t volume)
{
    if (buzzer == NULL)
        return;

    if (buzzer->volume == volume)
        return;

    buzzer->volume = volume;
    __HAL_TIM_SET_COMPARE(buzzer->htim, buzzer->channel, buzzer->volume);
}

void hw_buzzer_set_state(hw_buzzer_t* buzzer, hw_buzzer_state_t state)
{
    if (buzzer == NULL)
        return;

    if (state >= HW_BUZZER_STATE_MAX)
        return;

    if (buzzer->state == state)
        return;

    buzzer->state = state;
    if (buzzer->state == HW_BUZZER_STATE_ON)
    {
        HAL_TIM_PWM_Start(buzzer->htim, buzzer->channel);
    }
    else
    {
        HAL_TIM_PWM_Stop(buzzer->htim, buzzer->channel);
    }
}
