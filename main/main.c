#include "lcd_driver.h"
#include "wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/task.h"

void lcd(void *arg);

void app_main(){
    xTaskCreate(lcd, "lcd", 4096, NULL, 5, NULL);
}

void lcd(void *arg){
    lcd_init();
    lcd_panel_set_backgrund(panel_handle, BLACK);
    lcd_panel_draw_char(50, 50, 'A', WHITE);
    lcd_panel_draw_string(0, 160, "Hello ESP32", WHITE);
    start_task_wifi_scan();
    while(1){
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}