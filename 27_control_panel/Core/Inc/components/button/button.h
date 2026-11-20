#ifndef COMPONENTS_BUTTON_H_
#define COMPONENTS_BUTTON_H_

#include "hardware/button.h"

typedef enum
{
    BUTTON_ALARM,
    BUTTON_ARM_STATUS,
    BUTTON_ZONE_1,
    BUTTON_ZONE_2,
    BUTTON_MAX,
} button_type_t;

typedef enum button_event
{
    BUTTON_EVENT_HANDLED = 0u,
    BUTTON_EVENT_SHORT_PRESS,
    BUTTON_EVENT_LONG_PRESS,
    // BUTTON_EVENT_STUCK,
    BUTTON_EVENT_MAX,
} button_event_t;

typedef void (*button_callback_t)(void*);

// typedef struct button button_t;

void button_init(button_type_t btn /* , GPIO_TypeDef* port, uint16_t pin */);
void button_register_callback(button_type_t btn, button_event_t event, button_callback_t cb);
void button_handle_event(button_type_t btn, button_event_t event, void* ctx);
button_event_t button_poll(button_type_t btn, const uint32_t now);
void button_handle(button_type_t btn, void* ctx);

const char* button_event_to_str(const button_event_t event);

#endif /* COMPONENTS_BUTTON_H_ */
