
idf_component_register(
                    SRCS "lcd_driver.c"
                    INCLUDE_DIRS "./"
                    REQUIRES esp_driver_spi esp_driver_gpio esp_lcd
                    )