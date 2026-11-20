#include "hardware/relay.h"

void hw_relay_init(hw_relay_t* relay, GPIO_TypeDef* port, uint16_t pin)
{
    if (relay == NULL)
        return;
    if (port == NULL)
        return;

    relay->port = port;
    relay->pin = pin;
    hw_relay_set(relay, HW_RELAY_STATE_OFF);
}

void hw_relay_set(hw_relay_t* relay, hw_relay_state_t state)
{
    if (relay == NULL)
        return;

    if (relay->state == state)
        return;

    relay->state = state;

    GPIO_PinState pin_state = (relay->state == HW_RELAY_STATE_OFF) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(relay->port, relay->pin, pin_state);
}
