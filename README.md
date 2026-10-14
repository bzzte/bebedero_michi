# Bebedero inteligente para Michi
Quien tiene una mascota no siempre sabe si el plato de agua está vacío. 
Se queda sin agua mientras estás en clase, trabajando o durmiendo, y no te enteras hasta que lo miras. 
Con los gatos es más delicado suelen tomar poca agua por naturaleza, así que pasar horas sin agua no les conviene.

Lo que hace el proyecto es avisa cuando el plato se vacía: LED rojo y pitidos, sin tener que revisarlo. 
Ademas muestra el nivel  para saber cuándo conviene rellenarlo, tambine llevala cuenta de las visitas y da una idea de con qué frecuencia va Michi al plato.
Un cambio fuerte en esa cifra puede ser una señal para prestarle atención.

## Hardware

**Placa:** ESP32 (ESP32-WROOM-32, revisión de chip v3.1)

| Componente | Pin del módulo | Pin del ESP32 |
|---|---|---|
| Sensor de nivel de agua | S | GPIO 34 |
| | + | 3V3 |
| | − | GND |
| PIR HC-SR501 | OUT | GPIO 27 |
| | VCC | VIN (5 V) |
| | GND | GND |
| Módulo LED RGB | R / G / B | GPIO 25 / 26 / 33 |
| | − (común) | GND |
| Buzzer activo | + | GPIO 23 |
| | − | GND |

El sensor de agua entrega un voltaje que varía (más agua = más voltaje).
Para leerlo como un número de 0 a 4095 hace falta un convertidor analógico-digital (ADC).
Muchos pines del ESP32 no tienen ADC, así que no sirve cualquiera.

## Requisitos de software

- ESP-IDF **v5.5.5**
- Target: **esp32**
- Probado en Windows con la terminal *ESP-IDF 5.5 CMD*

## Cómo compilar y flashear

Desde la carpeta raíz del proyecto:

```
idf.py set-target esp32
idf.py build
idf.py -p COMx flash monitor
```

Reemplazar `COMx` por el puerto de la placa (Administrador de dispositivos → Puertos COM y LPT). Para salir del monitor: `Ctrl + ]`.

 Lo que se ve en el monitor cuando funciona som las líneas de inicio de la placa (las que empiezan con boot: y cpu_start:). 
 Después aparece el mensaje del programa:

I (...) bebedero: Esperando ~1 min a que el PIR se estabilice...

Y luego, cada 500 ms, una línea con el estado completo:

I (...) bebedero: Agua: 1492 (LLENO) | PIR: 0 | Visitas: 4

## Pruebas unitarias

Las pruebas usan el framework **Unity** de ESP-IDF y se ejecutan en la placa:

```
cd test
idf.py set-target esp32
idf.py build
idf.py -p COMx flash monitor
```

## Funcionamiento y decisiones de diseño

### Promedio de lecturas
el sensor de agua da lecturas con ruido: aunque el sensor esté quieto, el valor salta. En tus primeras pruebas, sin promedio, viste cosas como:

1087 → 940 → 836 → 325 → 722

Ese 325 no significa que el agua bajó de repente; es una lectura “mala”. 
Si se clasificara cada lectura sola, ese valor podría cambiar el estado, encender el LED de otro color o incluso disparar la alarma sin motivo.

El promedio suaviza esos saltos: una lectura rara entre cinco normales casi no mueve el resultado.

### Histéresis
Sin histéresis habría un solo límite. Por ejemplo: menos de 200 es VACIO y 200 o más es MEDIO. 
El problema aparece cuando el valor queda justo alrededor del límite, algo muy común cuando el agua está a esa altura o quedan gotas.

### Alerta del buzzer
El buzzer suena 3 pitidos cortos, pero solo en el momento en que el plato pasa a VACIO, no mientras sigue vacío.
En cada vuelta del ciclo, el programa compara el nivel anterior con el actual usando la función bebedero_debe_alertar(anterior, actual). 
Esa función devuelve true solo cuando se cumplen las dos condiciones:

el nivel actual es VACIO, y
el nivel anterior no era VACIO.

### Contador de visitas
Si las visitas bajan mucho de un día para otro, puede ser una señal de que algo anda mal:
está enferma, estresada, o no le gusta el agua (sucia, vieja, el plato en mal lugar).

Si las visitas suben mucho, puede estar tomando más de lo normal. 
En gatos eso también puede ser señal de un problema de salud que vale la pena consultar con el veterinario.
Combinado con el nivel de agua, puedes ver si va al plato y lo encuentra vacío.

| Tiempo | PIR | ¿Cuenta? | Motivo |
|---|---|---|---|
| 47.3 s | 0 → 1 | Sí (#5) | Pasaron más de 10 s |
| 53.3 s | 0 → 1 | No | Solo 6 s después |
| 59.2 s | 0 → 1 | Sí (#6) | 11.9 s después |
| 67.2 s | 0 → 1 | No | Solo 8 s después |

## Calibración y limitaciones

1. Para medir el sensor real. Primero el programa solo imprimía la lectura cruda del ADC (0 a 4095), y se metio el sensor en el agua a distintas alturas

2. Elegir los límites iniciales. Con esos datos se pusieron dos límites, cada uno con una histéresis de ±100:

VACIO / MEDIO = 200: sube a MEDIO con ≥ 300 y baja a VACIO con < 100.
MEDIO / LLENO = 1300: sube a LLENO con ≥ 1400 y baja a MEDIO con < 1200.

3. Ajustar por las gotas. Para llegar a VACIO había que secar muy bien el sensor.
Al sacarlo del agua quedan gotas en las pistas, que siguen dando lecturas de ~200 y, en otra prueba, de ~400.
Como bajar a VACIO exigía menos de 100, se quedaba en MEDIO. se subio LIMITE_VACIO_MEDIO en bebedero.c, y con eso ya pasó a VACIO sin secarlo (con un valor de ~400).

## Estructura del proyecto

```
bebedero_michi/
├── CMakeLists.txt              # proyecto principal ESP-IDF
├── sdkconfig                   # configuración del proyecto
├── main/
│   ├── CMakeLists.txt
│   └── bebedero_michi.c        # hardware: ADC, GPIO, timer, ciclo principal
├── components/
│   └── bebedero/               # lógica pura, sin hardware (se prueba con Unity)
│       ├── CMakeLists.txt
│       ├── include/bebedero.h
│       └── bebedero.c
├── test/                       # proyecto de pruebas Unity
│   ├── CMakeLists.txt
│   └── main/test_bebedero.c
├── LICENSE
└── README.md
```


## Videos y fotos

https://drive.google.com/drive/folders/1T9KYpbd2cirvHXYK_JlyAra6lJa93L55?usp=sharing

## Autores
Danna Alfonso, Nicol Herrera, Duvan Rivera

Optativa II — Universidad de San Buenaventura, Bogotá — 2026-2

## Créditos

- APIs y documentación de [Espressif ESP-IDF](https://docs.espressif.com/projects/esp-idf/).




## Licencia

Este proyecto está bajo la licencia MIT. Ver [LICENSE](LICENSE).
