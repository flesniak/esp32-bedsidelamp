/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include "esp_timer.h"
#include "esp_sleep.h"
#include "nvs_flash.h"

#include "wifi.h"

static const char *TAG = "example";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO CONFIG_BLINK_GPIO

static led_strip_handle_t led_strip;

static void fade_rgb_led(void* arg)
{
    const uint8_t fade_lut[] = {0, 1, 2, 4, 5, 6, 8, 10, 12, 14, 17, 20, 23, 28, 35, 47};
    const uint8_t idx_max = sizeof(fade_lut)-1;
    static uint8_t iteration = 0, idx = 0;
    uint8_t value = fade_lut[idx];
    idx += (iteration & 1) ? -1 : +1;
    switch (iteration) {
        case 0: case 1: led_strip_set_pixel(led_strip, 0, value, 0, 0); break;
        case 2: case 3: led_strip_set_pixel(led_strip, 0, 0, value, 0); break;
        default:        led_strip_set_pixel(led_strip, 0, 0, 0, value);
    }
    if (idx == idx_max || idx == 0) {
        if (iteration == 5)
            iteration = 0;
        else
            iteration++;
    }
    led_strip_refresh(led_strip);
}

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink addressable LED!");
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
        .max_leds = 1, // at least one LED on board
    };
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    /* Set all LED off to clear all pixels */
    led_strip_clear(led_strip);
}

void app_main(void)
{
    /* Configure the peripheral according to the LED type */
    configure_led();

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &fade_rgb_led,
        .name = "rgb_fade_timer"
    };

    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 40000));

    //Initialize NVS, required for WIFI
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();

    while (1)
        vTaskDelay(1000);

    // while (1) {
    //     ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
    //     blink_led();
    //     /* Toggle the LED state */
    //     s_led_state = !s_led_state;
    //     vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    // }
}
