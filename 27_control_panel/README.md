# Control panel

Control panel for alerting different zones.

Components:
- STM32F411CEU6
- Power supply
- 4 buttons: arm status, zone 1, zone 2, panic
- Buzzer via BJT as example of external controlled component
- Relay

Structure
- components -- logic for controlled components to interact with control panel
- control_panel -- control panel main updated function, zone definition, its' API for components to use
- hardware -- attempt in abstaction of vendor's hardware layer for better portability
- utils -- logger and map functions
- main.c -- startup point

Button control:
- arm status:
    - short press: sets next control panel state: SYS_DISARMED -> SYS_ARMING (delay) -> SYS_ARMED -> SYS_DISARMED / SYS_ALERT -> SYS_DISARMED
    - long press: reset system to SYS_DISARMED
- zone 1/2:
    - short press: "trip" zone (buzzer and relay are activated if system was in SYS_ARMED)
    - long press: bypass zone (cannot be tripped)
- panic:
    - short press: set panic via interrupt

Demo:
![Demo](assets/demo.mp4)
