idf_component_register(SRCS "main.c" "button_fsm/button_fsm.c"
                       PRIV_REQUIRES spi_flash
                       REQUIRES esp_driver_gpio esp_timer
                       INCLUDE_DIRS ".")
