#include "components/led/led.h"
#include "config.h"
#include "hardware/led.h"

typedef struct
{
    hw_led_t hw;
    uint32_t half_period;
    uint32_t last_update_timestamp;
} led_t;

static led_t leds[] = {[LED_HEARTBEAT_T] = {
                           .hw = {.port = HW_LED_HEARTBEAT_GPIO_Port, .pin = HW_LED_HEARTBEAT_Pin},
                           .half_period = CONFIG_LED_HEARTBEAT_HALF_PERIOD_MS,
                           .last_update_timestamp = 0u,
                       }};

void led_heartbeat(const uint32_t now)
{
    led_t* heart = &leds[LED_HEARTBEAT_T];
    if ((uint32_t)(now - heart->last_update_timestamp) >= heart->half_period)
    {
        hw_led_toggle(&heart->hw);
        heart->last_update_timestamp = now;
    }
}
