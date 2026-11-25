#ifndef HARDWARE_BUTTON_H_
#define HARDWARE_BUTTON_H_

#include "hardware/hardware.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>

#define HW_BTN_ALARM_Pin GPIO_PIN_7
#define HW_BTN_ALARM_GPIO_Port GPIOB
#define HW_BTN_ARM_STATUS_Pin GPIO_PIN_3
#define HW_BTN_ARM_STATUS_GPIO_Port GPIOB
#define HW_BTN_ZONE_1_Pin GPIO_PIN_4
#define HW_BTN_ZONE_1_GPIO_Port GPIOB
#define HW_BTN_ZONE_2_Pin GPIO_PIN_5
#define HW_BTN_ZONE_2_GPIO_Port GPIOB

typedef void (*hw_button_irq_cb)();

typedef struct
{
    GPIO_TypeDef* port;
    uint16_t pin;
    hw_button_irq_cb irq_cb;
} hw_button_t;

uint8_t hw_button_read(hw_button_t* btn);
hw_result_t hw_button_register_irq_callback(hw_button_t* btn, hw_button_irq_cb irq_cb);

#endif /* HARDWARE_BUTTON_H_ */
