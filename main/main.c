/* GPIO Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#define GPIO_OUTPUT_ENABLE     0
#define GPIO_OUTPUT_LEFT_STEP  1
#define GPIO_OUTPUT_LEFT_DIR   2
#define GPIO_OUTPUT_RIGHT_STEP 3
#define GPIO_OUTPUT_RIGHT_DIR  4

void app_main(void)
{
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 0b11111,
    };
    gpio_config(&io_conf);

    gpio_set_level(GPIO_OUTPUT_ENABLE, 0);
    gpio_set_level(GPIO_OUTPUT_LEFT_DIR, 0);
    gpio_set_level(GPIO_OUTPUT_RIGHT_DIR, 1);

    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 4000,
        .clk_cfg = LEDC_AUTO_CLK,
        .duty_resolution = LEDC_TIMER_13_BIT
    };

    ledc_timer_config(&timer_config);

    ledc_channel_config_t channel_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .gpio_num = GPIO_OUTPUT_LEFT_STEP,
        .intr_type = LEDC_INTR_DISABLE,
        .duty = 4096,
    };

    ledc_channel_config(&channel_config);

    int cnt = 0;

    while (1) {
        vTaskDelay(1);
        gpio_set_level(GPIO_OUTPUT_RIGHT_STEP, (cnt++) % 2);
    }
}
