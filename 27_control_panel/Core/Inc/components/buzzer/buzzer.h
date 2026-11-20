#ifndef COMPONENTS_BUZZER_H_
#define COMPONENTS_BUZZER_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>

void buzzer_init(TIM_HandleTypeDef* htim, uint32_t timer_freq, uint32_t timer_chan);
void buzzer_poll(const uint32_t now);
void buzzer_set_chirp(const uint32_t duration_ms, const uint32_t now);
void buzzer_start();
void buzzer_stop();

#endif /* COMPONENTS_BUZZER_H_ */
