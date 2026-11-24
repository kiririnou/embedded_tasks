#include "hardware/potentiometer.h"

hw_result_t hw_potentiomenter_init(hw_potentiometer_t* pot, ADC_HandleTypeDef* hadc)
{
    if (pot == NULL)
        return HW_RESULT_ERROR;

    if (hadc == NULL)
        return HW_RESULT_ERROR;

    pot->hadc = hadc;
    return HW_RESULT_OK;
}

hw_result_t hw_potentiometer_read(hw_potentiometer_t* pot, uint16_t* adc_val)
{
    hw_result_t rc = HW_RESULT_OK;
    if (pot == NULL)
    {
        rc = HW_RESULT_ERROR;
        goto exit;
    }

    if (pot->hadc == NULL)
    {
        rc = HW_RESULT_ERROR;
        goto exit;
    }

    if (adc_val == NULL)
    {
        rc = HW_RESULT_ERROR;
        goto exit;
    }

    if (HAL_ADC_Start(pot->hadc) != HAL_OK)
    {
        rc = HW_RESULT_ERROR;
        goto exit;
    }

    if (HAL_ADC_PollForConversion(pot->hadc, 1) == HAL_OK)
    {
        *adc_val = HAL_ADC_GetValue(pot->hadc);
    }
    else
    {
        rc = HW_RESULT_ERROR;
    }

    if (HAL_ADC_Stop(pot->hadc) != HAL_OK)
    {
        rc = HW_RESULT_ERROR;
    }
exit:
    return rc;
}
