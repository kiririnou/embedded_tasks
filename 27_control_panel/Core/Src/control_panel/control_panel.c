#include "control_panel/control_panel.h"
#include "components/button/button.h"
#include "components/buzzer/buzzer.h"
#include "components/relay/relay.h"
#include "control_panel/zone.h"
#include "utils/log.h"

typedef struct
{
    system_state_t sys_state;
    volatile bool is_panic;
    bool is_alert;
    bool is_arming;
    uint32_t arming_delay_ms;
    uint32_t enter_state_time_ms;
} control_panel_t;

static control_panel_t control_panel = {0};

static void control_panel_delay_arming(system_state_t state, uint32_t delay_ms);

void control_panel_init(TIM_HandleTypeDef* buzzer_htim, uint32_t buzzer_timer_freq,
                        uint32_t buzzer_timer_chan, ADC_HandleTypeDef* pot_hadc)
{
    control_panel.arming_delay_ms = 5000;

    button_init(BUTTON_ARM_STATUS);
    button_init(BUTTON_ZONE_1);
    button_init(BUTTON_ZONE_2);
    log_write("Buttons initialized");

    buzzer_init(buzzer_htim, buzzer_timer_freq, buzzer_timer_chan);
    log_write("Buzzer initialized");

    relay_init();
    log_write("Relay initialized");
}

void control_panel_run(const uint32_t now)
{
    // panic set via irq
    if (control_panel.is_panic)
    {
        control_panel_set_state(SYS_ALARM);
    }

    button_handle(BUTTON_ARM_STATUS, NULL);
    button_handle(BUTTON_ZONE_1, (void*)(uintptr_t)BUTTON_ZONE_1);
    button_handle(BUTTON_ZONE_2, (void*)(uintptr_t)BUTTON_ZONE_2);
    buzzer_poll(now);

    static uint32_t last_updated = 0u;
    if ((uint32_t)(now - last_updated) >= 1000u)
    {
        // log_write("zone 1: bypassed: %u | tripped: %u",
        //           zone_get_bypassed(ZONE_1),
        //           zone_get_tripped(ZONE_1));
        // log_write("zone 2: bypassed: %u | tripped: %u",
        //           zone_get_bypassed(ZONE_2),
        //           zone_get_tripped(ZONE_2));
        log_write("current_state: %u", control_panel.sys_state);
        last_updated = now;
    }

    switch (control_panel.sys_state)
    {
    case SYS_DISARMED:
        // Do nothing
        break;
    case SYS_ARMING:
        // Delay before moving to SYS_ARMED
        control_panel_delay_arming(SYS_ARMED, control_panel.arming_delay_ms);
        break;
    case SYS_ARMED:
        // Do nothing
        break;
    case SYS_ALARM:
        // Buzzer + relay
        buzzer_start();
        relay_open();
        break;
    }
}

void control_panel_set_panic(bool is_panic)
{
    if (is_panic)
    {
        log_write("start panic");
        buzzer_start();
        relay_open();
    }
    else
    {
        log_write("stop panic");
        buzzer_stop();
        relay_close();
    }
}

void control_panel_set_state(system_state_t state)
{
    if (control_panel.sys_state == state)
        return;

    control_panel.sys_state = state;
    control_panel.enter_state_time_ms = HAL_GetTick();
}

system_state_t control_panel_get_state(void)
{
    return control_panel.sys_state;
}

void control_panel_process_state(void)
{
    switch(control_panel.sys_state)
    {
    case SYS_DISARMED:
        log_write("Transit from SYS_DISARMED to SYS_ARMING");
        control_panel_set_state(SYS_ARMING);
        buzzer_set_chirp(60, HAL_GetTick());
        break;
    case SYS_ARMING:
        if (!control_panel.is_arming)
        {
            log_write("Delayed (%u ms) transit from SYS_DISARMED to SYS_ARMING", control_panel.arming_delay_ms);
            control_panel_set_state(SYS_ARMED);
        }
        else
        {
            log_write("Transit from SYS_ARMING to SYS_DISARMED");
            control_panel_set_state(SYS_DISARMED);
        }
        buzzer_set_chirp(200, HAL_GetTick());
        break;
    case SYS_ARMED:
        log_write("Transit from SYS_ARMED to SYS_DISARMED");
        control_panel_set_state(SYS_DISARMED);
        buzzer_set_chirp(300, HAL_GetTick());
        break;
    case SYS_ALARM:
        log_write("Transit from SYS_ALARM to SYS_DISARMED");
        control_panel_set_state(SYS_DISARMED);
        control_panel_set_panic(false);
        break;
    }
}

static void control_panel_delay_arming(system_state_t state, uint32_t delay_ms)
{
    control_panel.is_arming = true;
    if ((uint32_t)(HAL_GetTick() - control_panel.enter_state_time_ms) >= delay_ms)
    {
        log_write("Delayed (%u) transit to state (%u)", delay_ms, state);
        control_panel_set_state(state);
        control_panel.is_arming = false;
        buzzer_set_chirp(500, HAL_GetTick());
    }
}
