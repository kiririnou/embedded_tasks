#include "led.h"
#include "stm32f4xx_hal.h"

void Led_Init(Led *led, GPIO_TypeDef *gpio, uint16_t pin)
{
    assert_param(led != NULL);
    assert_param(gpio != NULL);

    led->gpio = gpio;
    led->pin = pin;
    led->time_last_update = 0U;
    HAL_GPIO_WritePin(led->gpio, led->pin, (GPIO_PinState)LED_STATE_OFF);
}

void Led_SetState(Led *led, Led_State state)
{
    assert_param(led != NULL);

    if (led->state != state)
    {
        led->state = state;
        HAL_GPIO_WritePin(led->gpio, led->pin, (GPIO_PinState)state);
    }
}

void Led_Toggle(Led *led)
{
    assert_param(led != NULL);
    Led_SetState(led, !led->state);
}
