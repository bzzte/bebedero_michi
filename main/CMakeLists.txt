idf_component_register(SRCS "bebedero_michi.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_adc esp_driver_gpio esp_timer bebedero)