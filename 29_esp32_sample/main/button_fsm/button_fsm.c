#include "button_fsm/button_fsm.h"
#include <stddef.h>

#define DEBOUNCE_US 30000LL
#define DOUBLE_CLICK_US 350000LL
#define LONG_PRESS_US 2000000LL
#define STUCK_US 5000000LL

static void publish(button_fsm_t* fsm, button_event_t ev, void* ctx)
{
    if (fsm == NULL)
        return;
    if (ev >= BUTTON_EVENT_MAX)
        return;
    fsm->callbacks[ev](ctx);
}

void button_fsm_init(button_fsm_t* fsm, int64_t now_us)
{
    *fsm = (button_fsm_t){0};
    fsm->state = BUTTON_STATE_IDLE;
}

void button_fsm_poll(button_fsm_t* s, bool raw, int64_t now, void* ctx)
{
    switch (s->state)
    {
    case BUTTON_STATE_IDLE:
        if (raw)
        {
            s->state = BUTTON_STATE_PRESSED;
            s->press_start_us = now;
            s->stuck_emitted = false;
        }

        if (s->second_press && (now - s->first_release_us > DOUBLE_CLICK_US))
        {
            s->second_press = false;
            publish(s, BUTTON_EVENT_SHORT, ctx);
        }
        break;
    case BUTTON_STATE_PRESSED:
        if (raw)
        {
            if (!s->stuck_emitted && now - s->press_start_us >= STUCK_US)
            {
                publish(s, BUTTON_EVENT_STUCK, ctx);
                s->stuck_emitted = true;
            }
        }
        else
        {
            s->state = BUTTON_STATE_RELEASED;
        }
        break;
    case BUTTON_STATE_RELEASED:
        s->state = BUTTON_STATE_IDLE;

        int64_t time_diff = now - s->press_start_us;
        if (!s->stuck_emitted && time_diff < LONG_PRESS_US && time_diff > DEBOUNCE_US)
        {
            if (s->second_press)
            {
                s->second_press = false;
                publish(s, BUTTON_EVENT_DOUBLE, ctx);
            }
            else
            {
                s->second_press = true;
                s->first_release_us = now;
            }
        }
        else if (!s->stuck_emitted && time_diff >= LONG_PRESS_US)
        {
            publish(s, BUTTON_EVENT_LONG, ctx);
        }
        break;
    case BUTTON_STATE_MAX:
        break;
    }
}

void button_fsm_register_callback(button_fsm_t* fsm, button_event_t ev, button_event_cb_t cb)
{
    if (cb == NULL)
        return;
    if (ev >= BUTTON_EVENT_MAX)
        return;

    fsm->callbacks[ev] = cb;
}
