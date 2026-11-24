#include "components/button/button_alarm.h"
#include "config.h"
#include "control_panel/control_panel.h"
#include "hardware/button.h"
#include "utils/log.h"

static void button_alarm_irq_cb();

static uint32_t s_last_debounce_time_ms = 0u;

result_t button_alarm_init(hw_button_t* hw)
{
    hw->port = HW_BTN_ALARM_GPIO_Port;
    hw->pin = HW_BTN_ALARM_Pin;
    if (hw_button_register_irq_callback(hw, &button_alarm_irq_cb) == HW_RESULT_ERROR)
    {
        log_write("button_alarm: failed to register irq callback");
        return RESULT_ERROR;
    }
    return RESULT_OK;
}

static void button_alarm_irq_cb()
{
    const uint32_t now = hw_get_tick();
    if ((uint32_t)(now - s_last_debounce_time_ms) >= CONFIG_BUTTON_ALERT_DEBOUNCE_MS)
    {
        control_panel_set_alarm(true);
        s_last_debounce_time_ms = now;
    }
}
