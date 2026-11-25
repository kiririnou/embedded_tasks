#include "control_panel/control_panel.h"
#include "components/button/button.h"
#include "components/buzzer/buzzer.h"
#include "components/potentiometer/potentiometer.h"
#include "components/relay/relay.h"
#include "config.h"
#include "hardware/hardware.h"
#include "utils/log.h"

typedef struct
{
    system_state_t sys_state;
    bool is_panic;
    bool is_arming;
    uint32_t arming_delay_ms;
    uint32_t enter_state_time_ms;
} control_panel_t;

static volatile bool s_alarm_pending = false;
static control_panel_t s_control_panel = {0};

static void control_panel_delay_arming(system_state_t state, uint32_t delay_ms);

result_t control_panel_init(TIM_HandleTypeDef* buzzer_htim, uint32_t buzzer_timer_freq,
                            uint32_t buzzer_timer_chan, ADC_HandleTypeDef* pot_hadc)
{
    if (button_init(BUTTON_ALARM) == RESULT_ERROR)
    {
        log_write("Error: Failed to initialize button alarm");
        return RESULT_ERROR;
    }

    if (button_init(BUTTON_ARM_STATUS) == RESULT_ERROR)
    {
        log_write("Error: Failed to initialize button arm status");
        return RESULT_ERROR;
    }

    if (button_init(BUTTON_ZONE_1) == RESULT_ERROR)
    {
        log_write("Error: Failed to initialize button zone 1");
        return RESULT_ERROR;
    }

    if (button_init(BUTTON_ZONE_2) == RESULT_ERROR)
    {
        log_write("Error: Failed to initialize button zone 2");
        return RESULT_ERROR;
    }

    if (buzzer_init(buzzer_htim, buzzer_timer_freq, buzzer_timer_chan) == RESULT_ERROR)
    {
        log_write("Error: Failed to initialize buzzer");
        return RESULT_ERROR;
    }

    if (relay_init() == RESULT_ERROR)
    {
        log_write("Error: Failed to initialize relay");
        return RESULT_ERROR;
    }

    if (potentiometer_init(pot_hadc) == RESULT_ERROR)
    {
        log_write("Potentiometer failed to initialized");
        return RESULT_ERROR;
    }

    log_write("control_panel: all clear");

    return RESULT_OK;
}

void control_panel_run()
{
    if (s_alarm_pending)
    {
        control_panel_set_state(SYS_ALARM);
    }

    button_handle(BUTTON_ARM_STATUS, NULL);
    button_handle(BUTTON_ZONE_1, (void*)(uintptr_t)BUTTON_ZONE_1);
    button_handle(BUTTON_ZONE_2, (void*)(uintptr_t)BUTTON_ZONE_2);
    buzzer_poll();

    switch (s_control_panel.sys_state)
    {
    case SYS_DISARMED:
        // Do nothing
        break;
    case SYS_ARMING:
        control_panel_delay_arming(SYS_ARMED, s_control_panel.arming_delay_ms);
        break;
    case SYS_ARMED:
        // Do nothing
        break;
    case SYS_ALARM:
        if (!s_control_panel.is_panic)
        {
            log_write("control_panel: Trigger alert");
            control_panel_set_panic(true);
        }
        break;
    }
}

void control_panel_set_alarm(bool is_alarm)
{
    s_alarm_pending = is_alarm;
}

void control_panel_set_panic(bool is_panic)
{
    s_control_panel.is_panic = is_panic;
    if (is_panic)
    {
        if (buzzer_start() == RESULT_ERROR)
            log_write("control_panel PANIC: Failed to start buzzer");

        if (relay_open() == RESULT_ERROR)
            log_write("control_panel PANIC: Failed to open relay");
    }
    else
    {
        if (buzzer_stop() == RESULT_ERROR)
            log_write("control_panel PANIC: Failed to stop buzzer");

        if (relay_close() == RESULT_ERROR)
            log_write("control_panel PANIC: Failed to close relay");
    }
}

void control_panel_set_state(system_state_t state)
{
    if (s_control_panel.sys_state == state)
        return;

    s_control_panel.sys_state = state;
    s_control_panel.enter_state_time_ms = hw_get_tick();
}

system_state_t control_panel_get_state(void)
{
    return s_control_panel.sys_state;
}

void control_panel_process_state(void)
{
    switch (s_control_panel.sys_state)
    {
    case SYS_DISARMED:
        s_control_panel.arming_delay_ms = potentiometer_convert_to_delay_ms();
        log_write("Transit from SYS_DISARMED to SYS_ARMING with delay (%u ms)",
                  s_control_panel.arming_delay_ms);
        control_panel_set_state(SYS_ARMING);
        (void)buzzer_set_chirp(CONFIG_SYS_DISARMED_CHIRP_MS);
        break;
    case SYS_ARMING:
        if (!s_control_panel.is_arming)
        {
            log_write("Delayed (%u ms) transit from SYS_DISARMED to SYS_ARMING",
                      s_control_panel.arming_delay_ms);
            control_panel_set_state(SYS_ARMED);
        }
        else
        {
            log_write("Transit from SYS_ARMING to SYS_DISARMED");
            control_panel_set_state(SYS_DISARMED);
        }
        (void)buzzer_set_chirp(CONFIG_SYS_ARMING_CHIRP_MS);
        break;
    case SYS_ARMED:
        log_write("Transit from SYS_ARMED to SYS_DISARMED");
        control_panel_set_state(SYS_DISARMED);
        (void)buzzer_set_chirp(CONFIG_SYS_ARMED_CHIRP_MS);
        break;
    case SYS_ALARM:
        log_write("Transit from SYS_ALARM to SYS_DISARMED");
        control_panel_set_state(SYS_DISARMED);
        control_panel_set_panic(false);
        control_panel_set_alarm(false);
        break;
    }
}

static void control_panel_delay_arming(system_state_t state, uint32_t delay_ms)
{
    s_control_panel.is_arming = true;
    if ((uint32_t)(hw_get_tick() - s_control_panel.enter_state_time_ms) >= delay_ms)
    {
        log_write("Delayed (%u) transit to state (%u)", delay_ms, state);
        control_panel_set_state(state);
        s_control_panel.is_arming = false;

        if (buzzer_set_chirp(CONFIG_SYS_ARMED_DELAYED_CHIRP_MS) == RESULT_ERROR)
        {
            log_write("control_panel: failed to notify delay via buzzer");
        }
    }
}
