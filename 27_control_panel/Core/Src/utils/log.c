#include "utils/log.h"
#include "config.h"
#include "hardware/hardware.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static UART_HandleTypeDef* s_huart = NULL;

void log_init(UART_HandleTypeDef* huart)
{
    s_huart = huart;
}

void log_write(const char* fmt, ...)
{
    assert_param(fmt != NULL);
    assert_param(strlen(fmt) < CONFIG_LOG_BUFFER_SIZE);

    uint32_t now = hw_get_tick();
    char buffer[CONFIG_LOG_BUFFER_SIZE] = {0};

    uint32_t pos = sprintf(buffer, "[%u] ", now);

    va_list args;
    va_start(args, fmt);
    pos += vsnprintf(buffer + pos, CONFIG_LOG_BUFFER_SIZE - pos - 3, fmt, args);
    va_end(args);

    buffer[pos++] = '\r';
    buffer[pos++] = '\n';
    buffer[pos++] = '\0';

    HAL_UART_Transmit(s_huart, (uint8_t*)buffer, pos, 100);
}
