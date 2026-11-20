#ifndef CONTROL_PANEL_CONTROL_PANEL_H_
#define CONTROL_PANEL_CONTROL_PANEL_H_

#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    SYS_DISARMED = 0u,
    SYS_ARMING,
    SYS_ARMED,
    SYS_ALARM,
} system_state_t;

// TODO: fix. not portable
void control_panel_init(TIM_HandleTypeDef* htim_buzzer, uint32_t buzzer_timer_freq,
                        uint32_t buzzer_timer_chan, ADC_HandleTypeDef* pot_hadc);
void control_panel_run(const uint32_t now);

// TODO: API for external calls
void control_panel_set_panic(bool is_panic);
void control_panel_set_state(system_state_t state);
system_state_t control_panel_get_state(void);
void control_panel_process_state(void);

#endif /* CONTROL_PANEL_CONTROL_PANEL_H_ */
