#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "bebedero.h"

#define N_MUESTRAS 5

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
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1, ADC_CHANNEL_6, &chan_cfg));

    // 3. Variables para el promedio y el estado
    int muestras[N_MUESTRAS] = {0};
    int indice = 0;
    nivel_t nivel = NIVEL_VACIO;

    // 4. Leer, promediar, clasificar e imprimir cada 500 ms
    while (1) {
        ESP_ERROR_CHECK(adc_oneshot_read(adc1, ADC_CHANNEL_6, &muestras[indice]));
        indice = (indice + 1) % N_MUESTRAS;   // buffer circular

        int promedio = bebedero_promedio(muestras, N_MUESTRAS);
        nivel = bebedero_clasificar(promedio, nivel);

        ESP_LOGI(TAG, "Promedio: %4d  ->  %s", promedio, bebedero_nivel_texto(nivel));
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}