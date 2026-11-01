#ifndef __UTILES_H__
#define __UTILES_H__

#define MAX_CALLES 500
#define MAX_RECORRIDO 100

#include "hitrun.h"

/*
 * Recibe un vector de coordenadas y su tope. En el vector se almacenan las calles de la ciudad.
 * Inicializa el vector con las coordenadas de las calles.
 */
void obtener_calles(coordenada_t calles[MAX_CALLES], int *tope_calles);

/*
 * Recibe un vector de coordenadas y su tope
 * Inicializa el vector con las coordenadas de algún recorrido.
 */
void obtener_recorrido(coordenada_t recorrido[MAX_RECORRIDO], int *tope_recorrido);

#endif
