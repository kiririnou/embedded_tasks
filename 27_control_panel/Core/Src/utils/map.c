#include "utils/map.h"

uint32_t map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max)
{
    return (x - in_min) * (out_max - out_min + 1) / (in_max - in_min + 1) + out_min;
}
