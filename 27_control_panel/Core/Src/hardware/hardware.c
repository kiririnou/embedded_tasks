#include "hardware/hardware.h"
#include "stm32f4xx_hal.h"

uint32_t hw_get_tick(void)
{
    return HAL_GetTick();
}
