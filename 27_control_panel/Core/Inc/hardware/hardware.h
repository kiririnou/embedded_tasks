#ifndef HARDWARE_HARDWARE_H_
#define HARDWARE_HARDWARE_H_

#include <stdint.h>

typedef enum
{
    HW_RESULT_OK = 0u,
    HW_RESULT_ERROR,
} hw_result_t;

uint32_t hw_get_tick(void);

#endif /* HARDWARE_HARDWARE_H_ */
