#include "hardware/buzzer.h"
#include "hardware/hardware.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_tim.h"

hw_result_t hw_buzzer_init(hw_buzzer_t* buzzer, TIM_HandleTypeDef* htim, uint32_t timer_freq,
                           uint32_t channel)
{
    if (buzzer == NULL)
        return HW_RESULT_ERROR;

    if (htim == NULL)
        return HW_RESULT_ERROR;

    buzzer->htim = htim;
    buzzer->timer_freq = timer_freq;
    buzzer->channel = channel;
    buzzer->state = HW_BUZZER_STATE_OFF;
    buzzer->volume = 50;
    buzzer->freq = 500;
    return HW_RESULT_OK;
}

hw_result_t hw_buzzer_set_freq(hw_buzzer_t* buzzer, const uint32_t freq)
{
    if (buzzer == NULL)
        return HW_RESULT_ERROR;

    if (buzzer->freq == freq)
        return HW_RESULT_OK;

    buzzer->freq = (freq == 0) ? 1 : freq;
    uint32_t prescaler = (buzzer->timer_freq / (1000 * buzzer->freq)) - 1;
    __HAL_TIM_SET_PRESCALER(buzzer->htim, prescaler);
    return HW_RESULT_OK;
}

hw_result_t hw_buzzer_set_volume(hw_buzzer_t* buzzer, const uint32_t volume)
{
    if (buzzer == NULL)
        return HW_RESULT_ERROR;

    if (buzzer->volume == volume)
        return HW_RESULT_OK;

    buzzer->volume = volume;
    __HAL_TIM_SET_COMPARE(buzzer->htim, buzzer->channel, buzzer->volume);
    return HW_RESULT_OK;
}

hw_result_t hw_buzzer_set_state(hw_buzzer_t* buzzer, hw_buzzer_state_t state)
{
    if (buzzer == NULL)
        return HW_RESULT_ERROR;

    if (state >= HW_BUZZER_STATE_MAX)
        return HW_RESULT_ERROR;

    if (buzzer->state == state)
        return HW_RESULT_OK;

    if (state == HW_BUZZER_STATE_ON)
    {
        if (HAL_TIM_PWM_Start(buzzer->htim, buzzer->channel) != HAL_OK)
            return HW_RESULT_ERROR;
    }
    else
    {
        if (HAL_TIM_PWM_Stop(buzzer->htim, buzzer->channel) != HAL_OK)
            return HW_RESULT_ERROR;
    }
    buzzer->state = state;
    return HW_RESULT_OK;
}
