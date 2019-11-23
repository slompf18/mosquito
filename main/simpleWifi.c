#include "simpleWifi.h"

#include <string.h>
#include <esp_wifi.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_event_loop.h>
#include <nvs_flash.h>

#define LOG_NAME "Simple wifi"

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    //todo: check on router off/on
    switch (event->event_id)
    {
    case SYSTEM_EVENT_STA_START:
        ESP_LOGD(LOG_NAME, "Wifi driver started, connecting to wifi.");
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGD(LOG_NAME, "Wifi driver got ip: %s. Calling handler.",
                 ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));

        SimpleWifiCallback cb = (SimpleWifiCallback)ctx;
        cb();
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        ESP_LOGD(LOG_NAME, "Wifi driver was disconnected. Reconnecting...");
        esp_wifi_connect();
        break;

    case SYSTEM_EVENT_AP_STACONNECTED:
        ESP_LOGD(LOG_NAME, "A station connected to our soft access point:" MACSTR " join, AID=%d",
                 MAC2STR(event->event_info.sta_connected.mac),
                 event->event_info.sta_connected.aid);
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
        ESP_LOGD(LOG_NAME, "A station disconnected from our soft access point:" MACSTR "leave, AID=%d",
                 MAC2STR(event->event_info.sta_disconnected.mac),
                 event->event_info.sta_disconnected.aid);
        break;
    default:
        break;
    }

    return ESP_OK;
}

void init_simple_wifi(char *ssid, char *passwd, void *callback)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, callback));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    wifi_config_t wifi_config = {};

    memcpy(&wifi_config.sta.ssid, ssid, strlen(ssid));
    memcpy(&wifi_config.sta.password, passwd, strlen(passwd));

    ESP_LOGD(LOG_NAME, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(LOG_NAME, "wifi_init_sta finished. Connecting.");
}