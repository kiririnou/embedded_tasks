#ifndef LED_H
#define LED_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdint.h>

typedef enum
{
    LED_STATE_OFF = GPIO_PIN_RESET, LED_STATE_ON = GPIO_PIN_SET,
} Led_State;

typedef enum
{
    LED_PATTERN_BLINK = 0U,
    LED_PATTERN_ON,
    LED_PATTERN_OFF
} Led_Pattern;

typedef struct
{
    GPIO_TypeDef *gpio;
    uint16_t pin;
    Led_State state;
    uint32_t time_last_update;
} Led;

void Led_Init(Led *led, GPIO_TypeDef *gpio, uint16_t pin);
void Led_SetState(Led *led, Led_State state);
void Led_Toggle(Led *led);

#endif /* LED_H */
