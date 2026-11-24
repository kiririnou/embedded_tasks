#include "components/relay/relay.h"
#include "hardware/relay.h"

typedef struct
{
    hw_relay_t hw;
} relay_t;

static relay_t s_relay = {0};

result_t relay_init(void)
{
    if (hw_relay_init(&s_relay.hw, HW_RELAY_GPIO_Port, HW_RELAY_Pin) == HW_RESULT_ERROR)
        return RESULT_ERROR;

    return relay_close();
}

result_t relay_open(void)
{
    if (hw_relay_set(&s_relay.hw, HW_RELAY_STATE_ON) == HW_RESULT_ERROR)
        return RESULT_ERROR;

    return RESULT_OK;
}

result_t relay_close(void)
{
    if (hw_relay_set(&s_relay.hw, HW_RELAY_STATE_OFF) == HW_RESULT_ERROR)
        return RESULT_ERROR;

    return RESULT_OK;
}
