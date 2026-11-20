#include "components/button/button.h"
#include "components/button/button_arm_status.h"
#include "components/button/button_zone.h"
#include "config.h"
#include "hardware/button.h"
#include "utils/log.h"

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

static button_t buttons[BUTTON_MAX] = {0};

static button_event_t button_debounce(button_t* btn, const uint32_t now);

void button_init(button_type_t btn)
{
    if (btn >= BUTTON_MAX)
        return;
    log_write("Start button init");

    buttons[btn].raw = 0u;
    buttons[btn].state = BUTTON_STATE_IDLE;
    buttons[btn].event = BUTTON_EVENT_HANDLED;
    buttons[btn].last_debounce_timestamp = 0u;
    buttons[btn].press_start_timestamp = 0u;

    switch (btn)
    {
    case BUTTON_ALARM:
        // TODO: implment
        break;
    case BUTTON_ARM_STATUS:
        button_arm_status_init(&buttons[BUTTON_ARM_STATUS].hw);
        break;
    case BUTTON_ZONE_1:
        button_zone_init(BUTTON_ZONE_1, &buttons[BUTTON_ZONE_1].hw);
        break;
    case BUTTON_ZONE_2:
        button_zone_init(BUTTON_ZONE_2, &buttons[BUTTON_ZONE_2].hw);
        break;
    case BUTTON_MAX:
        // TODO: implment
        break;
    }
}

// TODO: return status?
void button_register_callback(button_type_t btn, button_event_t event, button_callback_t cb)
{
    if (btn >= BUTTON_MAX)
        return;
    if (event >= BUTTON_EVENT_MAX)
        return;
    if (cb == NULL)
        return;

    buttons[btn].callbacks[event] = cb;
}

// TODO: impl arguments (separate struct for callback?)(
void button_handle_event(button_type_t btn, button_event_t event, void* ctx)
{
    if (btn >= BUTTON_MAX)
        return;

    switch (event)
    {
    case BUTTON_EVENT_SHORT_PRESS:
        if (buttons[btn].callbacks[BUTTON_EVENT_SHORT_PRESS] != NULL)
            buttons[btn].callbacks[BUTTON_EVENT_SHORT_PRESS](ctx);
        buttons[btn].event = BUTTON_EVENT_HANDLED;
        break;
    case BUTTON_EVENT_LONG_PRESS:
        if (buttons[btn].callbacks[BUTTON_EVENT_LONG_PRESS] != NULL)
            buttons[btn].callbacks[BUTTON_EVENT_LONG_PRESS](ctx);
        buttons[btn].event = BUTTON_EVENT_HANDLED;
        break;
    // case BUTTON_EVENT_STUCK:
    //     if (btn->callbacks[BUTTON_EVENT_STUCK] != NULL)
    //         btn->callbacks[BUTTON_EVENT_STUCK](ctx);
    //     btn->event = BUTTON_EVENT_HANDLED;
    //     break;
    case BUTTON_EVENT_HANDLED:
    case BUTTON_EVENT_MAX:
        // TODO: something?
        break;
    }
}

button_event_t button_poll(button_type_t btn, const uint32_t now)
{
    if (btn >= BUTTON_MAX)
        return BUTTON_EVENT_MAX;
    buttons[btn].raw = hw_button_read(&buttons[btn].hw);
    return button_debounce(&buttons[btn], now);
}

void button_handle(button_type_t btn, void* ctx)
{
    if (btn >= BUTTON_MAX)
        return;
    const button_event_t event = button_poll(btn, HAL_GetTick());
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
            // NOTE: error?
            break;
        }
    }
    return btn->event;
}

// TODO: probably remove
const char* button_state_to_str(const button_state_t state)
{
    switch (state)
    {
    case BUTTON_STATE_IDLE:
        return "BUTTON_STATE_IDLE";
    case BUTTON_STATE_PRESSED:
        return "BUTTON_STATE_PRESSED";
    case BUTTON_STATE_HELD:
        return "BUTTON_STATE_HELD";
    case BUTTON_STATE_RELEASED:
        return "BUTTON_STATE_RELEASED";
    default:
        return "No button state";
    }
}

const char* button_event_to_str(const button_event_t event)
{
    switch (event)
    {
    case BUTTON_EVENT_HANDLED:
        return "BUTTON_EVENT_HANDLED";
    case BUTTON_EVENT_SHORT_PRESS:
        return "BUTTON_EVENT_SHORT_PRESS";
    case BUTTON_EVENT_LONG_PRESS:
        return "BUTTON_EVENT_LONG_PRESS";
    // case BUTTON_EVENT_STUCK:
    //     return "BUTTON_EVENT_STUCK";
    default:
        return "No button event";
    }
}
