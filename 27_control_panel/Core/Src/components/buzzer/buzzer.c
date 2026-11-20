#include "components/buzzer/buzzer.h"
#include "config.h"
#include "hardware/buzzer.h"
#include "utils/log.h"
#include <stdbool.h>

typedef struct
{
    hw_buzzer_t hw;
    bool is_active;
    uint32_t chirp_duration_ms;
    uint32_t chirp_time_start;
    // uint32_t last_time_update;
} buzzer_t;

static buzzer_t buzzer = {0};

void buzzer_init(TIM_HandleTypeDef* htim, uint32_t timer_freq, uint32_t timer_chan)
{
    hw_buzzer_init(&buzzer.hw, htim, timer_freq, timer_chan);
    hw_buzzer_set_volume(&buzzer.hw, CONFIG_BUZZER_VOLUME);
    hw_buzzer_set_freq(&buzzer.hw, CONFIG_BUZZER_FREQ);
    hw_buzzer_set_state(&buzzer.hw, HW_BUZZER_STATE_OFF);
}

void buzzer_poll(const uint32_t now)
{
    uint32_t temp_now = HAL_GetTick();
    if ((buzzer.is_active == true) &&
        ((uint32_t)(temp_now - buzzer.chirp_time_start) >= buzzer.chirp_duration_ms))
    {
        log_write(
            "buzzer: Buzzer is off | now: %u | chirp_time_start: %u | chirp_duration_ms: %u | diff: %u",
            now,
            buzzer.chirp_time_start,
            buzzer.chirp_duration_ms,
            ((uint32_t)(now - buzzer.chirp_time_start) >= buzzer.chirp_duration_ms));
        hw_buzzer_set_state(&buzzer.hw, HW_BUZZER_STATE_OFF);
        buzzer.is_active = false;
        // buzzer.last_time_update = now;
    }
}

void buzzer_set_chirp(const uint32_t duration_ms, const uint32_t now)
{
    log_write("%s: set chirp for %u ms", __FILE__, duration_ms);
    buzzer.chirp_duration_ms = duration_ms;
    hw_buzzer_set_state(&buzzer.hw, HW_BUZZER_STATE_ON);
    buzzer.is_active = true;
    buzzer.chirp_time_start = now;

    // log_write("%s: is_active: &u | chirp_duration_ms: %u", __FILE__, buzzer.is_active);
}

void buzzer_start()
{
    hw_buzzer_set_state(&buzzer.hw, HW_BUZZER_STATE_ON);
    buzzer.is_active = true;
    buzzer.chirp_time_start = HAL_GetTick();
}

void buzzer_stop()
{
    hw_buzzer_set_state(&buzzer.hw, HW_BUZZER_STATE_OFF);
    buzzer.is_active = false;
}
