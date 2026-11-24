#include "hardware/relay.h"

hw_result_t hw_relay_init(hw_relay_t* relay, GPIO_TypeDef* port, uint16_t pin)
{
    if (relay == NULL)
        return HW_RESULT_ERROR;

    if (port == NULL)
        return HW_RESULT_ERROR;

    relay->port = port;
    relay->pin = pin;
    HAL_GPIO_WritePin(relay->port, relay->pin, GPIO_PIN_SET);
    return HW_RESULT_OK;
}

hw_result_t hw_relay_set(hw_relay_t* relay, hw_relay_state_t state)
{
    if (relay == NULL)
        return HW_RESULT_ERROR;

    if (relay->state == state)
        return HW_RESULT_OK;

    relay->state = state;
    GPIO_PinState pin_state = (relay->state == HW_RELAY_STATE_OFF) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(relay->port, relay->pin, pin_state);
    return HW_RESULT_OK;
}
