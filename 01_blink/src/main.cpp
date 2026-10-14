#include <Arduino.h>
#include <cstdint>

#define THREE_LEDS 1

#define LED_PIN_RED 40
#define LED_PIN_BLUE 41
#if THREE_LEDS
#define LED_PIN_GREEN 42
#endif

#define TWO_LEDS_ALTERNATING_BLINK 0
#define TWO_LEDS_ALTERNATING_DOUBLE_BLINK 0
#define TWO_LEDS_ALTERNATING_BURSTS 0
#define TWO_LEDS_RANDOM_PATTERNS 1

void leds_alternating_blink(uint32_t delay_ms);
void leds_alternating_blink_wrapped(void);
void leds_alternating_double_blink(void);
void leds_alternating_burts(void);
void leds_random_blink(void);

typedef void (*pattern_fn)(void);

#define PATTERNS_SIZE 3
static pattern_fn patterns[PATTERNS_SIZE] = {
    leds_alternating_blink_wrapped,
    leds_alternating_double_blink,
    leds_alternating_burts,
};

void setup() {
    Serial.begin(115200);

    pinMode(LED_PIN_RED, OUTPUT);
    pinMode(LED_PIN_BLUE, OUTPUT);
#if THREE_LEDS
    pinMode(LED_PIN_GREEN, OUTPUT);
#endif
}

void loop() {
    #if TWO_LEDS_ALTERNATING_BLINK
        leds_alternating_blink(100);
    #endif

    #if TWO_LEDS_ALTERNATING_DOUBLE_BLINK
        leds_alternating_double_blink();
    #endif

    #if TWO_LEDS_ALTERNATING_BURSTS
        leds_alternating_burts();
    #endif

    #if TWO_LEDS_RANDOM_PATTERNS
        leds_random_blink();
    #endif
}

void led_blink(uint32_t pin, uint32_t delay_high_ms, uint32_t delay_low_ms) {
    digitalWrite(pin, HIGH);
    delay(delay_high_ms);
    digitalWrite(pin, LOW);
    delay(delay_low_ms);
}

void leds_alternating_blink(uint32_t delay_ms) {
    led_blink(LED_PIN_RED, delay_ms, 0);
    led_blink(LED_PIN_BLUE, delay_ms, 0);
#if THREE_LEDS
    led_blink(LED_PIN_GREEN, delay_ms, 0);
#endif
}

static uint32_t led_pins[] = {
    LED_PIN_RED,
    LED_PIN_BLUE,
#if THREE_LEDS
    LED_PIN_GREEN
#endif
};

void leds_alternating_double_blink(void) {
#if THREE_LEDS
    uint32_t leds_count = 3;
#else
    uint32_t leds_count = 2;
#endif

    for (size_t idx = 0; idx < leds_count; ++idx) {
        led_blink(led_pins[idx], 100, 100);
        led_blink(led_pins[idx], 100, 250);
    }
}

void leds_alternating_burts(void) {
    led_blink(LED_PIN_RED, 80, 100);
    led_blink(LED_PIN_RED, 80, 450);

    led_blink(LED_PIN_BLUE, 80, 100);
    led_blink(LED_PIN_BLUE, 80, 550);

#if THREE_LEDS
    led_blink(LED_PIN_GREEN, 100, 250);
    led_blink(LED_PIN_GREEN, 100, 350);
#endif
}

void leds_alternating_blink_wrapped(void) {
    leds_alternating_blink(200);
}

void leds_random_blink(void) {
    long pattern_idx = random(0, PATTERNS_SIZE);
    long repeats = random(1, 4);
    for (size_t idx = 0; idx < repeats; ++idx) {
        Serial.printf("Pattern idx: %ld\n", pattern_idx);
        patterns[pattern_idx]();
    }
}
