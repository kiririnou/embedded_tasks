#include "components/button/button_zone.h"
#include "components/button/button.h"
#include "components/buzzer/buzzer.h"
#include "config.h"
#include "control_panel/control_panel.h"
#include "control_panel/zone.h"
#include "utils/log.h"
#include <stdint.h>

static void button_zone_short_press_handle(void* ctx);
static void button_zone_long_press_handle(void* ctx);
static result_t button_zone_1_init(hw_button_t* hw);
static result_t button_zone_2_init(hw_button_t* hw);
static void zone_trip(zone_type_t zone);

result_t button_zone_init(button_type_t btn, hw_button_t* hw)
{
    result_t rc = RESULT_OK;
    switch (btn)
    {
    case BUTTON_ZONE_1:
        rc = button_zone_1_init(hw);
        break;
    case BUTTON_ZONE_2:
        rc = button_zone_2_init(hw);
        break;
    default:
        log_write("ERROR: unsupported button_type_t(%u) registering for zone. Skipping...", btn);
        rc = RESULT_ERROR;
        break;
    }
    return rc;
}

static result_t button_zone_1_init(hw_button_t* hw)
{
    result_t rc = RESULT_OK;
    hw->port = HW_BTN_ZONE_1_GPIO_Port;
    hw->pin = HW_BTN_ZONE_1_Pin;
    rc = button_register_callback(BUTTON_ZONE_1,
                                  BUTTON_EVENT_SHORT_PRESS,
                                  &button_zone_short_press_handle);
    rc = button_register_callback(BUTTON_ZONE_1,
                                  BUTTON_EVENT_LONG_PRESS,
                                  &button_zone_long_press_handle);
    return rc;
}

static result_t button_zone_2_init(hw_button_t* hw)
{
    result_t rc = RESULT_OK;
    hw->port = HW_BTN_ZONE_2_GPIO_Port;
    hw->pin = HW_BTN_ZONE_2_Pin;
    rc = button_register_callback(BUTTON_ZONE_2,
                                  BUTTON_EVENT_SHORT_PRESS,
                                  &button_zone_short_press_handle);
    rc = button_register_callback(BUTTON_ZONE_2,
                                  BUTTON_EVENT_LONG_PRESS,
                                  &button_zone_long_press_handle);
    return rc;
}

static void button_zone_short_press_handle(void* ctx)
{
    const button_type_t btn = (button_type_t)(uintptr_t)ctx;
    switch (btn)
    {
    case BUTTON_ZONE_1:
        zone_trip(ZONE_1);
        break;
    case BUTTON_ZONE_2:
        zone_trip(ZONE_2);
        break;
    default:
        log_write("Wrong button(%u) for zone", btn);
        break;
    }
}

static void button_zone_long_press_handle(void* ctx)
{
    const button_type_t btn = (button_type_t)(uintptr_t)ctx;
    switch (btn)
    {
    case BUTTON_ZONE_1:
        log_write("zone 1: bypassed");
        zone_set_bypassed(ZONE_1, !zone_get_bypassed(ZONE_1));
        buzzer_set_chirp(zone_get_bypassed(ZONE_1) ? CONFIG_ZONE_BYPASS_ENABLED_CHIRP_MS
                                                   : CONFIG_ZONE_BYPASS_DISABLED_CHIRP_MS);
        break;
    case BUTTON_ZONE_2:
        log_write("zone 2: bypassed");
        zone_set_bypassed(ZONE_2, !zone_get_bypassed(ZONE_2));
        buzzer_set_chirp(zone_get_bypassed(ZONE_2) ? CONFIG_ZONE_BYPASS_ENABLED_CHIRP_MS
                                                   : CONFIG_ZONE_BYPASS_DISABLED_CHIRP_MS);
        break;
    default:
        log_write("Wrong button(%u) for zone", btn);
        break;
    }
}

static void zone_trip(zone_type_t zone)
{
    if (control_panel_get_state() == SYS_ARMED && !zone_get_bypassed(zone))
    {
        log_write("zone %u: tripped", zone);
        zone_set_tripped(zone, true);
        control_panel_set_state(SYS_ALARM);
    }
}
