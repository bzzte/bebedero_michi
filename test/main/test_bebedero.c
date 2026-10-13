#include "unity.h"
#include "bebedero.h"

/* ---------- Promedio ---------- */

TEST_CASE("Promedio de tres lecturas", "[promedio]")
{
    int muestras[] = {100, 200, 300};
    TEST_ASSERT_EQUAL_INT(200, bebedero_promedio(muestras, 3));
}

TEST_CASE("Promedio sin muestras devuelve 0", "[promedio]")
{
    int muestras[] = {500};
    TEST_ASSERT_EQUAL_INT(0, bebedero_promedio(muestras, 0));
    TEST_ASSERT_EQUAL_INT(0, bebedero_promedio(NULL, 5));
}

/* ---------- Clasificación con histéresis ---------- */

TEST_CASE("Desde MEDIO necesita llegar a 1400 para ser LLENO", "[clasificar]")
{
    TEST_ASSERT_EQUAL(NIVEL_MEDIO, bebedero_clasificar(1350, NIVEL_MEDIO));
    TEST_ASSERT_EQUAL(NIVEL_LLENO, bebedero_clasificar(1400, NIVEL_MEDIO));
}

TEST_CASE("Desde LLENO no baja a MEDIO hasta pasar de 1200", "[clasificar]")
{
    // Caso real observado: 1309 se mantuvo en LLENO
    TEST_ASSERT_EQUAL(NIVEL_LLENO, bebedero_clasificar(1309, NIVEL_LLENO));
    TEST_ASSERT_EQUAL(NIVEL_MEDIO, bebedero_clasificar(1199, NIVEL_LLENO));
}

TEST_CASE("Llenar de golpe pasa de VACIO a LLENO", "[clasificar]")
{
    TEST_ASSERT_EQUAL(NIVEL_LLENO, bebedero_clasificar(2000, NIVEL_VACIO));
}

TEST_CASE("Texto de cada nivel", "[texto]")
{
    TEST_ASSERT_EQUAL_STRING("VACIO", bebedero_nivel_texto(NIVEL_VACIO));
    TEST_ASSERT_EQUAL_STRING("LLENO", bebedero_nivel_texto(NIVEL_LLENO));
}

TEST_CASE("La alerta suena solo al vaciarse", "[alerta]")
{
    // De MEDIO a VACIO debe alertar
    TEST_ASSERT_TRUE(bebedero_debe_alertar(NIVEL_MEDIO, NIVEL_VACIO));

    // Si ya estaba VACIO y sigue VACIO, NO debe alertar
    TEST_ASSERT_FALSE(bebedero_debe_alertar(NIVEL_VACIO, NIVEL_VACIO));

    // De VACIO a MEDIO (alguien llenó el plato) tampoco debe alertar
    TEST_ASSERT_FALSE(bebedero_debe_alertar(NIVEL_VACIO, NIVEL_MEDIO));
}

/* ---------- Ejecutar todas las pruebas ---------- */

void app_main(void)
{
    UNITY_BEGIN();
    unity_run_all_tests();
    UNITY_END();
}