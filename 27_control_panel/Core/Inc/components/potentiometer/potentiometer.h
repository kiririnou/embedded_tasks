#ifndef COMPONENTS_POTENTIOMETER_H_
#define COMPONENTS_POTENTIOMETER_H_

#include "common.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>

result_t potentiometer_init(ADC_HandleTypeDef* hadc);
uint16_t potentiometer_convert_to_delay_ms(void);

#endif /* COMPONENTS_POTENTIOMETER_H_ */
