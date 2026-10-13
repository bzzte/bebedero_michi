#pragma once
#include <stdbool.h>

typedef enum {
    NIVEL_VACIO = 0,
    NIVEL_MEDIO,
    NIVEL_LLENO
} nivel_t;

typedef struct {
    bool r;
    bool g;
    bool b;
} color_t;

int bebedero_promedio(const int *muestras, int n);
nivel_t bebedero_clasificar(int lectura, nivel_t anterior);
const char *bebedero_nivel_texto(nivel_t nivel);

// Color del LED para cada nivel
color_t bebedero_color(nivel_t nivel);

// true solo en el momento en que el plato pasa a VACIO
bool bebedero_debe_alertar(nivel_t anterior, nivel_t actual);