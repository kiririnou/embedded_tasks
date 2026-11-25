#ifndef COMPONENTS_BUZZER_H_
#define COMPONENTS_BUZZER_H_

#include "common.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>

result_t buzzer_init(TIM_HandleTypeDef* htim, uint32_t timer_freq, uint32_t timer_chan);
void buzzer_poll();
result_t buzzer_set_chirp(const uint32_t duration_ms);
result_t buzzer_start();
result_t buzzer_stop();

#endif /* COMPONENTS_BUZZER_H_ */
