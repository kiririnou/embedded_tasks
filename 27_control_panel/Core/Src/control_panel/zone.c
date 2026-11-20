#include "control_panel/zone.h"

typedef struct zone
{
    // uint8_t id;
    bool bypassed;
    bool tripped;
} zone_t;

static zone_t zones[ZONE_MAX] = {
    [ZONE_1] = {.bypassed = false, .tripped = false},
    [ZONE_2] = {.bypassed = false, .tripped = false},
};

// void zone_init(zone_t* zone);

void zone_set_bypassed(zone_type_t zone, bool bypassed)
{
    if (zone >= ZONE_MAX)
        return;
    zones[zone].bypassed = bypassed;
}

void zone_set_tripped(zone_type_t zone, bool tripped)
{
    if (zone >= ZONE_MAX)
        return;
    zones[zone].tripped = tripped;
}

bool zone_get_bypassed(zone_type_t zone)
{
    if (zone >= ZONE_MAX)
        return false;
    return zones[zone].bypassed;
}

bool zone_get_tripped(zone_type_t zone)
{
    if (zone >= ZONE_MAX)
        return false;
    return zones[zone].tripped;
}
