#define SSID "Ways"
#define PASS "ShowMeTheWorld"
#define LOGNAME "App"

#include "./simpleWifi.h"
#include <homekit.h>
#include <homekit_fascade.h>

#include <esp_system.h>
#include <esp_log.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <driver/gpio.h>

#define IO_PIR 21
SemaphoreHandle_t guard = NULL;
bool app_sth_is_moving = false;
void *app_characteristic;
bool initialized = false;

void *on_read()
{
    return &app_sth_is_moving;
}

void IRAM_ATTR on_isr(void *arg)
{
    xSemaphoreGiveFromISR(guard, NULL);
}

void motion_handler(void *args_ptr)
{
    const TickType_t delay = 50;
    for (;;)
    {
        if (!app_characteristic)
        {
            vTaskDelay(delay);
        }
        else if (xSemaphoreTake(guard, delay) == pdTRUE)
        {
            app_sth_is_moving = gpio_get_level(IO_PIR);
            hk_notify(app_characteristic);
        }
    }
}

void on_wifi_initialized()
{
    if(!initialized)
    {
        ESP_LOGI(LOGNAME, "Wifi initalized!");
        hk_init("Mosquito", HK_CAT_SWITCH, "111-22-222", ESP_LOG_DEBUG);
        //hk_reset();
        hk_setup_start();
        app_characteristic = hk_setup_add_motion_sensor(
            "Mosquito", "Slompf Industries", "A motion sensor.", "0000001", "0.1",
            true, on_read);
        hk_setup_finish();

        //init worker task
        vSemaphoreCreateBinary(guard);
        xTaskCreate(motion_handler, "motion_handler", 3072, NULL, 10, NULL);

        // init io
        gpio_set_direction(IO_PIR, GPIO_MODE_INPUT);
        gpio_set_intr_type(IO_PIR, GPIO_INTR_ANYEDGE);
        gpio_install_isr_service(0);
        gpio_isr_handler_add(IO_PIR, on_isr, (void *)IO_PIR);
            initialized = true;
    }
}

void app_main()
{
    ESP_LOGI(LOGNAME, "SDK version:%s\n", esp_get_idf_version());
    ESP_LOGI(LOGNAME, "Starting");

    // init wlan
    ESP_LOGD(LOGNAME, "Initializing wifi!\n");
    init_simple_wifi(SSID, PASS, on_wifi_initialized);
}