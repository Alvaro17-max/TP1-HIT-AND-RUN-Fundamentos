#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "hitrun.h"

const int JUEGO_SIGUE = 0;
const int JUEGO_GANADO = 1;
const int JUEGO_PERDIDO = -1;

// Función auxiliar en juego.c para validar la acción ingresada por el usuario
bool es_jugada_valida(juego_t juego, char movimiento)
{
    // Si ingresó minúscula, lo normalizamos temporalmente para validar
    if (movimiento >= 'a' && movimiento <= 'z')
    {
        movimiento = (char)(movimiento - 'a' + 'A');
    }

    // 1. Validamos que el carácter sea uno de los comandos oficiales
    if (movimiento != 'W' && movimiento != 'S' && movimiento != 'A' && movimiento != 'D' &&
        movimiento != 'G' && movimiento != 'P' && movimiento != 'T')
    {
        return false;
    }

    // 2. Si es una habilidad, validamos que Homero tenga usos disponibles
    if (movimiento == 'G' && juego.homero.gruas_disponibles <= 0)
        return false;
    if (movimiento == 'P' && juego.homero.policias_disponibles <= 0)
        return false;
    if (movimiento == 'T' && juego.homero.turbos_disponibles <= 0)
        return false;

    // Nota: Los límites de mapa y colisiones en caminos para W, A, S, D se resuelven en hitrun.c.
    // El enunciado pide validar "lo que ingresa el usuario", por ende controlamos la entrada de caracteres.
    return true;
}

int main()
{
    srand((unsigned int)time(NULL));
    juego_t juego;
    inicializar_juego(&juego);

    printf("¡Bienvenido a Hit & Run: Salvando Springfield!\n");
    printf("Ayuda a Homero a conseguir el ancla y frenar el monorriel.\n");

    while (estado_juego(juego) == JUEGO_SIGUE)
    {
        mostrar_juego(juego);
        char movimiento = ' ';

        // Bucle de validación interactiva obligatorio por enunciado (Sección 4)
        do
        {
            printf("\nIntroduce tu movimiento (W/A/S/D - G: Grúa, P: Policía, T: Turbo): ");
            if (scanf(" %c", &movimiento) != 1)
            {
                movimiento = ' '; // Reseteamos ante fallos de buffer
            }

            if (!es_jugada_valida(juego, movimiento))
            {
                printf("Acción inválida o sin usos disponibles. Intenta nuevamente.\n");
            }
        } while (!es_jugada_valida(juego, movimiento));

        // Realizamos la jugada de forma segura ya que el caracter pasó el control
        realizar_jugada(&juego, movimiento);
    }

    // Dibujamos el tablero del desenlace final
    mostrar_juego(juego);

    int resultado = estado_juego(juego);
    if (resultado == JUEGO_GANADO)
    {
        printf("\n¡FELICITACIONES! ¡Homero llegó a tiempo y salvó a Springfield de la catástrofe! 🍩✨\n");
    }
    else if (resultado == JUEGO_PERDIDO)
    {
        printf("\n¡GAME OVER! El monorriel chocó, Homero se quedó sin nafta o tuviste un accidente. ☠️\n");
    }

    return 0;
}
