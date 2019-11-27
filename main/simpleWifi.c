#include "simpleWifi.h"

#include <string.h>
#include <esp_wifi.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_event.h>

#define LOG_NAME "Simple wifi"

static void event_handler(void* args, esp_event_base_t event_base, 
                                int32_t event_id, void* event_data)
{
    ESP_LOGI(LOG_NAME, "Got event: %s, %d", event_base, event_id);
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(LOG_NAME, "Wifi driver started, connecting to wifi.");
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(LOG_NAME, "Wifi driver was disconnected. Reconnecting...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(LOG_NAME, "Got event: got ip");
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(LOG_NAME, "Wifi driver got ipCalling handler: "IPSTR, IP2STR(&event->ip_info.ip));
        SimpleWifiCallback cb = (SimpleWifiCallback)args;
        cb();
    }
}

void init_simple_wifi(char *ssid, char *passwd, void *callback)
{
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, callback));

    wifi_config_t wifi_config = {};

    memcpy(&wifi_config.sta.ssid, ssid, strlen(ssid));
    memcpy(&wifi_config.sta.password, passwd, strlen(passwd));

    ESP_LOGI(LOG_NAME, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(LOG_NAME, "wifi_init_sta finished. Connecting.");
}