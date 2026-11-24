#include "components/button/button.h"
#include "components/button/button_alarm.h"
#include "components/button/button_arm_status.h"
#include "components/button/button_zone.h"
#include "config.h"
#include "hardware/button.h"
#include "hardware/hardware.h"

#define BUTTON_PRESSED 1u
#define BUTTON_RELEASED 0u

typedef enum button_state
{
    BUTTON_STATE_IDLE = 0u,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_HELD,
    BUTTON_STATE_RELEASED,
    BUTTON_STATE_MAX,
} button_state_t;

typedef struct button
{
    hw_button_t hw;
    uint8_t raw;
    button_state_t state;
    button_event_t event;
    uint32_t last_debounce_timestamp;
    uint32_t press_start_timestamp;
    button_callback_t callbacks[BUTTON_EVENT_MAX];
} button_t;

static button_t s_buttons[BUTTON_MAX] = {0};

static button_event_t button_debounce(button_t* btn, const uint32_t now);

result_t button_init(button_type_t btn)
{
    if (btn >= BUTTON_MAX)
        return RESULT_ERROR;

    result_t rc = RESULT_OK;

    s_buttons[btn].raw = 0u;
    s_buttons[btn].state = BUTTON_STATE_IDLE;
    s_buttons[btn].event = BUTTON_EVENT_HANDLED;
    s_buttons[btn].last_debounce_timestamp = 0u;
    s_buttons[btn].press_start_timestamp = 0u;

    switch (btn)
    {
    case BUTTON_ALARM:
        rc = button_alarm_init(&s_buttons[BUTTON_ALARM].hw);
        break;
    case BUTTON_ARM_STATUS:
        rc = button_arm_status_init(&s_buttons[BUTTON_ARM_STATUS].hw);
        break;
    case BUTTON_ZONE_1:
        rc = button_zone_init(BUTTON_ZONE_1, &s_buttons[BUTTON_ZONE_1].hw);
        break;
    case BUTTON_ZONE_2:
        rc = button_zone_init(BUTTON_ZONE_2, &s_buttons[BUTTON_ZONE_2].hw);
        break;
    case BUTTON_MAX:
        rc = RESULT_ERROR;
        break;
    }

    return rc;
}

result_t button_register_callback(button_type_t btn, button_event_t event, button_callback_t cb)
{
    if (btn >= BUTTON_MAX)
        return RESULT_ERROR;

    if (event >= BUTTON_EVENT_MAX)
        return RESULT_ERROR;

    if (cb == NULL)
        return RESULT_ERROR;

    s_buttons[btn].callbacks[event] = cb;
    return RESULT_OK;
}

void button_handle_event(button_type_t btn, button_event_t event, void* ctx)
{
    if (btn >= BUTTON_MAX)
        return;

    switch (event)
    {
    case BUTTON_EVENT_SHORT_PRESS:
        if (s_buttons[btn].callbacks[BUTTON_EVENT_SHORT_PRESS] != NULL)
            s_buttons[btn].callbacks[BUTTON_EVENT_SHORT_PRESS](ctx);
        s_buttons[btn].event = BUTTON_EVENT_HANDLED;
        break;
    case BUTTON_EVENT_LONG_PRESS:
        if (s_buttons[btn].callbacks[BUTTON_EVENT_LONG_PRESS] != NULL)
            s_buttons[btn].callbacks[BUTTON_EVENT_LONG_PRESS](ctx);
        s_buttons[btn].event = BUTTON_EVENT_HANDLED;
        break;
    case BUTTON_EVENT_HANDLED:
    case BUTTON_EVENT_MAX:
        // Do nothing
        break;
    }
}

button_event_t button_poll(button_type_t btn, const uint32_t now)
{
    if (btn >= BUTTON_MAX)
        return BUTTON_EVENT_MAX;

    s_buttons[btn].raw = hw_button_read(&s_buttons[btn].hw);
    return button_debounce(&s_buttons[btn], now);
}

void button_handle(button_type_t btn, void* ctx)
{
    if (btn >= BUTTON_MAX)
        return;

    const button_event_t event = button_poll(btn, hw_get_tick());
    button_handle_event(btn, event, ctx);
}

static button_event_t button_debounce(button_t* btn, const uint32_t now)
{
    if (btn == NULL)
        return BUTTON_EVENT_MAX;

    if (btn->event != BUTTON_EVENT_HANDLED)
        return btn->event;

    const uint32_t time_diff = (uint32_t)(now - btn->last_debounce_timestamp);
    if (time_diff >= CONFIG_BUTTON_DEBOUNCE_MS)
    {
        btn->last_debounce_timestamp = now;
        const uint8_t is_pressed = (btn->raw == BUTTON_PRESSED);
        const uint32_t press_duration = (uint32_t)(now - btn->press_start_timestamp);

        switch (btn->state)
        {
        case BUTTON_STATE_IDLE:
        {
            if (is_pressed)
            {
                btn->state = BUTTON_STATE_PRESSED;
                btn->press_start_timestamp = now;
            }
        }
        break;
        case BUTTON_STATE_PRESSED:
        {
            if (is_pressed == BUTTON_PRESSED)
            {
                btn->state = BUTTON_STATE_PRESSED;
                if (press_duration >= CONFIG_BUTTON_SHORT_PRESS_MAX_MS)
                {
                    btn->state = BUTTON_STATE_HELD;
                }
            }
            else
            {
                btn->state = BUTTON_STATE_RELEASED;
                btn->event = BUTTON_EVENT_SHORT_PRESS;
            }
        }
        break;
        case BUTTON_STATE_HELD:
        {
            if (is_pressed == BUTTON_RELEASED)
            {
                btn->state = BUTTON_STATE_RELEASED;
                btn->event = BUTTON_EVENT_LONG_PRESS;
            }
        }
        break;
        case BUTTON_STATE_RELEASED:
        {
            if (btn->event == BUTTON_EVENT_HANDLED)
            {
                btn->state = BUTTON_STATE_IDLE;
            }
        }
        break;
        default:
            break;
        }
    }
    return btn->event;
}
