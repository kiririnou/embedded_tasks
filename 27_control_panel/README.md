# Control Panel

### Structure
- 

### TODO:

- [x] main: pass htim, timer_freq, timer_channel and hadc to control_panel for buzzer and pot inits
- [x] control_panel: pass htim and timer_freq, channel to buzzer
- [ ] control_panel: pass hadc to pot
- [x] test buzzer
- [ ] pot component + api
- [ ] test pot
- [x] relay: api
- [x] relay: test
- [ ] buzzer: investigate if it worth to pass global now or get time in place
- [ ] button: panic irq
- [x] control_panel: state machine + api
- [x] control_panel: delay transition
- [ ] control_panel: delay via pot
- [x] control_panel: rename control_panel_set_delay_state to arming only?
- [x] arm_button: impl short press
- [x] arm_button: impl long press
- [ ] hardware: connect buzzer via transistor
- [ ] hw: HAL_GetTick() wrapper
- [ ] overall: move all plain numbers to defines and `config.h`
- [ ] clang-format: fix single item array formatting e.g. `led.c`

- HW

- [x] led?
    - [x] init
    - [x] heartbeat

- [x] buttons
    - [x] init
    - [x] fsm debounce
    - [x] register callbacks for events

- [x] potentiometer
    - [x] init
    - [x] read

- [x] buzzer
    - [x] init
    - [x] pwm
    - [x] play sound
    - [x] set freq
        - formula: prescaler = TIM_CLK / (ARR x FREQ) - 1

- [ ] relay
    - [x] init
    - [x] set state
    - [x] tested
    - [ ] hw: connect led
    - [ ] led tested

- SW

- [ ] panel
    - [ ] init hw
    - [ ] main loop

- [ ] utils
    - [ ] log
        - [ ] init
        - [x] log_write
    - [ ] rolling_avg_sampler
        - [x] impl
        - [ ] tested
        - [ ] REVIEW IF NEEDED
    - [ ] map
        - [x] impl
        - [ ] tested
