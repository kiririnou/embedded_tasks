#include <Arduino.h>
#include <cstdint>

#define THREE_LEDS 0

#define LED_PIN_RED 40
#define LED_PIN_BLUE 41
#if THREE_LEDS
#define LED_PIN_GREEN 42
#endif

enum class LedMode : uint8_t {
    AlternatingBlink,
    DoubleBlink,
    TripleBursts,
    RandomPatterns
};

static constexpr LedMode LED_MODE = LedMode::RandomPatterns;

void leds_alternating_blink(const uint32_t delay_ms);
void leds_alternating_blink_wrapped(void);
void leds_double_blink(void);
void leds_triple_bursts(void);
void leds_random_patterns(void);

typedef void (*pattern_fn)(void);

static constexpr pattern_fn patterns[] = {
    leds_alternating_blink_wrapped,
    leds_double_blink,
    leds_triple_bursts,
};
static constexpr size_t patterns_size = sizeof(patterns) / sizeof(patterns[0]);

void setup() {
    Serial.begin(115200);

    pinMode(LED_PIN_RED, OUTPUT);
    pinMode(LED_PIN_BLUE, OUTPUT);
#if THREE_LEDS
    pinMode(LED_PIN_GREEN, OUTPUT);
#endif
}

void loop() {
    switch (LED_MODE) {
    case LedMode::AlternatingBlink:
        leds_alternating_blink(100);
        break;
    case LedMode::DoubleBlink:
        leds_double_blink();
        break;
    case LedMode::TripleBursts:
        leds_triple_bursts();
        break;
    case LedMode::RandomPatterns:
        leds_random_patterns();
        break;
    }
}

void led_blink(const uint8_t pin, const uint32_t delay_high_ms, const uint32_t delay_low_ms) {
    digitalWrite(pin, HIGH);
    delay(delay_high_ms);
    digitalWrite(pin, LOW);
    delay(delay_low_ms);
}

void leds_alternating_blink(const uint32_t delay_ms) {
    led_blink(LED_PIN_RED, delay_ms, 0);
    led_blink(LED_PIN_BLUE, delay_ms, 0);
#if THREE_LEDS
    led_blink(LED_PIN_GREEN, delay_ms, 0);
#endif
}

static constexpr uint32_t led_pins[] = {
    LED_PIN_RED,
    LED_PIN_BLUE,
#if THREE_LEDS
    LED_PIN_GREEN
#endif
};
static constexpr size_t leds_count = sizeof(led_pins) / sizeof(led_pins[0]);

void leds_double_blink(void) {
    for (size_t idx = 0; idx < leds_count; ++idx) {
        led_blink(led_pins[idx], 100, 100);
        led_blink(led_pins[idx], 100, 250);
    }
}

void leds_triple_bursts(void) {
    led_blink(LED_PIN_RED, 80, 100);
    led_blink(LED_PIN_RED, 80, 200);
    led_blink(LED_PIN_RED, 80, 450);

    led_blink(LED_PIN_BLUE, 80, 100);
    led_blink(LED_PIN_BLUE, 80, 330);
    led_blink(LED_PIN_BLUE, 80, 550);

#if THREE_LEDS
    led_blink(LED_PIN_GREEN, 100, 250);
    led_blink(LED_PIN_GREEN, 100, 100);
    led_blink(LED_PIN_GREEN, 100, 350);
#endif
}

void leds_alternating_blink_wrapped(void) {
    leds_alternating_blink(200);
}

void leds_random_patterns(void) {
    const size_t pattern_idx = static_cast<size_t>(random(0L, static_cast<long>(patterns_size)));
    const size_t repeats = static_cast<size_t>(random(1L, 4L));
    for (size_t idx = 0; idx < repeats; ++idx) {
        Serial.printf("Pattern idx: %u\n", pattern_idx);
        patterns[pattern_idx]();
    }
}
