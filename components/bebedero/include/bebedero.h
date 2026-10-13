#pragma once
#include <stdbool.h>
#include <stdint.h>

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

typedef struct {
    int     total;          // visitas contadas
    int64_t ultima_ms;      // momento de la última visita contada
    bool    pir_anterior;   // estado del PIR en la lectura anterior
} visitas_t;

int bebedero_promedio(const int *muestras, int n);
nivel_t bebedero_clasificar(int lectura, nivel_t anterior);
const char *bebedero_nivel_texto(nivel_t nivel);
color_t bebedero_color(nivel_t nivel);
bool bebedero_debe_alertar(nivel_t anterior, nivel_t actual);

// Visitas: reinicia el contador
void bebedero_visitas_reiniciar(visitas_t *v);

// Visitas: devuelve true si esta lectura del PIR cuenta como visita nueva
bool bebedero_visitas_actualizar(visitas_t *v, bool pir_activo, int64_t ahora_ms);