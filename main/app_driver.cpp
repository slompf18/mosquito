/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_log.h>
#include <stdlib.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <device.h>
#include <esp_matter.h>

#include "app_driver.h"

using namespace chip::app::Clusters;
using namespace esp_matter;

#define IO_PIR 21
static const char *TAG = "app_driver";
static SemaphoreHandle_t guard = NULL;
static bool app_sth_is_moving = false;
static uint32_t endpoint_id;

static void app_driver_new_value()
{
    uint32_t cluster_id = OccupancySensing::Id;
    uint32_t attribute_id = OccupancySensing::Attributes::Occupancy::Id;

    node_t *node = node::get();
    endpoint_t *endpoint = endpoint::get(node, endpoint_id);
    cluster_t *cluster = cluster::get(endpoint, cluster_id);
    attribute_t *attribute = attribute::get(cluster, attribute_id);
    ESP_LOGW(TAG, "New value: %d (%d-%d-%d)", app_sth_is_moving, endpoint_id, cluster_id, attribute_id);
    
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    attribute::get_val(attribute, &val);
    val.val.b = app_sth_is_moving;
    attribute::update(endpoint_id, cluster_id, attribute_id, &val);
}

static void IRAM_ATTR app_driver_on_isr(void *arg)
{
    xSemaphoreGiveFromISR(guard, NULL);
}

static void app_driver_motion_handler(void *args)
{
    const TickType_t delay = 50;
    for (;;)
    {
         if (xSemaphoreTake(guard, delay) == pdTRUE)
        {
            bool sth_was_moving = app_sth_is_moving;
            app_sth_is_moving = gpio_get_level((gpio_num_t)IO_PIR);

            if (sth_was_moving != app_sth_is_moving)
            {
                ESP_LOGI(TAG, "Something is moving: %d", app_sth_is_moving);
                app_driver_new_value();
            }
        }
    }
}

esp_err_t app_driver_occu_init(uint32_t id_endpoint){
    endpoint_id = id_endpoint;

    // init worker task
    vSemaphoreCreateBinary(guard);
    xTaskCreate(app_driver_motion_handler, "motion_handler", 3072, NULL, 10, NULL);

    // init io config
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.pin_bit_mask = 1ULL << IO_PIR;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = (gpio_pullup_t)0;
    io_conf.pull_down_en = (gpio_pulldown_t)1;
    gpio_config(&io_conf);

    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(
        (gpio_num_t)IO_PIR, 
        app_driver_on_isr, 
        (void *)IO_PIR)
    );

    return ESP_OK;
}