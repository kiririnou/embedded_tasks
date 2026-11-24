#ifndef CONTROL_PANEL_ZONE_H_
#define CONTROL_PANEL_ZONE_H_

#include <stdbool.h>

typedef enum zone_state
{
    ZONE_1 = 0u,
    ZONE_2,
    ZONE_MAX
} zone_type_t;

void zone_set_bypassed(zone_type_t zone, bool bypassed);
void zone_set_tripped(zone_type_t zone, bool tripped);
bool zone_get_bypassed(zone_type_t zone);
bool zone_get_tripped(zone_type_t zone);

#endif /* CONTROL_PANEL_ZONE_H_ */
