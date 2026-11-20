#include "components/relay/relay.h"
#include "hardware/relay.h"
#include "utils/log.h"

typedef struct
{
    hw_relay_t hw;
} relay_t;

static relay_t relay = {0};

void relay_init(void)
{
    hw_relay_init(&relay.hw, HW_RELAY_GPIO_Port, HW_RELAY_Pin);
}

void relay_open(void)
{
    // log_write("Relay open");
    hw_relay_set(&relay.hw, HW_RELAY_STATE_ON);
}

void relay_close(void)
{
    // log_write("Relay close");
    hw_relay_set(&relay.hw, HW_RELAY_STATE_OFF);
}
