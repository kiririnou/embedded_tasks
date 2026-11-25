#include "components/button/button_arm_status.h"
#include "components/button/button.h"
#include "control_panel/control_panel.h"
#include "utils/log.h"

static void button_arm_status_short_press_handle(void* ctx);
static void button_arm_status_long_press_handle(void* ctx);

result_t button_arm_status_init(hw_button_t* hw)
{
    result_t rc = RESULT_OK;
    hw->port = HW_BTN_ARM_STATUS_GPIO_Port;
    hw->pin = HW_BTN_ARM_STATUS_Pin;
    rc = button_register_callback(BUTTON_ARM_STATUS,
                                  BUTTON_EVENT_SHORT_PRESS,
                                  &button_arm_status_short_press_handle);
    rc = button_register_callback(BUTTON_ARM_STATUS,
                                  BUTTON_EVENT_LONG_PRESS,
                                  &button_arm_status_long_press_handle);
    return rc;
}

static void button_arm_status_short_press_handle(void* ctx)
{
    (void)ctx;
    log_write("arm_status: Process");
    control_panel_process_state();
}

static void button_arm_status_long_press_handle(void* ctx)
{
    (void)ctx;
    log_write("arm_status: Disarm");
    control_panel_set_panic(false);
    control_panel_set_alarm(false);
    control_panel_set_state(SYS_DISARMED);
}
