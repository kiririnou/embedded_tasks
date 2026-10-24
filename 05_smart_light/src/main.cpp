#include <Arduino.h>

#include "HardwareSerial.h"
#include "utils.h"
#include "config.h"
#include "esp32-hal.h"

#include "light_sensor.h"
#include "led.h"
#include "button.h"

static LightSensor light_sensor{};
static Led led{};
static Button button{};

void setup() {
    Serial.begin(Config::BAUD_RATE);

    light_sensor.setup();
    led.setup();
    button.setup([]() {
        led.request_max_brightness();
    });
}

void loop() {
    g_time_now = millis();

    light_sensor.update();
    led.update(light_sensor.sampled_value);
    button.update();
}

