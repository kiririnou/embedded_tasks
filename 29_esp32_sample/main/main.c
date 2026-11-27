/*
 * SPDX-FileCopyrightText: 2010-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "hal/gpio_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/projdefs.h"
#include "esp_timer.h"
#include "driver/gpio.h"

#include "button_fsm/button_fsm.h"

#define PIN_LED GPIO_NUM_15
#define PIN_BTN GPIO_NUM_16
#define POLL_MS 10
#define LED_UPDATE_INTERVAL_US 1000000ll

static button_fsm_t btn_fsm = {0};
static uint8_t led_state = 0u;

void on_button_press(void* ctx)
{
    (void)ctx;
    led_state = led_state == 0u ? 1u : 0u;
    gpio_set_level(PIN_LED, led_state);
}

void app_main(void)
{
    gpio_config_t led = {
        .pin_bit_mask = 1ULL << PIN_LED,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&led));
    ESP_ERROR_CHECK(gpio_set_level(PIN_LED, 0u));

    gpio_config_t btn = {
        .pin_bit_mask = 1LL << PIN_BTN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&btn));

    button_fsm_init(&btn_fsm, esp_timer_get_time());
    button_fsm_register_callback(&btn_fsm, BUTTON_EVENT_SHORT, &on_button_press);

    printf("Hello world!\n");

    const TickType_t interval = pdMS_TO_TICKS(POLL_MS);
    TickType_t last_update = xTaskGetTickCount();
    while (true)
    {
        bool raw = gpio_get_level(PIN_BTN) == 0;
        button_fsm_poll(&btn_fsm, raw, esp_timer_get_time(), NULL);
        xTaskDelayUntil(&last_update, interval);
    }
}
