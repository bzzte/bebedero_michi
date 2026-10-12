#pragma once

typedef enum {
    NIVEL_VACIO = 0,
    NIVEL_MEDIO,
    NIVEL_LLENO
} nivel_t;

// Promedio de n lecturas. Devuelve 0 si n <= 0 o si muestras es NULL.
int bebedero_promedio(const int *muestras, int n);

// Clasifica una lectura del ADC (0-4095) según el nivel anterior (histéresis).
nivel_t bebedero_clasificar(int lectura, nivel_t anterior);

// Texto del nivel, para imprimir o enviar a la página web.
const char *bebedero_nivel_texto(nivel_t nivel);