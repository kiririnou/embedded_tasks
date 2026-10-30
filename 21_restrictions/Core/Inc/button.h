#ifndef BUTTON_H
#define BUTTON_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

typedef enum
{
    BUTTON_EVENT_PENDING = 0U, BUTTON_EVENT_HANDLED,
} ButtonEvent_State;

typedef struct
{
    GPIO_TypeDef *gpio;
    uint8_t pin;
    ButtonEvent_State state;
    uint8_t raw;
    uint8_t is_in_debounce;
    uint32_t press_time;
    uint32_t timer_time;
    uint32_t time_last_update;
} Button;

void Button_Init(volatile Button *button, GPIO_TypeDef *gpio, uint16_t pin);

#endif /* BUTTON_H */
