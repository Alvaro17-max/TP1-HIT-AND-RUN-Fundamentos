#ifndef __HITRUN_H__
#define __HITRUN_H__

#include <stdbool.h>

#define MAX_AUTOS 200
#define MAX_OBJETOS 150
#define MAX_FILAS 25
#define MAX_COLUMNAS 25
#define MAX_RECORRIDO 100
#define MAX_POSICIONES 4
#define MAX_CALLES 500

typedef struct coordenada {
    int fil;
    int col;
} coordenada_t;

typedef struct personaje {
    coordenada_t posicion;
    bool recolecto_ancla;
    int gruas_disponibles;
    int policias_disponibles;
    int turbos_disponibles;
    int movs_con_turbo;
    int nafta;
} personaje_t;

typedef struct vehiculo {
    int posicion;
    coordenada_t recorrido[MAX_RECORRIDO];
    int tope_recorrido;
    int nafta;
    int movimientos_frenado;
    int velocidad;
} vehiculo_t;

typedef struct monorriel {
    int posiciones[MAX_POSICIONES];
    int tope_posiciones;
    coordenada_t recorrido[MAX_RECORRIDO];
    int tope_recorrido;
    int movimientos_frenado;
} monorriel_t;

typedef struct objeto {
    char tipo;
    coordenada_t posicion;
} objeto_t;

typedef struct juego {
    personaje_t homero;
    vehiculo_t vehiculos[MAX_AUTOS];
    int tope_vehiculos;
    objeto_t objetos[MAX_OBJETOS];
    int tope_objetos;
    coordenada_t calles[MAX_CALLES];
    int tope_calles;
    coordenada_t ancla;
    monorriel_t monorriel;
    int movs_realizados;
} juego_t;

/*
 * Pre condiciones: -
 * Post condiciones: Inicializará el juego, cargando toda la información inicial de Homero, Moe, los bloques, las herramientas y los obstáculos.
 */
void inicializar_juego(juego_t* juego);


/*
 * Pre condiciones: El juego debe estar inicializado previamente con `inicializar_juego` y la acción
 * debe ser válida.
 * Post condiciones: Realizará la acción recibida por parámetro actualizando el juego.
 */
void realizar_jugada(juego_t* juego, char movimiento);

/*
 * Pre condiciones: El juego debe estar inicializado previamente con `inicializar_juego `.
 * Post condiciones: Imprime el juego por pantalla.
 */
void mostrar_juego(juego_t juego);

/*
 * Pre condiciones: El juego deberá estar inicializado previamente con `inicializar_juego `
 * Post condiciones: Devuelve:
 * --> 1 si es ganado
 * --> -1 si es perdido
 * --> 0 si se sigue jugando
 * El juego se dará por ganado cuando Homero recolecta el ancla y llega a la última estación antes del monorriel
 * Se dará por perdido si Homero se queda sin nafta o si choca con otro vehículo.
 */
int estado_juego(juego_t juego);


#endif // _HITRUN__H__