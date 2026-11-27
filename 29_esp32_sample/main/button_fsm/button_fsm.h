#ifndef BUTTON_FSM_H
#define BUTTON_FSM_H
#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    BUTTON_EVENT_SHORT,
    BUTTON_EVENT_DOUBLE,
    BUTTON_EVENT_LONG,
    BUTTON_EVENT_STUCK,
    BUTTON_EVENT_MAX
} button_event_t;

typedef enum
{
    BUTTON_STATE_IDLE,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_RELEASED,
    BUTTON_STATE_MAX
} button_state_t;

typedef void (*button_event_cb_t)(void* ctx);

typedef struct
{
    button_event_cb_t callbacks[BUTTON_EVENT_MAX];
    button_state_t state;
    bool stuck_emitted;
    bool second_press;
    int64_t press_start_us;
    int64_t first_release_us;
} button_fsm_t;

// All timestamps in microseconds, monotonic, provided by caller.
void button_fsm_init(button_fsm_t* fsm, int64_t now_us);
void button_fsm_poll(button_fsm_t* fsm, bool raw_pressed, int64_t now_us, void* ctx);
void button_fsm_register_callback(button_fsm_t* fsm, button_event_t ev, button_event_cb_t cb);
#endif
