#include "utils/log.h"
#include "config.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static UART_HandleTypeDef* s_huart = NULL;

void log_init(UART_HandleTypeDef* huart)
{
    s_huart = huart;
}

// TODO: add structure to keep logger state, uart handle, log_level?
void log_write(const char* fmt, ...)
{
    // assert_param(huart != NULL);
    assert_param(fmt != NULL);
    assert_param(strlen(fmt) < 256);

    uint32_t now = HAL_GetTick();
    char buffer[CONFIG_LOG_BUFFER_SIZE] = {0};

    uint32_t pos = sprintf(buffer, "[%u] ", now);

    va_list args;
    va_start(args, fmt);
    pos += vsprintf(buffer + pos, fmt, args);
    va_end(args);

    buffer[pos++] = '\r';
    buffer[pos++] = '\n';
    buffer[pos++] = '\0';

    HAL_UART_Transmit(s_huart, (uint8_t*)buffer, pos, 100);
}
