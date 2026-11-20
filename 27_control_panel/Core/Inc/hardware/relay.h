#ifndef HARDWARE_RELAY_H_
#define HARDWARE_RELAY_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>

#define HW_RELAY_Pin GPIO_PIN_10
#define HW_RELAY_GPIO_Port GPIOA

typedef enum relay_state
{
    HW_RELAY_STATE_OFF = 0u,
    HW_RELAY_STATE_ON,
} hw_relay_state_t;

typedef struct relay
{
    GPIO_TypeDef* port;
    uint16_t pin;
    hw_relay_state_t state;
} hw_relay_t;

void hw_relay_init(hw_relay_t* relay, GPIO_TypeDef* port, uint16_t pin);
void hw_relay_set(hw_relay_t* relay, hw_relay_state_t state);

#endif /* HARDWARE_RELAY_H_ */
