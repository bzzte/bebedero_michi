#include <stddef.h>
#include "bebedero.h"

// Límites calibrados con el sensor real (ver README)
#define LIMITE_VACIO_MEDIO  200
#define LIMITE_MEDIO_LLENO  1300
#define HISTERESIS          100

int bebedero_promedio(const int *muestras, int n)
{
    if (muestras == NULL || n <= 0) {
        return 0;
    }
    long suma = 0;
    for (int i = 0; i < n; i++) {
        suma += muestras[i];
    }
    return (int)(suma / n);
}

nivel_t bebedero_clasificar(int lectura, nivel_t anterior)
{
    switch (anterior) {
    case NIVEL_VACIO:
        if (lectura >= LIMITE_MEDIO_LLENO + HISTERESIS) return NIVEL_LLENO;
        if (lectura >= LIMITE_VACIO_MEDIO + HISTERESIS) return NIVEL_MEDIO;
        return NIVEL_VACIO;

    case NIVEL_MEDIO:
        if (lectura >= LIMITE_MEDIO_LLENO + HISTERESIS) return NIVEL_LLENO;
        if (lectura <  LIMITE_VACIO_MEDIO - HISTERESIS) return NIVEL_VACIO;
        return NIVEL_MEDIO;

    case NIVEL_LLENO:
        if (lectura < LIMITE_VACIO_MEDIO - HISTERESIS) return NIVEL_VACIO;
        if (lectura < LIMITE_MEDIO_LLENO - HISTERESIS) return NIVEL_MEDIO;
        return NIVEL_LLENO;
    }
    return NIVEL_VACIO;  // por si llega un valor inválido
}

const char *bebedero_nivel_texto(nivel_t nivel)
{
    switch (nivel) {
    case NIVEL_VACIO: return "VACIO";
    case NIVEL_MEDIO: return "MEDIO";
    case NIVEL_LLENO: return "LLENO";
    }
    return "DESCONOCIDO";
}

color_t bebedero_color(nivel_t nivel)
{
    switch (nivel) {
    case NIVEL_LLENO: return (color_t){ .r = false, .g = true,  .b = false }; // verde
    case NIVEL_MEDIO: return (color_t){ .r = true,  .g = true,  .b = false }; // amarillo
    case NIVEL_VACIO: return (color_t){ .r = true,  .g = false, .b = false }; // rojo
    }
    return (color_t){ .r = false, .g = false, .b = true };                    // azul = error
}

bool bebedero_debe_alertar(nivel_t anterior, nivel_t actual)
{
    return (actual == NIVEL_VACIO) && (anterior != NIVEL_VACIO);
}
#define ESPERA_ENTRE_VISITAS_MS  10000   // 10 s

void bebedero_visitas_reiniciar(visitas_t *v)
{
    v->total = 0;
    v->ultima_ms = 0;
    v->pir_anterior = false;
}

bool bebedero_visitas_actualizar(visitas_t *v, bool pir_activo, int64_t ahora_ms)
{
    bool flanco_subida = pir_activo && !v->pir_anterior;
    v->pir_anterior = pir_activo;

    if (!flanco_subida) {
        return false;
    }
    if (v->total > 0 && (ahora_ms - v->ultima_ms) < ESPERA_ENTRE_VISITAS_MS) {
        return false;   // es la misma visita, Michi sigue ahí
    }
    v->total++;
    v->ultima_ms = ahora_ms;
    return true;
}