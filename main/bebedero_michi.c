#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"

static const char *TAG = "bebedero";

void app_main(void)
{
    // 1. Crear la unidad ADC1 (no ADC2, porque ADC2 no funciona con WiFi)
    adc_oneshot_unit_handle_t adc1;
    adc_oneshot_unit_init_cfg_t init_cfg = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_cfg, &adc1));

    // 2. Configurar el canal del GPIO 34
    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = ADC_ATTEN_DB_12,          // permite medir hasta ~3.3 V
        .bitwidth = ADC_BITWIDTH_DEFAULT,  // 12 bits: valores de 0 a 4095
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1, ADC_CHANNEL_6, &chan_cfg));

    // 3. Leer e imprimir cada 500 ms
    while (1) {
        int lectura = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adc1, ADC_CHANNEL_6, &lectura));
        ESP_LOGI(TAG, "Sensor de agua: %d", lectura);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}