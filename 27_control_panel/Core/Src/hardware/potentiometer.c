#include "hardware/potentiometer.h"

void hw_potentiomenter_init(hw_potentiometer_t* pot, ADC_HandleTypeDef* hadc)
{
    if (pot == NULL)
        return;
    if (hadc == NULL)
        return;

    pot->hadc = hadc;
}

uint32_t hw_potentiometer_read(hw_potentiometer_t* pot)
{
    if (pot == NULL)
        return 0u;

    uint16_t adc_val = 0u;
    HAL_ADC_Start(pot->hadc);
    // TODO: handle error
    if (HAL_ADC_PollForConversion(pot->hadc, 1) == HAL_OK)
    {
        adc_val = HAL_ADC_GetValue(pot->hadc);
    }
    HAL_ADC_Stop(pot->hadc);
    return adc_val;
}
