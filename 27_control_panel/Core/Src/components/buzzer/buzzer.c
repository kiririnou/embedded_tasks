#include "components/buzzer/buzzer.h"
#include "config.h"
#include "hardware/buzzer.h"
#include "hardware/hardware.h"
#include "utils/log.h"
#include <stdbool.h>

typedef struct
{
    hw_buzzer_t hw;
    bool is_active;
    uint32_t chirp_duration_ms;
    uint32_t chirp_time_start;
} buzzer_t;

static buzzer_t s_buzzer = {0};

result_t buzzer_init(TIM_HandleTypeDef* htim, uint32_t timer_freq, uint32_t timer_chan)
{
    if (hw_buzzer_init(&s_buzzer.hw, htim, timer_freq, timer_chan) == HW_RESULT_ERROR)
        return RESULT_ERROR;

    if (hw_buzzer_set_volume(&s_buzzer.hw, CONFIG_BUZZER_VOLUME) == HW_RESULT_ERROR)
        return RESULT_ERROR;

    if (hw_buzzer_set_freq(&s_buzzer.hw, CONFIG_BUZZER_FREQ) == HW_RESULT_ERROR)
        return RESULT_ERROR;

    if (hw_buzzer_set_state(&s_buzzer.hw, HW_BUZZER_STATE_OFF) == HW_RESULT_ERROR)
        return RESULT_ERROR;

    return RESULT_OK;
}

void buzzer_poll()
{
    uint32_t now = hw_get_tick();
    if ((s_buzzer.is_active == true) && (s_buzzer.chirp_duration_ms != 0u) &&
        ((uint32_t)(now - s_buzzer.chirp_time_start) >= s_buzzer.chirp_duration_ms))
    {
        if (hw_buzzer_set_state(&s_buzzer.hw, HW_BUZZER_STATE_OFF) == HW_RESULT_ERROR)
            log_write("buzzer: failed to set state (%u)", HW_BUZZER_STATE_OFF);
        s_buzzer.is_active = false;
    }
}

result_t buzzer_set_chirp(const uint32_t duration_ms)
{
    s_buzzer.chirp_duration_ms = duration_ms;
    if (hw_buzzer_set_state(&s_buzzer.hw, HW_BUZZER_STATE_ON) == HW_RESULT_ERROR)
    {
        log_write("buzzer: failed to set state (%u)", HW_BUZZER_STATE_ON);
        return RESULT_ERROR;
    }
    s_buzzer.is_active = true;
    s_buzzer.chirp_time_start = hw_get_tick();
    return RESULT_OK;
}

result_t buzzer_start()
{
    if (hw_buzzer_set_state(&s_buzzer.hw, HW_BUZZER_STATE_ON) == HW_RESULT_ERROR)
    {
        log_write("buzzer: failed to set state (%u)", HW_BUZZER_STATE_ON);
        return RESULT_ERROR;
    }
    s_buzzer.is_active = true;
    s_buzzer.chirp_time_start = hw_get_tick();
    s_buzzer.chirp_duration_ms = 0u;
    return RESULT_OK;
}

result_t buzzer_stop()
{
    if (hw_buzzer_set_state(&s_buzzer.hw, HW_BUZZER_STATE_OFF) == HW_RESULT_ERROR)
    {
        log_write("buzzer: failed to set state (%u)", HW_BUZZER_STATE_OFF);
        return RESULT_ERROR;
    }
    s_buzzer.is_active = false;
    return RESULT_OK;
}
