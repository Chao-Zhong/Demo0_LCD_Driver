#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H

#include <stdio.h>
#include <string.h>
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"


/*硬件基本配置*/
#define LCD_HOST    SPI2_HOST
#define LCD_PIXEL_CLK_HZ    (40*1000*1000)
#define BKL_ON_LEVEL   1            //背光灯点亮电平

/*端口定义*/
#define MOSI_PIN    6
#define CLK_PIN     7
#define CS_PIN      14
#define DC_PIN      15
#define RST_PIN     21
#define BKL_PIN     22

/*LCD显示设置*/
#define LCD_H       172             //水平方向像素
#define LCD_V       320             //垂直方向像素

#define LCD_COLOR_BITS      16      //16位色

//颜色定义
#define YELLOW      0x3F00
#define WHITE       0xFFFF
#define BLACK       0x0000

extern spi_bus_config_t bus_cfg;
extern esp_lcd_panel_io_handle_t io_handle;
extern esp_lcd_panel_io_spi_config_t io_cfg;
extern esp_lcd_panel_handle_t panel_handle;
extern esp_lcd_panel_dev_config_t panel_cfg; 

void lcd_init(void);
void lcd_panel_set_backgrund(esp_lcd_panel_handle_t panel, uint16_t color);
void lcd_panel_draw_char(uint16_t x, uint16_t y, unsigned char ch, uint16_t color);
void lcd_panel_draw_string(uint8_t x, uint8_t y, char *p, uint16_t color);

#endif
