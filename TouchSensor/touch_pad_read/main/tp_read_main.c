/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/touch_pad.h"
#include "esp_log.h"

#define TOUCH_BUTTON_NUM    14
#define TOUCH_CHANGE_CONFIG 0
#define TOUCH_BASELINE_SAMPLES 10
#define TOUCH_SAMPLE_DELAY_MS 200

static const char *TAG = "touch read";
static const touch_pad_t button[TOUCH_BUTTON_NUM] = {
    TOUCH_PAD_NUM1,
    TOUCH_PAD_NUM2,
    TOUCH_PAD_NUM3,
    TOUCH_PAD_NUM4,
    TOUCH_PAD_NUM5,
    TOUCH_PAD_NUM6,
    TOUCH_PAD_NUM7,
    TOUCH_PAD_NUM8,
    TOUCH_PAD_NUM9,
    TOUCH_PAD_NUM10,
    TOUCH_PAD_NUM11,
    TOUCH_PAD_NUM12,
    TOUCH_PAD_NUM13,
    TOUCH_PAD_NUM14
};

/*
  Read values sensed at all available touch pads.
 Print out values in a loop on a serial monitor.
 */
static void tp_example_read_task(void *pvParameter)
{
    uint32_t baseline[TOUCH_BUTTON_NUM] = {0};
    uint32_t touch_value[TOUCH_BUTTON_NUM];
    uint64_t baseline_sum[TOUCH_BUTTON_NUM] = {0};

    printf("Calibrando durante %d segundos. No toques los sensores...\n",
           TOUCH_BASELINE_SAMPLES * TOUCH_SAMPLE_DELAY_MS / 1000);
    fflush(stdout);

    for (int sample = 0; sample < TOUCH_BASELINE_SAMPLES; sample++) {
        for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
            ESP_ERROR_CHECK(touch_pad_read_raw_data(button[i], &touch_value[i]));
            baseline_sum[i] += touch_value[i];
        }
        vTaskDelay(TOUCH_SAMPLE_DELAY_MS / portTICK_PERIOD_MS);
    }
    for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
        baseline[i] = baseline_sum[i] / TOUCH_BASELINE_SAMPLES;
    }

    while (1) {
        for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
            ESP_ERROR_CHECK(touch_pad_read_raw_data(button[i], &touch_value[i]));
        }

        printf("\033[2J\033[H");
        printf("Monitor tactil | cambio = actual - base (al tocar suele bajar)\n\n");
        printf("PAD   BASE     ACTUAL   CAMBIO\n");
        for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
            int64_t delta = (int64_t)touch_value[i] - (int64_t)baseline[i];
            printf("T%-2d  %6" PRIu32 "   %6" PRIu32 "   %+6" PRId64 "\n",
                   button[i], baseline[i], touch_value[i], delta);
        }
        fflush(stdout);
        vTaskDelay(TOUCH_SAMPLE_DELAY_MS / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    /* Initialize touch pad peripheral. */
    touch_pad_init();
    for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
        touch_pad_config(button[i]);
    }
#if TOUCH_CHANGE_CONFIG
    /* If you want change the touch sensor default setting, please write here(after initialize). There are examples: */
    touch_pad_set_measurement_interval(TOUCH_PAD_SLEEP_CYCLE_DEFAULT);
    touch_pad_set_charge_discharge_times(TOUCH_PAD_MEASURE_CYCLE_DEFAULT);
    touch_pad_set_voltage(TOUCH_PAD_HIGH_VOLTAGE_THRESHOLD, TOUCH_PAD_LOW_VOLTAGE_THRESHOLD, TOUCH_PAD_ATTEN_VOLTAGE_THRESHOLD);
    touch_pad_set_idle_channel_connect(TOUCH_PAD_IDLE_CH_CONNECT_DEFAULT);
    for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
        touch_pad_set_cnt_mode(button[i], TOUCH_PAD_SLOPE_DEFAULT, TOUCH_PAD_TIE_OPT_DEFAULT);
    }
#endif
    /* Denoise setting at TouchSensor 0. */
    touch_pad_denoise_t denoise = {
        /* The bits to be cancelled are determined according to the noise level. */
        .grade = TOUCH_PAD_DENOISE_BIT4,
        .cap_level = TOUCH_PAD_DENOISE_CAP_L4,
    };
    touch_pad_denoise_set_config(&denoise);
    touch_pad_denoise_enable();
    ESP_LOGI(TAG, "Denoise function init");

    /* Enable touch sensor clock. Work mode is "timer trigger". */
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    touch_pad_fsm_start();

    /* Start task to read values by pads. */
    xTaskCreate(&tp_example_read_task, "touch_pad_read_task", 4096, NULL, 5, NULL);
}
