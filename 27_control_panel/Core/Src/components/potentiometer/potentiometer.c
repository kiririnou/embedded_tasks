#include "components/potentiometer/potentiometer.h"
#include "common.h"
#include "config.h"
#include "hardware/potentiometer.h"
#include "stm32f4xx_hal_adc.h"
#include "utils/log.h"
#include "utils/map.h"

typedef struct
{
    hw_potentiometer_t hw;
    ADC_HandleTypeDef* hadc;
} potentiometer_t;

static potentiometer_t s_pot = {0};

result_t potentiometer_init(ADC_HandleTypeDef* hadc)
{
    if (hadc == NULL)
        return RESULT_ERROR;

    s_pot.hadc = hadc;
    hw_potentiomenter_init(&s_pot.hw, s_pot.hadc);
    return RESULT_OK;
}

uint16_t potentiometer_convert_to_delay_ms(void)
{
    uint16_t adc_value = 0u;
    if (hw_potentiometer_read(&s_pot.hw, &adc_value) == HW_RESULT_ERROR)
    {
        log_write("pontentiometer: Failed to read potentiometer value. using minimal delay (%u)",
                  CONFIG_POTENTIOMETER_ADC_MIN_VALUE);
        return CONFIG_POTENTIOMETER_ADC_MIN_VALUE;
    }

    const uint16_t converted_value = map(adc_value,
                                         CONFIG_POTENTIOMETER_ADC_MIN_VALUE,
                                         CONFIG_POTENTIOMETER_ADC_MAX_VALUE,
                                         CONFIG_POTENTIOMETER_MIN_SEC,
                                         CONFIG_POTENTIOMETER_MAX_SEC);
    return converted_value * 1000;
}
