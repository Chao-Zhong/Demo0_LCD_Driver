#ifndef DRAW_STRING_H
#define DRAW_STRING_H

#include <string.h>
#include <stdio.h>
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#define LCD_HOST    SPI2_HOST
#define LCD_PIXEL_CLK_HZ    (5*1000*1000)
#define BKL_ON_LEVEL   1
#define MOSI_PIN    6
#define CLK_PIN     7
#define CS_PIN      14
#define DC_PIN      15
#define RST_PIN     21
#define BKL_PIN     22

#define LCD_H       172
#define LCD_V       320

#define PARALLEL_LINES  16
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define YELLOW  0xFFE0

void lcd_draw_string(esp_lcd_panel_handle_t panel_handle, const char* str, int x, int y, uint16_t color, uint16_t bg_color, uint8_t size);

#endif