#include "hardware/button.h"
#include "stm32f4xx_hal_gpio.h"

uint8_t hw_button_read(hw_button_t* btn)
{
    return HAL_GPIO_ReadPin(btn->port, btn->pin) == GPIO_PIN_RESET;
}
