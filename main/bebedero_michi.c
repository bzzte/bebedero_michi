#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "bebedero.h"

#define N_MUESTRAS 5

// Pines (ver tabla de conexiones en el README)
#define PIN_R       25
#define PIN_G       26
#define PIN_B       33
#define PIN_BUZZER  23
#define PIN_PIR     27

// Pon 1 si el pin común de tu RGB va a "+" (ánodo común)
#define RGB_ANODO_COMUN 0

static const char *TAG = "bebedero";

static void configurar_pines(void)
{
    gpio_config_t salidas = {
        .pin_bit_mask = (1ULL << PIN_R) | (1ULL << PIN_G) |
                        (1ULL << PIN_B) | (1ULL << PIN_BUZZER),
        .mode = GPIO_MODE_OUTPUT,
    };
    ESP_ERROR_CHECK(gpio_config(&salidas));
    gpio_set_level(PIN_BUZZER, 0);

    gpio_config_t entrada_pir = {
        .pin_bit_mask = (1ULL << PIN_PIR),
        .mode = GPIO_MODE_INPUT,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&entrada_pir));
}

static void poner_color(color_t c)
{
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

    configurar_pines();

    int muestras[N_MUESTRAS] = {0};
    int indice = 0;
    nivel_t nivel = NIVEL_VACIO;
    poner_color(bebedero_color(nivel));

    visitas_t visitas;
    bebedero_visitas_reiniciar(&visitas);

    ESP_LOGI(TAG, "Esperando ~1 min a que el PIR se estabilice...");

    while (1) {
        // --- Nivel de agua ---
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

        // --- Visitas de Michi ---
        bool pir = gpio_get_level(PIN_PIR);
        int64_t ahora_ms = esp_timer_get_time() / 1000;   // µs -> ms
        if (bebedero_visitas_actualizar(&visitas, pir, ahora_ms)) {
            ESP_LOGI(TAG, "Visita de Michi #%d", visitas.total);
        }

        ESP_LOGI(TAG, "Agua: %4d (%s) | PIR: %d | Visitas: %d",
                 promedio, bebedero_nivel_texto(nivel), pir, visitas.total);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}