#ifndef HARDWARE_POTENTIOMETER_H_
#define HARDWARE_POTENTIOMETER_H_

#include "hardware/hardware.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>

typedef struct potentiometer
{
    ADC_HandleTypeDef* hadc;
} hw_potentiometer_t;

hw_result_t hw_potentiomenter_init(hw_potentiometer_t* pot, ADC_HandleTypeDef* hadc);
hw_result_t hw_potentiometer_read(hw_potentiometer_t* pot, uint16_t* adc_value);

#endif /* HARDWARE_POTENTIOMETER_H_ */
