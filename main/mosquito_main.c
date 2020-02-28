#define LOGNAME "App"

#include <hk.h>
#include <hk_fascade.h>

#include <esp_system.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <nvs_flash.h>
#include <esp_event.h>

#define IO_PIR 21
SemaphoreHandle_t guard = NULL;
bool app_sth_is_moving = false;
void *app_chr;
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
        if (!app_chr)
        {
            vTaskDelay(delay);
        }
        else if (xSemaphoreTake(guard, delay) == pdTRUE)
        {
            app_sth_is_moving = gpio_get_level(IO_PIR);
            hk_notify(app_chr);
        }
    }
}

void app_main()
{
    ESP_LOGI(LOGNAME, "SDK version:%s\n", esp_get_idf_version());
    ESP_LOGI(LOGNAME, "Starting");

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    hk_setup_start();
    app_chr = hk_setup_add_motion_sensor(
        "Mosquito", "Slompf Industries", "A motion sensor.", "0000001", "0.1",
        true, on_read);
    hk_setup_finish();
    //hk_reset();

    // init wlan
    ESP_LOGI(LOGNAME, "Wifi initalized!");
    hk_init("Bla", HK_CAT_OTHER, "111-22-222");
    
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