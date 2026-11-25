#include "hardware/button.h"
#include "stm32f4xx_hal_gpio.h"

static hw_button_irq_cb s_irq_cb = NULL;

uint8_t hw_button_read(hw_button_t* btn)
{
    return HAL_GPIO_ReadPin(btn->port, btn->pin) == GPIO_PIN_RESET;
}

hw_result_t hw_button_register_irq_callback(hw_button_t* btn, hw_button_irq_cb irq_cb)
{
    if (btn == NULL)
        return HW_RESULT_ERROR;

    if (irq_cb == NULL)
        return HW_RESULT_ERROR;

    btn->irq_cb = irq_cb;
    s_irq_cb = irq_cb;
    return HW_RESULT_OK;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (s_irq_cb != NULL)
        s_irq_cb();
}
