#include "hardware/led.h"

void hw_led_init(hw_led_t* led, GPIO_TypeDef* port, uint16_t pin, uint32_t half_period)
{
    if (led == NULL)
        return;

    if (port == NULL)
        return;

    led->port = port;
    led->pin = pin;
}

void hw_led_toggle(hw_led_t* led)
{
    HAL_GPIO_TogglePin(led->port, led->pin);
}
