#include "utils.h"
#include "config.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"
#include <stdarg.h>
#include <stdio.h>

void Log_Write(UART_HandleTypeDef *huart, const char *fmt, ...)
{
    assert_param(huart != NULL);
    assert_param(fmt != NULL);
    assert_param(strlen(fmt) < CONFIG_LOG_BUFFER_SIZE);

    uint32_t now = HAL_GetTick();
    char buffer[CONFIG_LOG_BUFFER_SIZE] = { 0 };

    uint32_t pos = sprintf(buffer, "[%lu] ", now);

    va_list args;
    va_start(args, fmt);
    pos += vsprintf(buffer + pos, fmt, args);
    va_end(args);

    buffer[pos++] = '\r';
    buffer[pos++] = '\n';
    buffer[pos++] = '\0';

    HAL_UART_Transmit(huart, (uint8_t *)buffer, pos, HAL_MAX_DELAY);
}
