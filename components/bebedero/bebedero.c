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