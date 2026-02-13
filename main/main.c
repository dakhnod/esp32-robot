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
#include "soc/interrupts.h"
#include "hal/ledc_ll.h"
#include "hal/gpio_ll.h"

#define GPIO_OUTPUT_ENABLE     0
#define GPIO_OUTPUT_LEFT_STEP  1
#define GPIO_OUTPUT_LEFT_DIR   2
#define GPIO_OUTPUT_RIGHT_STEP 3
#define GPIO_OUTPUT_RIGHT_DIR  4

#define DUTY_RESOLUTION_BITS 3

int counter = 0;

void IRAM_ATTR leds_handler() {
    uint32_t intr_status = LEDC.int_st.val;
    if ((intr_status & LEDC_OVF_CNT_LSCH0_INT_ST) == 0) {
        return;
    }
    LEDC.int_clr.val = LEDC_OVF_CNT_LSCH0_INT_ST;

    gpio_set_level(8, (counter++) % 2);

    LEDC.channel_group[LEDC_LOW_SPEED_MODE].channel[LEDC_CHANNEL_0].conf0.ovf_cnt_rst = 1;
}

void app_main(void)
{
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 0b11111 | (1 << 8),
    };
    gpio_config(&io_conf);

    gpio_set_level(GPIO_OUTPUT_ENABLE, 0);
    gpio_set_level(GPIO_OUTPUT_LEFT_DIR, 0);
    gpio_set_level(GPIO_OUTPUT_RIGHT_DIR, 1);

    esp_intr_alloc(ETS_LEDC_INTR_SOURCE, ESP_INTR_FLAG_LEVEL1 | ESP_INTR_FLAG_IRAM, leds_handler, NULL, NULL);

    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 4000,
        .clk_cfg = LEDC_AUTO_CLK,
        .duty_resolution = DUTY_RESOLUTION_BITS
    };

    ledc_timer_config(&timer_config);

    ledc_channel_config_t channel_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .gpio_num = GPIO_OUTPUT_LEFT_STEP,
        .intr_type = LEDC_INTR_DISABLE,
        .duty = 1 << (DUTY_RESOLUTION_BITS - 1),
    };

    ledc_channel_config(&channel_config);

    LEDC.channel_group[LEDC_LOW_SPEED_MODE].channel[LEDC_CHANNEL_0].conf0.ovf_num = 1000;
    LEDC.channel_group[LEDC_LOW_SPEED_MODE].channel[LEDC_CHANNEL_0].conf0.ovf_cnt_en = 1;
    LEDC.int_ena.val |= LEDC_OVF_CNT_LSCH0_INT_ST;
    LEDC.channel_group[LEDC_LOW_SPEED_MODE].channel[LEDC_CHANNEL_0].conf0.low_speed_update = 1;

    int cnt = 0;

    while (1) {
        vTaskDelay(1);
        gpio_set_level(GPIO_OUTPUT_RIGHT_STEP, (cnt++) % 2);
    }
}
