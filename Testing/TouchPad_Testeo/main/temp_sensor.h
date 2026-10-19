
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/temperature_sensor.h"

static const char *TAG = "ESP32_S2_TEMP";

void app_main(void)
{
    temperature_sensor_handle_t temp_sensor = NULL;

    // Configurar el sensor interno de temperatura
    temperature_sensor_config_t temp_sensor_config =
        TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);

    ESP_LOGI(TAG, "Inicializando sensor interno de temperatura...");

    ESP_ERROR_CHECK(
        temperature_sensor_install(&temp_sensor_config, &temp_sensor)
    );

    ESP_ERROR_CHECK(temperature_sensor_enable(temp_sensor));

    ESP_LOGI(TAG, "Sensor activado. Iniciando lecturas...");

    while (1) {
        float temperatura = 0.0f;

        esp_err_t err =
            temperature_sensor_get_celsius(temp_sensor, &temperatura);

        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Temperatura interna: %.2f grados C",
                     temperatura);
        } else {
            ESP_LOGE(TAG, "Error al leer temperatura: %s",
                     esp_err_to_name(err));
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
