#define SSID "Ways"
#define PASS "ShowMeTheWorld"
#define LOGNAME "App"

#include <simpleWifi.h>
#include <homekit.h>
#include <homekit_fascade.h>

#include <esp_system.h>
#include <esp_log.h>

#define LED 19
bool value = false;


void set_led(bool value)
{
    ESP_LOGI(LOGNAME, "Setting led: %d", value);
    gpio_set_level(LED, value);
    value = value;
}

void on_light_write(void *data)
{
    value = *((bool *)data);
    set_led(value);
}

void on_light_identify()
{
    ESP_LOGI(LOGNAME, "Identify was called");
}

void *on_light_read()
{
    return &value;
}

void on_wifi_initialized()
{
    ESP_LOGI(LOGNAME, "Wifi initalized!");
    hk_init("Elk", HK_CAT_SWITCH, "111-22-222", ESP_LOG_DEBUG);
    //hk_reset();
    hk_setup_start();
    hk_setup_add_switch(
        "My Switch", "Slompf Industries", "The one and only", "0000001", "0.1",
        true, on_light_identify, on_light_read, on_light_write);
    hk_setup_finish();
}

void app_main()
{
    gpio_pad_select_gpio(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    set_led(true);

    ESP_LOGI(LOGNAME, "SDK version:%s\n", esp_get_idf_version());
    ESP_LOGI(LOGNAME, "Starting");

    ESP_LOGD(LOGNAME, "Initializing wifi!\n");
    init_simple_wifi(SSID, PASS, on_wifi_initialized);
}
