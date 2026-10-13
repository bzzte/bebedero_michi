#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "bebedero.h"

#define N_MUESTRAS 5

// Pines (ver tabla de conexiones en el README)
#define PIN_R       25
#define PIN_G       26
#define PIN_B       33
#define PIN_BUZZER  23

// Pon 1 si el pin común de tu RGB va a "+" (ánodo común)
#define RGB_ANODO_COMUN 0

static const char *TAG = "bebedero";

static void configurar_salidas(void)
{
    gpio_config_t cfg = {
        .pin_bit_mask = (1ULL << PIN_R) | (1ULL << PIN_G) |
                        (1ULL << PIN_B) | (1ULL << PIN_BUZZER),
        .mode = GPIO_MODE_OUTPUT,
    };
    ESP_ERROR_CHECK(gpio_config(&cfg));
    gpio_set_level(PIN_BUZZER, 0);
}

static void poner_color(color_t c)
{
    // Con ánodo común el LED enciende con 0, así que se invierte
    gpio_set_level(PIN_R, RGB_ANODO_COMUN ? !c.r : c.r);
    gpio_set_level(PIN_G, RGB_ANODO_COMUN ? !c.g : c.g);
    gpio_set_level(PIN_B, RGB_ANODO_COMUN ? !c.b : c.b);
}

static void pitar(int veces)
{
    for (int i = 0; i < veces; i++) {
        gpio_set_level(PIN_BUZZER, 1);
        vTaskDelay(pdMS_TO_TICKS(150));
        gpio_set_level(PIN_BUZZER, 0);
        vTaskDelay(pdMS_TO_TICKS(150));
    }
}

void app_main(void)
{
    // ADC1, canal 6 = GPIO 34
    adc_oneshot_unit_handle_t adc1;
    adc_oneshot_unit_init_cfg_t init_cfg = { .unit_id = ADC_UNIT_1 };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_cfg, &adc1));

    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1, ADC_CHANNEL_6, &chan_cfg));

    configurar_salidas();

    int muestras[N_MUESTRAS] = {0};
    int indice = 0;
    nivel_t nivel = NIVEL_VACIO;
    poner_color(bebedero_color(nivel));

    while (1) {
        ESP_ERROR_CHECK(adc_oneshot_read(adc1, ADC_CHANNEL_6, &muestras[indice]));
        indice = (indice + 1) % N_MUESTRAS;

        int promedio = bebedero_promedio(muestras, N_MUESTRAS);
        nivel_t anterior = nivel;
        nivel = bebedero_clasificar(promedio, nivel);

        if (nivel != anterior) {
            ESP_LOGI(TAG, "Cambio: %s -> %s",
                     bebedero_nivel_texto(anterior), bebedero_nivel_texto(nivel));
            poner_color(bebedero_color(nivel));
        }
        if (bebedero_debe_alertar(anterior, nivel)) {
            ESP_LOGW(TAG, "¡Plato vacio! Avisando...");
            pitar(3);
        }

        ESP_LOGI(TAG, "Promedio: %4d  ->  %s", promedio, bebedero_nivel_texto(nivel));
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}