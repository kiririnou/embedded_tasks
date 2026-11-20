#ifndef HARDWARE_POTENTIOMETER_H_
#define HARDWARE_POTENTIOMETER_H_

#include "stm32f4xx_hal.h"

typedef struct potentiometer
{
    ADC_HandleTypeDef* hadc;
} hw_potentiometer_t;

void hw_potentiomenter_init(hw_potentiometer_t* pot, ADC_HandleTypeDef* hadc);
uint32_t hw_potentiometer_read(hw_potentiometer_t* pot);

#endif /* HARDWARE_POTENTIOMETER_H_ */
