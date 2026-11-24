#ifndef HARDWARE_LED_H_
#define HARDWARE_LED_H_

#include "stm32f4xx_hal.h"

#define HW_LED_HEARTBEAT_Pin GPIO_PIN_13
#define HW_LED_HEARTBEAT_GPIO_Port GPIOC

typedef struct
{
    GPIO_TypeDef* port;
    uint16_t pin;
} hw_led_t;

void hw_led_init(hw_led_t* led, GPIO_TypeDef* port, uint16_t pin, uint32_t half_period);
void hw_led_toggle(hw_led_t* led);

#endif /* HARDWARE_LED_H_ */
