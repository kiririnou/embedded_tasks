#include "control_panel/zone.h"

typedef struct zone
{
    bool bypassed;
    bool tripped;
} zone_t;

static zone_t s_zones[ZONE_MAX] = {
    [ZONE_1] = {.bypassed = false, .tripped = false},
    [ZONE_2] = {.bypassed = false, .tripped = false},
};

void zone_set_bypassed(zone_type_t zone, bool bypassed)
{
    if (zone >= ZONE_MAX)
        return;
    s_zones[zone].bypassed = bypassed;
}

void zone_set_tripped(zone_type_t zone, bool tripped)
{
    if (zone >= ZONE_MAX)
        return;
    s_zones[zone].tripped = tripped;
}

bool zone_get_bypassed(zone_type_t zone)
{
    if (zone >= ZONE_MAX)
        return false;
    return s_zones[zone].bypassed;
}

bool zone_get_tripped(zone_type_t zone)
{
    if (zone >= ZONE_MAX)
        return false;
    return s_zones[zone].tripped;
}
