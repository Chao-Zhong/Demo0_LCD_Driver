#include "lcd_driver.h"
#include "lcdfont.h"
/*配置SPI总线*/
spi_bus_config_t bus_cfg = {
    .sclk_io_num = CLK_PIN,
    .mosi_io_num = MOSI_PIN,
    .miso_io_num = -1,
    .quadhd_io_num = -1,
    .quadwp_io_num = -1,
    .max_transfer_sz = LCD_H * LCD_V * LCD_COLOR_BITS / 8 + 10      //单次发送数据的最大数据量（单位：字节）
};

/*配置LCD面板IO*/
esp_lcd_panel_io_handle_t io_handle;
esp_lcd_panel_io_spi_config_t io_cfg = {
    .dc_gpio_num = DC_PIN,
    .cs_gpio_num = CS_PIN,
    .pclk_hz = LCD_PIXEL_CLK_HZ,
    .lcd_cmd_bits = 8,
    .lcd_param_bits = 8,
    .spi_mode = 0,
    .trans_queue_depth = 10
};

/*配置LCD面板*/
esp_lcd_panel_handle_t panel_handle = NULL;        //LCD面板操作句柄
esp_lcd_panel_dev_config_t panel_cfg = {
    .reset_gpio_num = RST_PIN,
    .rgb_ele_order = 0,
    .bits_per_pixel = LCD_COLOR_BITS
};


void lcd_init (){
    //初始化背光gpio
    gpio_config_t bkl_gpio_cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << BKL_PIN
    };
    ESP_ERROR_CHECK(gpio_config(&bkl_gpio_cfg));

    //初始化spi总线
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &bus_cfg, SPI_DMA_CH_AUTO));       //初始化SPI总线
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_cfg, &io_handle));     //初始化LCD面板IO
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_cfg, &panel_handle));        //创建ST7789面板

    //初始化面板
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));     //重置lcd面板
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));      //初始化lcd面板

    //配置显示方向
    ESP_ERROR_CHECK(esp_lcd_panel_set_gap(panel_handle, 34, 0));         //设置偏移

    //开启显示
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
    esp_lcd_panel_invert_color(panel_handle, true);

    //打开背光
    gpio_set_level(BKL_PIN, BKL_ON_LEVEL);
}

/**
 * @brief   设置屏幕背景颜色
 * @param   panel   屏幕面板句柄
 * @param   color   颜色
 * @return  void
*/
void lcd_panel_set_backgurnd(esp_lcd_panel_handle_t panel, uint16_t color){
    if (panel == NULL) return;
    //创建颜色缓冲区
    static uint16_t color_buf[LCD_H * LCD_V];
    
    for (int i = 0; i < LCD_H * LCD_V; i++){
        color_buf[i] = color;
    }
        esp_lcd_panel_draw_bitmap(panel, 0, 0, LCD_H, LCD_V, color_buf);
}

/**
 * @brief   获取字符的点阵数据
 * @param   c           字符
 * @param   front       字体
 * @return  字符点阵数据指针
*/
const uint8_t * get_char_bitmap(char c){
    //检查字符有效性
    if (c < 32 || c > 126){
        return NULL;
    }

    //计算字符在字模数组中的索引
    uint8_t char_index = c - 32;

    //返回索引到的字符数据
    return (const uint8_t *)asc2_2412[char_index];
}

/**
 * @brief   lcd显示单个字符
 * @param   x   x坐标
 * @param   y   y坐标
 * @param   ch  要显示的字符
 * @param   color   颜色
*/
void lcd_panel_draw_char(uint16_t x, uint16_t y, unsigned char ch, uint16_t color){
    uint8_t temp;
    uint16_t y0 = y;
    uint8_t csize = 36;
    uint8_t *pfont = 0;
    ch = ch - ' ';      //ASCII码偏移

    uint16_t color_buf[] = {color};
    pfont = (uint8_t *)asc2_2412[ch];      //获取点阵数据
    
    for (uint8_t t = 0; t < csize; t++){
        temp = pfont[t];

        for (uint8_t t1 = 0; t1 < 8; t1++){
            if (temp & 0x80){
                esp_lcd_panel_draw_bitmap(panel_handle, x, y, x + 1, y + 1, color_buf);     //有效点
            } else {
                esp_lcd_panel_draw_bitmap(panel_handle, x, y, x + 1, y + 1, BLACK);
            }

            temp <<= 1;
            y++;

            if (y >= LCD_V) return;

            if ((y - y0) == 24){
                y = y0;
                x++;

                if (x >= LCD_H) return;

                break;
            }
        }
    }
}

/**
 * @brief   lcd显示字符串函数
 * @param   x               起始x坐标
 * @param   y               起始y坐标
 * @param   width           显示区域宽度
 * @param   height          显示其余高度
 * @param   str             要显示的字符串
 * @param   color           字符颜色  
*/
void lcd_panel_draw_string(uint8_t x, uint8_t y, char *p, uint16_t color){
    uint8_t x0 = x;

    while ((*p <= '~') && (*p >= ' ')){
        if (x > LCD_H){
            x = x0;
            y += 24;
        }

        if (y >= LCD_V) break;

        lcd_panel_draw_char(x, y, *p, color);
        x += 12;
        p++;
    }
}