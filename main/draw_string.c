#include "draw_string.h"
#include "lcdfont.h"

// 根据字体大小获取字符实际尺寸（匹配字模数组特性）
static void get_char_size(uint8_t size, int* width, int* height, int* bytes_per_row) {
    switch (size) {
        case 12:  // 12x6字体（asc2_1206字模）
            *width = 12;
            *height = 6;
            *bytes_per_row = 2;  // 12列需要2字节存储（12/8=1.5→向上取整）
            break;
        case 16:  // 16x8字体（asc2_1608字模）
            *width = 16;
            *height = 8;
            *bytes_per_row = 2;  // 16列需要2字节存储
            break;
        case 24:
            *width = 24;
            *height = 12;
            *bytes_per_row = 3;
            break;
        default:  // 默认使用16x8字体
            *width = 16;
            *height = 8;
            *bytes_per_row = 2;
    }
}
// 获取字符的点阵数据（仅支持ASCII码，这里简化处理）
const uint8_t* get_char_bitmap(char c, uint8_t front) {
    //检查字符有效性
    if (c < 32 || c > 126){
        return NULL;
    }

    //计算字符在字模数组中的索引（索引 = ASCII码 - 32）
    uint8_t char_index = c - 32;

    //根据字模宽度选择数组，返回对应字模首地址
    switch (front){
        case 12:
            return (const uint8_t *)asc2_1206[char_index];
        case 16:
            return (const uint8_t *)asc2_1608[char_index];
        case 24:
            return (const uint8_t *)asc2_2412[char_index];
    }
    return NULL;  // 未定义的字符返回空
}

// LCD显示字符串函数
// 参数：
//   panel_handle: LCD面板句柄
//   str: 要显示的字符串
//   x: 起始X坐标
//   y: 起始Y坐标
//   color: 字符颜色（RGB565格式，如0x00FF为红色）
//   bg_color: 背景颜色
//   size：字体大小
void lcd_draw_string(esp_lcd_panel_handle_t panel_handle, const char* str, int x, int y, uint16_t color, uint16_t bg_color, uint8_t size) {
       // 参数校验（防止越界和空指针）
    if (!panel_handle || !str || x < 0 || y < 0 || x >= LCD_H|| y >= LCD_V) {
        return;
    }
    int char_width, char_height, bytes_per_row;
    get_char_size(size, &char_width, &char_height, &bytes_per_row);
    const int char_spacing = 1;  // 字符间距（像素）

    int current_x = x;
    int current_y = y;

    // 遍历字符串中的每个字符
    for (int i = 0; str[i] != '\0'; i++) {
        // 为单个字符创建像素缓冲区
        static uint16_t char_buf[24 * 24];
        int buf_size = char_height * char_width;
        memset(char_buf, bg_color, buf_size * sizeof(uint16_t));  // 初始化为黑色（或背景色）
        const uint8_t* bitmap = get_char_bitmap(str[i], size);
        if (!bitmap) continue;  // 跳过未定义字符

        // 计算字符绘制区域（防止超出屏幕）
        if (current_x + char_width > LCD_V) {  // 横向超出，换行
            current_x = x;
            current_y += char_height + 2;  // 行间距2像素
            if (current_y + char_height > LCD_H) {  // 纵向超出，停止绘制
                break;
            }
        }

        // 解析点阵数据到像素缓冲区
        for (int row = 0; row < char_height; row++) {
            for (int col = 0; col < char_width; col++) {
                int byte_index = row * bytes_per_row + (col / 8);
                uint8_t bit_mask = 1 << (7 - (col % 8));
                uint8_t pixel = (bitmap[byte_index] & bit_mask) ? 1 : 0;
                    // 有效像素：设置为指定颜色
                    char_buf[row * char_width + col] = pixel ? color : bg_color;
                }
                // 无效像素：保持缓冲区初始值（背景色）
            }
        

        // 绘制单个字符到LCD（调用现有位图绘制函数）
        esp_lcd_panel_draw_bitmap(
            panel_handle,
            current_x, current_y,                  // 起始坐标
            current_x + char_width,        // 结束X坐标（起始X + 字符宽度）
            current_y + char_height,               // 结束Y坐标（起始Y + 字符高度）
            char_buf                       // 字符像素数据
        );

        // 移动到下一个字符的位置
        current_y += char_height + char_spacing;
    }
}