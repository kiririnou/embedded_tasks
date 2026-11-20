#ifndef COMPONENTS_LED_H_
#define COMPONENTS_LED_H_

#include <stdint.h>
typedef enum
{
    LED_HEARTBEAT_T,
} led_type_t;

void led_heartbeat(const uint32_t now);

#endif /* COMPONENTS_LED_H_ */
