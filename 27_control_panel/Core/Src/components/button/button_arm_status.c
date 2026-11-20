#include "components/button/button_arm_status.h"
#include "components/button/button.h"
#include "control_panel/control_panel.h"
#include "utils/log.h"

static void button_arm_status_short_press_handle(void* ctx);
static void button_arm_status_long_press_handle(void* ctx);

void button_arm_status_init(hw_button_t* hw)
{
    hw->port = HW_BTN_ARM_STATUS_GPIO_Port;
    hw->pin = HW_BTN_ARM_STATUS_Pin;
    button_register_callback(BUTTON_ARM_STATUS,
                             BUTTON_EVENT_SHORT_PRESS,
                             &button_arm_status_short_press_handle);
    button_register_callback(BUTTON_ARM_STATUS,
                             BUTTON_EVENT_LONG_PRESS,
                             &button_arm_status_long_press_handle);
}

static void button_arm_status_short_press_handle(void* ctx)
{
    // updated control panels state
    // Disarmed -> Arming -> Armed -- with delay?
    // or cancel alarm + disarm
    (void)ctx;
    log_write("arm_status: short press");
    control_panel_process_state();
}

static void button_arm_status_long_press_handle(void* ctx)
{
    // in any control panel state
    // emergency reset:
    // - cancel panic
    // - go to Disarmed
    // - disable buzzer and relay
    (void)ctx;
    log_write("arm_status: long press");
    control_panel_set_panic(false);
    // control_panel_set_state(SYS_DISARMED);
    control_panel_process_state();
}
