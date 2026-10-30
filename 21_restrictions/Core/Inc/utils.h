#ifndef UTILS_H
#define UTILS_H

#include "stm32f4xx_hal.h"

#define INTERVAL_ACTION(interval, current_time, last_time) \
    if ((uint32_t)(current_time - last_time) >= interval)

void Log_Write(UART_HandleTypeDef *huart, const char *fmt, ...);

#endif /* UTILS_H */
