#ifndef CONTROL_PANEL_UTILS_LOG_H_
#define CONTROL_PANEL_UTILS_LOG_H_

#include "stm32f4xx_hal.h"

void log_init(UART_HandleTypeDef* huart);
void log_write(const char* fmt, ...);

#endif /* CONTROL_PANEL_UTILS_LOG_H_ */
