#include "button.h"

#include "stm32f4xx_hal.h"

void Button_Init(volatile Button *button, GPIO_TypeDef *gpio, uint16_t pin)
{
    assert_param(button);
    assert_param(gpio);

    button->gpio = gpio;
    button->pin = pin;
    button->raw = 0U;
    button->state = BUTTON_EVENT_HANDLED;
    button->press_time = 0U;
    button->timer_time = 0U;
    button->time_last_update = 0U;
}
