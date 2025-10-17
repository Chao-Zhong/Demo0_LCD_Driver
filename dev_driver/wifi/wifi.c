#include<string.h>
#include"freertos/FreeRTOS.h"
#include"esp_wifi.h"
#include"esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"

#define SCAN_LIST_SIZE  16

static void wifi_scan(void){
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    uint16_t num = SCAN_LIST_SIZE;
    wifi_ap_record_t ap_info[SCAN_LIST_SIZE];
    uint16_t ap_cnt = 0;
    memset(ap_info, 0, sizeof(ap_info));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_wifi_scan_start(NULL, true);

    printf("Max AP number ap_info can be hold = %u\n", num);
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_cnt));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&num, ap_info));
    printf("Total APs scanned = %u, actaul AP number ap_info holds = %u\n", ap_cnt, num);
    for (int i = 0; i < num; i ++){
        printf("SSID \t\t%s\n", ap_info[i].ssid);
        printf("RSSI \t\t%d\n", ap_info[i].rssi);
    }
}

void app_wifi_scan(void *arg){
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    wifi_scan();
    while (1)
    {
        vTaskDelay(1);
    }
    
}

void start_task_wifi_scan(void){
    xTaskCreate(app_wifi_scan, "wifi_scan", 4096, NULL, 5, NULL);
}