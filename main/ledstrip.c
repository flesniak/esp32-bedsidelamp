#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"

#include "led_luts.h"

static const char *TAG = "ledstrip";

/* LED strip initialization with the GPIO and pixels number*/
static const led_strip_config_t strip_config = {
    .strip_gpio_num = 9,
    .max_leds = 30,
};
static const led_strip_rmt_config_t rmt_config = {
    .resolution_hz = 10 * 1000 * 1000, // 10MHz
};
static led_strip_handle_t led_strip;

void ledstrip_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Initializing for %lu leds on GPIO %d",
        strip_config.max_leds, strip_config.strip_gpio_num);
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));

    led_strip_clear(led_strip);

    uint8_t color = 0;
    uint8_t pulse_cnt = 0;
    uint8_t pulse_rise = 4;
    uint8_t pulse_full = 2;
    uint8_t pulse_fall = 4;
    uint8_t pulse_width = 20;
    const uint8_t pillars = 6;
    const uint8_t leds_per_pillar = 5;
    while (1) {
        for (int led=0; led<leds_per_pillar; led++) {
            uint8_t value = 0;

            // pulse_rise rising leds
            if (led <= pulse_cnt && pulse_cnt - led < pulse_rise)
                value = led_lut_16[pulse_cnt - led + 1];
            // pulse_full full leds
            else if (pulse_cnt - led >= pulse_rise && pulse_cnt - led < pulse_rise + pulse_full)
                value = led_lut_16[pulse_rise + 1];
            // pulse_fall falling leds
            else if (pulse_cnt - led >= pulse_rise + pulse_full && pulse_cnt - led < pulse_rise + pulse_full + pulse_fall)
                value = led_lut_16[pulse_rise + pulse_full + pulse_fall - pulse_cnt + led];

            uint8_t red = 0, green = 0, blue = 0;
            switch (color) {
                case 0: red = 3*value; break;
                case 1: green = value; break;
                case 2: blue = value; break;
            }

            for (int pillar=0; pillar<pillars; pillar++) {
                int this_led = leds_per_pillar*pillar + led;
                if (pillar & 1)
                    this_led = leds_per_pillar*(pillar+1) - led - 1;
                led_strip_set_pixel(led_strip, this_led, red, green, blue);
            }

            // led_strip_set_pixel(led_strip, i,
            //     i % 3 == 0 ? 255 : 0,
            //     i % 3 == 1 ? 255 : 0,
            //     i % 3 == 2 ? 255 : 0
            // );
        }
        led_strip_refresh(led_strip);

        pulse_cnt++;
        if (pulse_cnt == pulse_width) {
            pulse_cnt = 0;

            color++;
            if (color == 3)
                color = 0;
        }

        // vTaskDelay(40000);
        // ESP_LOGI(TAG, "Strip update");
        vTaskDelay(6);
    }

    vTaskDelete(NULL);
}
