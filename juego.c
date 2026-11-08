#include "hitrun.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define COMANDO_ARRIBA    'W'
#define COMANDO_ABAJO     'S'
#define COMANDO_IZQUIERDA 'A'
#define COMANDO_DERECHA   'D'

#define COMANDO_GRUA      'G'
#define COMANDO_POLICIA   'P'
#define COMANDO_TURBO     'T'

#define MINUSCULA_A       'a'
#define MINUSCULA_Z       'z'
#define MAYUSCULA_A       'A'

#define TOPE_MIN_HERRAMIENTAS 0
#define JUEGO_SIGUE     0
#define JUEGO_GANADO    1
#define JUEGO_PERDIDO  -1


/*
* Pre condiciones:
    -> 'movimiento' representa el carácter de acción ingresado desde la consola.
* Post condiciones:
    -> Devuelve true si el carácter coincide estrictamente con alguno de los comandos oficiales (direcciones COMANDO_ARRIBA, COMANDO_ABAJO, COMANDO_IZQUIERDA, COMANDO_DERECHA o habilidades COMANDO_GRUA, COMANDO_POLICIA, COMANDO_TURBO).
    -> Devuelve false en caso contrario.
*/
bool es_movimiento_valido(char movimiento){
    return !(movimiento != COMANDO_ARRIBA && movimiento != COMANDO_ABAJO && 
             movimiento != COMANDO_IZQUIERDA && movimiento != COMANDO_DERECHA && 
             movimiento != COMANDO_GRUA && movimiento != COMANDO_POLICIA && 
             movimiento != COMANDO_TURBO);
}

/*
* Pre condiciones:
    -> 'juego' debe ser una estructura 'juego_t' correctamente cargada e inicializada en memoria.
    -> 'movimiento' representa el carácter de acción ingresado por el usuario.
* Post condiciones:
    -> Si el carácter se encuentra en el rango de letras minúsculas (entre MINUSCULA_A y MINUSCULA_Z), lo normaliza transformándolo a su correspondiente mayúscula (MAYUSCULA_A).
    -> Devuelve true si la jugada es válida en el sistema y, en caso de ser una habilidad especial, Homero posee existencias disponibles en su estructura.
    -> Devuelve false en caso contrario (si el comando no existe o si no restan usos activos de la herramienta).
*/
bool es_jugada_valida(juego_t juego, char movimiento){
    if (movimiento >= MINUSCULA_A && movimiento <= MINUSCULA_Z){
        movimiento = (char)(movimiento - MINUSCULA_A + MAYUSCULA_A);
    }   
    if (!es_movimiento_valido(movimiento)){
        return false;
    }  
    if (movimiento == COMANDO_GRUA && juego.homero.gruas_disponibles <= TOPE_MIN_HERRAMIENTAS) {
        return false;
    }
    if (movimiento == COMANDO_POLICIA && juego.homero.policias_disponibles <= TOPE_MIN_HERRAMIENTAS) {
        return false;
    }
    if (movimiento == COMANDO_TURBO && juego.homero.turbos_disponibles <= TOPE_MIN_HERRAMIENTAS) {
        return false;
    } 
    return true;
}

int main(){
  srand((unsigned int)time(NULL)); 
  juego_t juego;
  inicializar_juego(&juego);
  printf("¡Bienvenido a Hit & Run: Salvando Springfield!\n");
  printf("Ayuda a Homero a conseguir el ancla y frenar el monorriel.\n");
  printf("M: monorriel, L: loma de burro, A: autos, C: ancla, E: estacion, H: Homero\n");

  while (estado_juego(juego) == JUEGO_SIGUE){
    system("clear");
    mostrar_juego(juego);
    char movimiento = ' ';
    bool jugada_valida = false;
    while (!jugada_valida){
      printf("\nIntroduce tu movimiento (W/A/S/D - G: Grúa, P: Policía, T: Turbo): ");
      if (scanf(" %c", &movimiento) != 1){
        movimiento = ' ';
      }
      if (es_jugada_valida(juego, movimiento)){
        jugada_valida = true;
      }else{
        printf("Acción inválida o sin usos disponibles. Intenta nuevamente.\n");
      }
    }
    realizar_jugada(&juego, movimiento);
  }
  system("clear");
  mostrar_juego(juego);
  int resultado = estado_juego(juego);
  if (resultado == JUEGO_GANADO){
    printf("\n¡FELICITACIONES! ¡Homero llegó a tiempo y salvó a Springfield de la catástrofe!. \n");
  }else if (resultado == JUEGO_PERDIDO){
    printf("\n¡GAME OVER! El monorriel chocó, Homero se quedó sin nafta o tuviste un accidente. \n");
  }
  return 0;
}
