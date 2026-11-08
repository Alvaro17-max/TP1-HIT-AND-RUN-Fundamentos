#include "hitrun.h"
#include "utiles.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define PUNTERO_VALIDO NULL
#define PASOS_NORMALES       1
#define NAFTA_GASTO_NORMAL   1
#define PASOS_CON_TURBO      2

const int MIN_FIL_POSICION = 0;
const int MIN_COL_POSICION = 0;
const int MAX_FIL_POSICION = 24;
const int MAX_COL_POSICION = 24;

const int FIL_INICIAL_HOMERO = 19;
const int COL_INICIAL_HOMERO = 19;
const bool ANCLA_NO_RECOLECTADA = false;
const int CANT_INICIAL_NAFTA = 100;
const int CANT_INICIAL_GRUAS = 5;
const int CANT_INICIAL_POLICIAS = 4;
const int CANT_INICIAL_TURBOS = 3;
const int CANT_INICIAL_MOVS_CON_TURBO = 0;
const int CANT_INICIAL_AUTOS = 6;

const int POSICION_INICIAL_TRAYECTO = 0;
const int NAFTA_MIN_AUTO = 30;
const int NAFTA_MAX_AUTO = 50;
const int VEL_MIN_AUTO = 1;
const int VEL_MAX_AUTO = 4;

const int MOV_INICIAL_FRENADO = 0;
const int FRECUENCIA_NUEVO_AUTO = 5;
const int NAFTA_MAX_HOMERO = 150;
const int NAFTA_GASTO_TURBO = 3;
const int CANT_DIRRCCIONES_CELDAS = 4;

const int POS_MIN_FIL_MONORRIEL = 1;
const int POS_MAX_FIL_MONORRIEL = 23;
const int POS_MIN_COL_MONORRIEL = 1;
const int POS_MAX_COL_MONORRIEL = 23;
const int CANT_VAGONES = 4;

const int DURACION_POLICIA_FRENADO = 5;
const int DURACION_EFECTO_TURBO = 5;
const int DURACION_FRENADO_SEMAFORO = 2;

const int CANT_ESTACIONES_SERVICIO = 20;
const int CANT_SEMAFOROS = 15;
const int CANT_LOMAS_BURRO = 10;
const int REFUELLO_ESTACION = 30;

const char MOVER_ARRIBA    = 'W';
const char MOVER_ABAJO     = 'S';
const char MOVER_IZQUIERDA = 'A';
const char MOVER_DERECHA   = 'D';

const char TIPO_LOMA_BURRO = 'L';
const char TIPO_ESTACION = 'E';
const char TIPO_SEMAFORO = 'S';
const char TIPO_ANCLA = 'C';

const char COMANDO_GRUA    = 'G';
const char COMANDO_POLICIA = 'P';
const char COMANDO_TURBO   = 'T';

const char MINUSCULA_A     = 'a';
const char MINUSCULA_Z     = 'z';
const char MAYUSCULA_A     = 'A';

const int JUEGO_GANADO = 1;
const int JUEGO_PERDIDO = -1;
const int JUEGO_SIGUE = 0;

/*
 * Pre condiciones: -
 * Post condiciones: 
 * Devuelve -true si los valores de la posicion1 (fila,columna) coincide con los valores de la posicion2 (fila, columna).
 * Devuelve -false- en caso contrario
 */
bool son_posiciones_iguales(coordenada_t posicion1, coordenada_t posicion2){
    return (posicion1.fil == posicion2.fil && posicion1.col == posicion2.col);
}

/*
 * Pre condiciones: -
 * Post condiciones: 
 * Devuelve la distancia Manhattan (la distancia entre ellos es la suma de los valores absolutos de las diferencias de las coordenadas)
 */
int distancia_manhattan(coordenada_t posicion1, coordenada_t posicion2){
    int distancia_filas = abs(posicion1.fil - posicion2.fil);
    int distancia_columnas = abs(posicion1.col - posicion2.col);
    return distancia_filas + distancia_columnas;
}

/*
 * Pre condiciones: -
 * Post condiciones: 
 * Devuelve true si la coordenada de la posicion se encuentra dentro de los límites del terreno (0 a 24) inclusiives
 */
bool es_posicion_dentro_del_rango(coordenada_t posicion){
    return (posicion.fil >= MIN_FIL_POSICION &&
            posicion.fil <= MAX_FIL_POSICION &&
            posicion.col >= MIN_COL_POSICION &&
            posicion.col <= MAX_COL_POSICION);
}

/*
 * Pre condiciones:
 * El vector de 'calles' debe estar inicializado.
 * El valor de 'tope_calles' debe ser > 0.
 * Post condiciones: 
 * Devuelve true si la coordenada de posicion coincide con alguna calle registrada en el vector, o false en caso contrario.
 */
bool es_calle(coordenada_t calles[MAX_CALLES], int tope_calles, coordenada_t posicion){
    bool en_calle = false;
    int indice_calle = 0;
    while (!en_calle && (indice_calle < tope_calles)){
        if (son_posiciones_iguales(calles[indice_calle], posicion)){
            en_calle = true;
        }else{
            indice_calle++;
        }
    }
    return en_calle;
}

/*
 * Pre condiciones:
 *  -> El vector de 'calles' debe estar inicializado.
 *  -> El valor de 'tope_calles' debe ser > 0.
 * Post condiciones: 
 * Devuelve true si la coordenada "posicion" NO es una calle y hay al menos una calle en el vector 
   de calles en la que la distancia Manhattan con respecto a "posicion" es igual a 1. 
 * Devuelve false en caso contrario (si "posicion" ya es una calle o si se encuentra a una distancia mayor de todas las calles)
 */
bool es_calle_vecina(coordenada_t posicion, coordenada_t calles[MAX_CALLES], int tope_calles){
    bool es_vecina = false;
    if (!es_calle(calles, tope_calles, posicion)){
        int i = 0;
        while (!es_vecina && (i < tope_calles)){
            if (distancia_manhattan(posicion, calles[i]) == 1){
                es_vecina = true;
            }else{
                i++;
            }
        }
    }
    return es_vecina;
}

/* 
 * Pre condiciones: -
 * Post condiciones: 
 * Devuelve true si la coordenada coincide exactamente con alguna de las 4 esquinas del monorriel 
   (1,1), (1,23), (23,23) o (23,1).
 */
bool es_esquina_monorriel(coordenada_t posicion){
    return (posicion.fil == POS_MIN_FIL_MONORRIEL &&
            posicion.col == POS_MIN_COL_MONORRIEL) ||
           (posicion.fil == POS_MIN_FIL_MONORRIEL &&
            posicion.col == POS_MAX_COL_MONORRIEL) ||
           (posicion.fil == POS_MAX_FIL_MONORRIEL &&
            posicion.col == POS_MAX_COL_MONORRIEL) ||
           (posicion.fil == POS_MAX_FIL_MONORRIEL &&
            posicion.col == POS_MIN_COL_MONORRIEL);
}

/* 
 * Pre condiciones : 
 * El monorriel debe tener su recorrido cargado en la memoria (el vector de coordenadas)
 * El tope del recorrido del monorriel debe ser mayor a 0. 
 * Post condiciones: 
 * Devuelve true si la fila y columna de alguna de las posiciones del recorrido del monorriel coincide exactamente 
   con la fila y columna de la coordenada "posicion" pasada por paramtero. 
 * Devuelve false en caso contrario
 */
bool esta_en_recorrido_monorriel(coordenada_t posicion, juego_t *juego){
    bool esta_sobre_vias = false;
    int i = 0;
    while (!esta_sobre_vias && (i < juego->monorriel.tope_recorrido)){
        if (son_posiciones_iguales(posicion, juego->monorriel.recorrido[i])){
            esta_sobre_vias = true;
        }else{
            i++;
        }
    }
    return esta_sobre_vias;
}

/* 
 * Pre condiciones : 
 * El vector de vehiculos debe estar correctamente inicializado 
 * int*tope_vehiculos debe estar inicializado y apuntar a un valor entero válido mayor a 0 
 * El valor de poscion_vehiculo debe ser un numero valido  mayor a cero y menor al valor de *tope_vehiculos.
 * Post condiciones: 
 * Elimina el vehiculo que se enceuntra en el indice de "posicion_vehiculo".
 * Por cada auto quitado resta uno al valor guardado en la direccion del puntero *tope_vehiculos.
 */
void eliminar_vehiculo(vehiculo_t vehiculos[MAX_AUTOS], int *tope_vehiculos, int posicion_vehiculo){
    int i = posicion_vehiculo;
    while (i < (*tope_vehiculos) - 1){
        vehiculos[i] = vehiculos[i + 1];
        i++;
    }
    (*tope_vehiculos) -= 1;
}

/*
 * Pre condiciones: 
 * Los punteros posicion_inicio y posicion_fin deben apuntar a valores enterors validos 
 * Post condiciones: 
 * Compara los valores de coordenada_homero" y "coordenada_auto" y guarda el menor en la direccion de
   memoria apuntada por "posicion_inicio" y "posicion_fin"
 */
void obtener_extremos_tramo(int coordenada_homero, int coordenada_auto, int *posicion_inicio, int *posicion_fin){
    if (coordenada_homero < coordenada_auto){
        *posicion_inicio = coordenada_homero;
        *posicion_fin = coordenada_auto;
    }else{
        *posicion_inicio = coordenada_auto;
        *posicion_fin = coordenada_homero;
    }
}

/*
 * Pre condiciones:
 *  -> El vector de 'calles' debe estar inicializado.
 *  -> El valor de 'tope_calles' debe ser > 0.
 *  -> 'columna_inicial' debe ser menor que 'columna_final' (valores obtenidos al ordenar los extremos del tramo).
 *  -> 'fila_fija' debe representar una fila válida dentro de los límites del terreno.
 * Post condiciones: 
 * Devuelve true si todos los casilleros intermedios del tramo evaluado de forma horizontal
   (desde columna_inicial + 1 hasta antes de columna_final) coinciden con una calle registrada en el vector.
 * Devuelve false en caso contrario (si encuentra al menos un bache, vereda o terreno que no sea calle).
 */
bool verificar_camino_horizontal(int columna_inicial, int columna_final, int fila_fija, coordenada_t calles[MAX_CALLES], int tope_calles){
    bool es_continuo = true;
    int i = columna_inicial + 1;
    coordenada_t casillero_intermedio;
    casillero_intermedio.fil = fila_fija;

    while (es_continuo && (i < columna_final)){
        casillero_intermedio.col = i;
        if (!es_calle(calles, tope_calles, casillero_intermedio)){
            es_continuo = false;
        }else{
            i++;
        }
    }
    return es_continuo;
}

/*
 * Pre condiciones:
 *  -> El vector de 'calles' debe estar inicializado.
 *  -> El valor de 'tope_calles' debe ser > 0.
 *  -> 'fila_inicial' debe ser menor que 'fila_final' (valores obtenidos al ordenar los extremos del tramo).
 *  -> 'columna_fija' debe representar una columna válida dentro de los límites del terreno.
 * Post condiciones: 
 * Devuelve true si todos los casilleros intermedios del tramo evaluado de forma vertical
   (desde fila_inicial + 1 hasta antes de fila_final) coinciden con una calle registrada en el vector.
 * Devuelve false en caso contrario (si encuentra al menos un bache, vereda o terreno que no sea calle).
 */
bool verificar_tramo_vertical(int fila_inicial, int fila_final, int columna_fija, coordenada_t calles[MAX_CALLES], int tope_calles){
    bool es_continuo = true;
    int i = fila_inicial + 1;
    coordenada_t casillero_intermedio;
    casillero_intermedio.col = columna_fija;

    while (es_continuo && (i < fila_final)){
        casillero_intermedio.fil = i;
        if (!es_calle(calles, tope_calles, casillero_intermedio)){
            es_continuo = false;
        }else{
            i++;
        }
    }
    return es_continuo;
}

/*
 * Pre condiciones: 
 * 'posicion_homero' y 'posicion_auto' representan posiciones en la misma (fila o columna). 
 * 'calles' debe está correctamente inicializado con un tope válido. 
 * Post condiciones: 
 * Devuelve true si el trayecto de calles entre Homero y el auto es una línea recta continua sin baches.
 */
bool calle_es_continua(coordenada_t posicion_homero, coordenada_t posicion_auto, coordenada_t calles[MAX_CALLES], int tope_calles){
    bool es_continua = true;
    int posicion_inicio;
    int posicion_fin;
    if (posicion_homero.fil == posicion_auto.fil){
        obtener_extremos_tramo(posicion_homero.col, posicion_auto.col, &posicion_inicio, &posicion_fin);
        es_continua = verificar_camino_horizontal(posicion_inicio, posicion_fin, posicion_homero.fil, calles, tope_calles);
    }
    else if (posicion_homero.col == posicion_auto.col){
        obtener_extremos_tramo(posicion_homero.fil, posicion_auto.fil, &posicion_inicio, &posicion_fin);
        es_continua = verificar_tramo_vertical(posicion_inicio, posicion_fin, posicion_homero.col, calles, tope_calles);
    }
    return es_continua;
}

/*
 * Pre condiciones:  
 * El puntero juego debe apuntar a una estructura "juego_t" corretamente inicializada. 
 * El juego debe tener cargado los datos de Homero, el monorriel y el vector de objetos con un tope valido.
 * Post condiciones: 
 * Dvuelve true si la coordenada posicion coincide con la ubicacion actual de homero, con cualquier 
   celda que compone el recorrido del monoriel o la posicion de algun objeto ya existente. 
 * Devuelve False en caso contrario (si el casillero consultado se encuentre libre).
 */
bool posicion_ocupada(coordenada_t posicion, juego_t *juego){
    bool esta_ocupada = false;
    if (son_posiciones_iguales(posicion, juego->homero.posicion)){
        esta_ocupada = true;
    }
    if (!esta_ocupada && son_posiciones_iguales(posicion, juego->ancla)){
        esta_ocupada = true;
    }
    if (!esta_ocupada && esta_en_recorrido_monorriel(posicion, juego)){
        esta_ocupada = true;
    }
    int i = 0;
    while (!esta_ocupada && (i < juego->tope_objetos)){
        if (son_posiciones_iguales(posicion, juego->objetos[i].posicion)){
            esta_ocupada = true;
        }
        i++;
    }
    return esta_ocupada;
}

/*
 * Pre condiciones: 
 * 'personaje' debe apuntar a una dirección de memoria válida asignada para una estructura 'personaje_t'.
 * Post condiciones: 
 * Si 'personaje' es distinto de PUNTERO_VALIDO, Carga los valores iniciales de Homero: posición (19,19).
 */
void inicializar_personajes(personaje_t *personaje){
    if (personaje != PUNTERO_VALIDO){
        personaje->posicion.fil = FIL_INICIAL_HOMERO;
        personaje->posicion.col = COL_INICIAL_HOMERO;
    }
}

/*
 * Pre condiciones:
 *  -> 'personaje' debe apuntar a una dirección de memoria válida asignada para una estructura 'personaje_t'.
 * Post condiciones: 
 * Si 'personaje' es distinto de PUNTERO_VALIDO, inicializa el estado de las herramientas,
   habilidades y combustible de Homero con los valores por defecto definidos en las constantes del juego
 * (0 turnos de turbo, 100 L de nafta iniciales, 4 policias iniciales , 5 gruas iniciales, y el ancla sin recolectar).
 */
void inicializar_objetos_herramientas(personaje_t *personaje){
    if (personaje != PUNTERO_VALIDO){
        personaje->recolecto_ancla = ANCLA_NO_RECOLECTADA;
        personaje->gruas_disponibles = CANT_INICIAL_GRUAS;
        personaje->policias_disponibles = CANT_INICIAL_POLICIAS;
        personaje->turbos_disponibles = CANT_INICIAL_TURBOS;
        personaje->movs_con_turbo = CANT_INICIAL_MOVS_CON_TURBO;
        personaje->nafta = CANT_INICIAL_NAFTA;
    }
}

/*
* Pre condiciones:
    -> El puntero 'auto_nuevo' debe apuntar a una dirección de memoria válida asignada para una estructura 'vehiculo_t'.
* Post condiciones:
    -> Carga el recorrido oficial de la cátedra en el vector de recorrido del vehículo mediante la función obtener_recorrido.
    -> Inicializa el índice de posición actual en la constante de salida (POSICION_INICIAL_TRAYECTO).
    -> Asigna un valor aleatorio de combustible entre NAFTA_MIN_AUTO y NAFTA_MAX_AUTO inclusive.
    -> Asigna una velocidad aleatoria entre VEL_MIN_AUTO y VEL_MAX_AUTO inclusive.
    -> Asigna el estado de frenado inicial en la constante (MOV_INICIAL_FRENADO).
*/
void inicializar_auto_individual(vehiculo_t *auto_nuevo){
    obtener_recorrido(auto_nuevo->recorrido, &(auto_nuevo->tope_recorrido));    
    auto_nuevo->posicion = POSICION_INICIAL_TRAYECTO; 
    auto_nuevo->nafta = (rand() % ((NAFTA_MAX_AUTO - NAFTA_MIN_AUTO) + 1)) + NAFTA_MIN_AUTO;
    auto_nuevo->velocidad = (rand() % ((VEL_MAX_AUTO - VEL_MIN_AUTO) + 1)) + VEL_MIN_AUTO;
    auto_nuevo->movimientos_frenado = MOV_INICIAL_FRENADO;
}


/*
 * Pre condiciones:
 *  -> 'juego' deb apuntar a una direccion de memoria válida asignada para una estructura 'juego_t'.
 *  ->  Las constantes 'CANT_INICIAL_AUTOS' y 'MAX_AUTOS' deben estar inicializadas con valores > 0.
 * Post condiciones: 
 * Si 'juego' es distinto de PUNTERO_VALIDO, inicializa el contador 'tope_vehiculos' en 0 y llena
   el vector de tráfico llamando Iterativamente a 'inicializar_auto_individual' hasta alcanzar la cantidad
   inicial requerida, garantizando no superar el límite físico de la matriz.
 */
void inicializar_autos_juego(juego_t *juego){
    if (juego != PUNTERO_VALIDO){
        juego->tope_vehiculos = 0;
        while (juego->tope_vehiculos < CANT_INICIAL_AUTOS && juego->tope_vehiculos < MAX_AUTOS){
            inicializar_auto_individual(&(juego->vehiculos[juego->tope_vehiculos]));
            juego->tope_vehiculos++;
        }
    }
}

/*
 * Pre condiciones:
 *  -> 'auto_actual' debe apuntar a una estructura 'vehiculo_t' inicializada en memoria.
 * Post condiciones: 
 *  -> Devuelve true si el vehículo tiene nafta > 0 y su contador de movimientos_frenado es 0.
 *  -> Devuelve false en caso contrario (si el vehículo no tiene nafta o si tiene movimientos de frenado > 0).
 */
bool auto_puede_circular(vehiculo_t *auto_actual){
    bool puede = true;
    if (auto_actual->movimientos_frenado > 0){
        puede = false;
    }else if (auto_actual->nafta <= 0){
        puede = false;
    }
    return puede;
}

/*
 * Pre condiciones:
 *  -> 'auto_actual' debe ser un puntero válido que apunte a una estructura 'vehiculo_t' en memoria.
 *  -> El vehículo debe cumplir las condiciones para circular (con nafta > 0 y movimientos de frenado = 0)
 * Post condiciones: Descuenta la nafta del vehículo de forma proporcional a su velocidad actual,
   asegurando que el nivel de nafta no disminuya de 0, y aumenta su índice de posición dentro del recorrido
   en una cantidad de casilleros equivalente a dicha velocidad.
 */
void mover_y_descontar_nafta_auto(vehiculo_t *auto_actual){
    int gasto_nafta = auto_actual->velocidad;
    auto_actual->nafta -= gasto_nafta;
    if (auto_actual->nafta < 0){
        auto_actual->nafta = 0;
    }
    auto_actual->posicion += auto_actual->velocidad;
}

/*
 * Pre condiciones:
 *  -> El vector 'vehiculos' debe estar correctamente inicializado.
 *  -> tope_vehiculos' debe ser un puntero válido a la variable que contiene la cantidad de autos actuales.
 *  -> El índice 'i' debe ser un valor válido dentro de los límites del vector (0 <= i < *tope_vehiculos).
 * Post condiciones: 
 * Comprueba si el vehículo en la posición 'i' es mayor o igual a su tope máximo de recorrido.
    En caso afirmativo, llama a eliminar_vehiculo para removerlo de la circulación.
 */
void verificar_y_eliminar_auto_terminado(vehiculo_t vehiculos[MAX_AUTOS], int *tope_vehiculos, int i){
    if (vehiculos[i].posicion >= vehiculos[i].tope_recorrido){
        eliminar_vehiculo(vehiculos, tope_vehiculos, i);
    }
}


/*
 * Pre condiciones:
 *  -> 'juego' debe apuntar a una estructura 'juego_t' correctamente inicializada en memoria.
 * Post condiciones: 
 * Desplaza secuencialmente de manera invertida únicamente los vehículos activos que tengan nafta
   y no estén frenados. No descuenta turnos de frenado en este caso.
 * -> verifica y elimina de la circulación a los autos que hayan completado con éxito su recorrido
      mediante la función 'verificar_y_eliminar_auto_terminado'.  
 */
void actualizar_y_mover_autos(juego_t *juego){
    if (juego != PUNTERO_VALIDO){
        int i = juego->tope_vehiculos - 1;
        while (i >= 0){
            vehiculo_t *auto_actual = &(juego->vehiculos[i]);
            if (auto_puede_circular(auto_actual)){
                mover_y_descontar_nafta_auto(auto_actual);
            }
            verificar_y_eliminar_auto_terminado(juego->vehiculos, &(juego->tope_vehiculos), i);
            i--;
        }
    }
}

/*
 * Pre condiciones:
 *  -> El vector 'recorrido' debe estar declarado previamente con espacio en memoria.
 *  -> Las constantes de posición mínima y máxima de fila/columna del monorriel deben estar inicializadas con valores > 0.
 * Post condiciones: Carga iterativamente las coordenadas del recorrido cerrado en sentido antihorario recorriendo el perímetro.
 * Devuelve un número entero que representa la cantidad total de casilleros válidos que conforman el camino del monorriel.
 */
int trazar_camino_monorriel(coordenada_t recorrido[MAX_RECORRIDO]){
    int tope = 0;
    int fila = POS_MIN_FIL_MONORRIEL;
    while (fila < POS_MAX_FIL_MONORRIEL){
        recorrido[tope].fil = fila;
        recorrido[tope].col = POS_MIN_COL_MONORRIEL;
        tope++;
        fila++;
    }
    int columna = POS_MIN_COL_MONORRIEL;
    while (columna < POS_MAX_COL_MONORRIEL){
        recorrido[tope].fil = POS_MAX_FIL_MONORRIEL;
        recorrido[tope].col = columna;
        tope++;
        columna++;
    }
    fila = POS_MAX_FIL_MONORRIEL;
    while (fila > POS_MIN_FIL_MONORRIEL){
        recorrido[tope].fil = fila;
        recorrido[tope].col = POS_MAX_COL_MONORRIEL;
        tope++;
        fila--;
    }
    columna = POS_MAX_COL_MONORRIEL;
    while (columna > POS_MIN_COL_MONORRIEL){
        recorrido[tope].fil = POS_MIN_FIL_MONORRIEL;
        recorrido[tope].col = columna;
        tope++;
        columna--;
    }
    return tope;
}

/*
 * Pre condiciones:
 *  -> 'monorriel'debe apuntar a una estructura 'monorriel_t' correctamente inicializada en memoria.
 *  -> Las constantes de posición mínima de fila y columna del monorriel deben estar inicializadas con valores > 0.
 * Post condiciones: 
 * Si 'monorriel' es distinto de PUNTERO_VALIDO, inicializa por completo el estado del tren cargando su recorrido perimetral,
   volviendo a convertir sus movimientos de frenado en 0 y asociando el índice de posición de sus 4 vagones de salida de forma estructurada.
 */
void inicializar_monorriel(monorriel_t *monorriel){
    if (monorriel != PUNTERO_VALIDO){
        monorriel->tope_recorrido = trazar_camino_monorriel(monorriel->recorrido);
        monorriel->movimientos_frenado = 0;
        monorriel->tope_posiciones = MAX_POSICIONES;

        coordenada_t metas_vagones[] = {
            {POS_MIN_FIL_MONORRIEL, POS_MIN_COL_MONORRIEL},
            {POS_MIN_FIL_MONORRIEL, POS_MIN_COL_MONORRIEL + 1},
            {POS_MIN_FIL_MONORRIEL, POS_MIN_COL_MONORRIEL + 2},
            {POS_MIN_FIL_MONORRIEL, POS_MIN_COL_MONORRIEL + 3}};
        for (int vagon = 0; vagon < MAX_POSICIONES; vagon++){
            bool vagon_ubicado = false;
            int j = 0;
            while (!vagon_ubicado && (j < monorriel->tope_recorrido)){
                if (son_posiciones_iguales(monorriel->recorrido[j], metas_vagones[vagon])){
                    monorriel->posiciones[vagon] = j;
                    vagon_ubicado = true;
                }else{
                    j++;
                }
            }
        }
    }
}


/*
* Pre condiciones:
    -> 'movimiento' debe ser un carácter que coincida con una tecla de dirección válida del sistema.
    -> 'sig_fil' y 'sig_col' deben ser punteros válidos que apunten a direcciones de memoria asignadas.
* Post condiciones:
    -> Modifica por referencia los valores enteros de la fila y la columna apuntadas según la dirección indicada por 
    las constantes (MOVER_ARRIBA, MOVER_ABAJO, MOVER_IZQUIERDA o MOVER_DERECHA).
*/
void calcular_desplazamiento(char movimiento, int *sig_fil, int *sig_col){
    if (movimiento == MOVER_ARRIBA){
        (*sig_fil)--;
    }
    else if (movimiento == MOVER_ABAJO){
        (*sig_fil)++;
    }
    else if (movimiento == MOVER_IZQUIERDA){
        (*sig_col)--;
    }
    else if (movimiento == MOVER_DERECHA){
        (*sig_col)++;
    }
}

/*
* Pre condiciones:
    -> El vector de 'calles' debe estar correctamente inicializado con un 'tope_calles' > 0.
* Post condiciones:
    -> Devuelve true si la coordenada de la 'posicion_siguiente' se encuentra dentro de los límites del terreno y coincide con una calle válida por la que se puede circular.
    -> Devuelve false en caso contrario (si se sale del terreno o si es una vereda/obstáculo).
*/
bool verificar_paso_valido(coordenada_t posicion_siguiente, coordenada_t calles[MAX_CALLES], int tope_calles){
    return (es_posicion_dentro_del_rango(posicion_siguiente) && es_calle(calles, tope_calles, posicion_siguiente));
}

/*
 * Pre condiciones: 'juego' debe apuntar a una estructura válida.
 * Post condiciones: Si Homero está parado exactamente sobre el ancla, cambia su estado a recolectado.
 */
void chequear_reaccion_ancla(juego_t *juego){
    if (!juego->homero.recolecto_ancla){
        if (son_posiciones_iguales(juego->homero.posicion, juego->ancla)){
            juego->homero.recolecto_ancla = true;
        }
    }
}

/*
 * Pre condiciones: 'juego' debe apuntar a una estructura válida.
 * Post condiciones: Recorre los objetos del mapa y suma combustible si Homero está a distancia Manhattan 1 de una estación.
 */
void chequear_reaccion_estaciones(juego_t *juego){
    int i = 0;
    while (i < juego->tope_objetos){
        if (juego->objetos[i].tipo == TIPO_ESTACION){
            if (distancia_manhattan(juego->homero.posicion, juego->objetos[i].posicion) == 1){
                juego->homero.nafta += REFUELLO_ESTACION;
                if (juego->homero.nafta > NAFTA_MAX_HOMERO){
                    juego->homero.nafta = NAFTA_MAX_HOMERO;
                }
            }
        }
        i++;
    }
}

/*
* Pre condiciones:
    -> El puntero 'personaje' debe apuntar a una dirección de memoria válida asignada para una estructura 'personaje_t'.
    -> 'pasos' y 'gasto_nafta' deben ser punteros válidos que apunten a direcciones de memoria asignadas.
* Post condiciones:
    -> Establece la cantidad de casilleros a avanzar y el combustible a consumir basándose en el estado del turbo de Homero.
    -> Si no posee movimientos con turbo activos, asigna los valores base (PASOS_NORMALES y NAFTA_GASTO_NORMAL).
    -> Si posee movimientos con turbo activos, asigna los valores potenciados (PASOS_CON_TURBO y NAFTA_GASTO_TURBO).
*/
void obtener_parametros_movimiento(personaje_t *personaje, int *pasos, int *gasto_nafta){
    *pasos = PASOS_NORMALES;
    *gasto_nafta = NAFTA_GASTO_NORMAL;
    if (personaje->movs_con_turbo > 0){
        *pasos = PASOS_CON_TURBO;
        *gasto_nafta = NAFTA_GASTO_TURBO;
    }
}

/*
 * Pre condiciones: 'personaje' debe apuntar a una estructura 'personaje_t' válida en memoria.
 * Post condiciones: Descuenta la nafta cobrada por el intento de movimiento, asegurando que no baje de 0.
 */
void cobrar_nafta_movimiento(personaje_t *personaje, int gasto_nafta){
    personaje->nafta -= gasto_nafta;
    if (personaje->nafta < 0){
        personaje->nafta = 0;
    }
}

/*
 * Pre condiciones: 'monorriel' debe apuntar a una estructura monorriel_t válida en memoria.
 * Post condiciones: Desplaza secuencialmente los vagones traseros de atrás hacia adelante en el vector.
 */
void desplazar_vagones_monorriel(monorriel_t *monorriel){
    for (int vagon = monorriel->tope_posiciones - 1; vagon > 0; vagon--){
        monorriel->posiciones[vagon] = monorriel->posiciones[vagon - 1];
    }
}

/*
 * Pre condiciones: 'monorriel' debe apuntar a una estructura monorriel_t válida en memoria.
 * Post condiciones: Incrementa la posición de la cabeza en el recorrido circular cerrado.
 */
void avanzar_cabeza_monorriel(monorriel_t *monorriel){
    monorriel->posiciones[0]++;
    if (monorriel->posiciones[0] >= monorriel->tope_recorrido){
        monorriel->posiciones[0] = 0;
    }
}

/*
 * Pre condiciones:
 *  -> 'juego' debe ser un puntero válido que apunte a una estructura 'juego_t' en memoria.
 *  -> El vector 'juego->calles' ya debe estar poblado con un 'juego->tope_calles' > 0.
 *  -> La constante 'CANT_LOMAS_BURRO' debe estar definida con un valor mayor a 0.
 * Post condiciones: Ubica aleatoriamente en el vector de objetos la cantidad de lomas de burro
 * estipuladas por la constante, asegurando que se posicionen únicamente sobre calles válidas
 * que se encuentren completamente libres de otros obstáculos, incrementando 'juego->tope_objetos' por cada una.
 */
void inicializar_lomas_burro(juego_t *juego){
    int colocadas = 0;
    while (colocadas < CANT_LOMAS_BURRO){
        int indice_calle = rand() % juego->tope_calles;
        coordenada_t pos_aleatoria = juego->calles[indice_calle];

        if (!posicion_ocupada(pos_aleatoria, juego)){
            juego->objetos[juego->tope_objetos].tipo = TIPO_LOMA_BURRO;
            juego->objetos[juego->tope_objetos].posicion = pos_aleatoria;
            juego->tope_objetos++;
            colocadas++;
        }
    }
}

/*
 * Pre condiciones:
 *  -> 'juego' debe ser un puntero válido que apunte a una estructura 'juego_t' en memoria.
 *  -> El vector 'juego->calles' ya debe estar poblado con un 'juego->tope_calles' > 0.
 *  -> La constante 'CANT_ESTACIONES_SERVICIO' debe estar definida con un valor mayor a 0.
 * Post condiciones: Ubica aleatoriamente en el vector de objetos la cantidad de estaciones de servicio
 * estipuladas por la constante, garantizando que se sitúen en casilleros de vereda vacíos pero que
 * sean adyacentes (vecinos a distancia Manhattan igual a 1) a una calle, incrementando 'juego->tope_objetos' por cada una.
 */
void inicializar_estaciones_servicios(juego_t *juego){
    int colocadas = 0;
    while (colocadas < CANT_ESTACIONES_SERVICIO){
        coordenada_t pos_aleatoria;
        pos_aleatoria.fil = rand() % MAX_FILAS;
        pos_aleatoria.col = rand() % MAX_COLUMNAS;

        if (es_calle_vecina(pos_aleatoria, juego->calles, juego->tope_calles) && !posicion_ocupada(pos_aleatoria, juego)){
            juego->objetos[juego->tope_objetos].tipo = TIPO_ESTACION;
            juego->objetos[juego->tope_objetos].posicion = pos_aleatoria;
            juego->tope_objetos++;
            colocadas++;
        }
    }
}

/*
 * Pre condiciones:
 *  -> 'juego' debe ser un puntero válido que apunte a una estructura 'juego_t' en memoria.
 *  -> El vector 'juego->calles' ya debe estar poblado con un 'juego->tope_calles' > 0.
 *  -> La constante 'CANT_SEMAFOROS' debe estar definida con un valor mayor a 0.
 * Post condiciones: Ubica aleatoriamente en el vector de objetos la cantidad de semáforos
 * estipuladas por la constante, garantizando que se sitúen en casilleros de vereda vacíos pero que
 * sean adyacentes (vecinos a distancia Manhattan igual a 1) a una calle, incrementando 'juego->tope_objetos' por cada uno.
 */
void inicializar_semaforos(juego_t *juego){
    int colocadas = 0;
    while (colocadas < CANT_SEMAFOROS){
        coordenada_t pos_aleatoria;
        pos_aleatoria.fil = rand() % MAX_FILAS;
        pos_aleatoria.col = rand() % MAX_COLUMNAS;

        if (es_calle_vecina(pos_aleatoria, juego->calles, juego->tope_calles) && !posicion_ocupada(pos_aleatoria, juego)){
            juego->objetos[juego->tope_objetos].tipo = TIPO_SEMAFORO;
            juego->objetos[juego->tope_objetos].posicion = pos_aleatoria;
            juego->tope_objetos++;
            colocadas++;
        }
    }
}

/*
 * Pre condiciones:
 *  -> 'juego' debe ser un puntero válido que apunte a una estructura 'juego_t' en memoria.
 *  -> El vector 'juego->calles' ya debe estar poblado con un 'juego->tope_calles' > 0.
 * Post condiciones: Determina de forma aleatoria una coordenada de calle que se encuentre
 * completamente vacía y libre de otros obstáculos del escenario para asignar allí la posición
 * física final de la estructura de la 'juego->ancla'.
 */
void inicializar_ancla_nivel(juego_t *juego){
    bool ancla_colocada = false;
    while (!ancla_colocada){
        int indice_calle = rand() % juego->tope_calles;
        coordenada_t pos_candidata = juego->calles[indice_calle];
        if (!posicion_ocupada(pos_candidata, juego)){
            juego->ancla = pos_candidata;
            ancla_colocada = true;
        }
    }
}

/*
 * Pre condiciones: 'juego' debe apuntar a una estructura válida con sus calles y personaje ya cargados.
 * Post condiciones: Ubica de forma modular y secuencial todos los elementos interactivos del escenario.
 */
void inicializar_objetos(juego_t *juego){
    if (juego != PUNTERO_VALIDO){
        juego->tope_objetos = 0;
        inicializar_estaciones_servicios(juego);
        inicializar_semaforos(juego);
        inicializar_lomas_burro(juego);
        inicializar_ancla_nivel(juego);
    }
}

/*
 * Pre condiciones: 'juego' debe apuntar a una estructura válida de tipo
 * juego_t. Post condiciones: Inicializa por completo el estado global de la
 * partida, cargando las calles de la cátedra, a Homero, los vehículos, el
 * monorriel y los objetos aleatorios.
 */
void inicializar_juego(juego_t *juego){
    if (juego != PUNTERO_VALIDO){
        juego->movs_realizados = 0;
        obtener_calles(juego->calles, &(juego->tope_calles));
        inicializar_personajes(&(juego->homero));
        inicializar_objetos_herramientas(&(juego->homero));
        inicializar_autos_juego(juego);
        inicializar_monorriel(&(juego->monorriel));
        inicializar_objetos(juego);
    }
}

/*
 * Pre condiciones: 'juego' debe apuntar a una estructura válida.
 * Post condiciones: Si Homero está sobre el ancla y no la recolectó, cambia su estado a verdadero.
 */
void verificar_recoleccion_ancla(juego_t *juego){
    if (!juego->homero.recolecto_ancla){
        if (son_posiciones_iguales(juego->homero.posicion, juego->ancla)){
            juego->homero.recolecto_ancla = true;
        }
    }
}

/*
 * Pre condiciones: 'juego' debe apuntar a una estructura válida.
 * Post condiciones: Recorre los objetos para verificar si Homero está adyacente a una estación de servicio y recarga combustible.
 */
void verificar_recarga_estaciones(juego_t *juego){
    int i = 0;
    while (i < juego->tope_objetos){
        if (juego->objetos[i].tipo == TIPO_ESTACION){
            if (distancia_manhattan(juego->homero.posicion, juego->objetos[i].posicion) == 1){
                juego->homero.nafta += REFUELLO_ESTACION;
                if (juego->homero.nafta > NAFTA_MAX_HOMERO){
                    juego->homero.nafta = NAFTA_MAX_HOMERO;
                }
            }
        }
        i++;
    }
}

/*
 * Pre condiciones: 'juego' debe apuntar a una estructura 'juego_t' válida en memoria.
 * Post condiciones: Recorre el vector de vehículos y asigna 5 movimientos de frenado a todos los autos en circulación.
 */
void aplicar_policia_trafico(juego_t *juego){
    int i = 0;
    while (i < juego->tope_vehiculos){
        juego->vehiculos[i].movimientos_frenado = DURACION_POLICIA_FRENADO;
        i++;
    }
}

/*
 * Pre condiciones: 'juego' debe apuntar a una estructura 'juego_t' válida en memoria.
 * Post condiciones: Evalúa y remolca de forma estructurada en reversa los autos sin nafta alineados con Homero mediante una calle limpia.
 */
void aplicar_grua_remolque(juego_t *juego){
    coordenada_t pos_h = juego->homero.posicion;
    int i = juego->tope_vehiculos - 1; 
    while (i >= 0){
        int idx = juego->vehiculos[i].posicion;
        coordenada_t pos_a = juego->vehiculos[i].recorrido[idx];
        if (juego->vehiculos[i].nafta <= 0 &&
            (pos_h.fil == pos_a.fil || pos_h.col == pos_a.col) &&
            calle_es_continua(pos_h, pos_a, juego->calles, juego->tope_calles)){
            eliminar_vehiculo(juego->vehiculos, &(juego->tope_vehiculos), i);
        }
        i--;
    }
}

/*
 * Pre condiciones: 'juego' debe apuntar a una estructura válida. 'accion' debe ser una habilidad (G, P, T).
 * Post condiciones: Ejecuta de forma modular el efecto secundario de la habilidad elegida descontando un uso disponible.
 */
void ejecutar_habilidad(juego_t *juego, char accion){
    if (juego != PUNTERO_VALIDO){
        if (accion == 'T' && juego->homero.turbos_disponibles > 0){
            juego->homero.movs_con_turbo = DURACION_EFECTO_TURBO;
            juego->homero.turbos_disponibles--;
        }else if (accion == 'P' && juego->homero.policias_disponibles > 0){
            aplicar_policia_trafico(juego);
            juego->homero.policias_disponibles--;
        }else if (accion == 'G' && juego->homero.gruas_disponibles > 0){
            aplicar_grua_remolque(juego);
            juego->homero.gruas_disponibles--;
        }
    }
}


/*
* Pre condiciones:
    -> 'auto_actual' debe apuntar a una estructura 'vehiculo_t' válida en memoria.
    -> 'objeto' representa un elemento estático del mapa correctamente inicializado.
* Post condiciones:
    -> Evalúa la posición actual del vehículo respecto al obstáculo recibido.
    -> Si está a distancia Manhattan 1 de un semáforo, aplica DURACION_FRENADO_SEMAFORO movimientos de espera.
    -> Si pisa una loma de burro, reduce su velocidad linealmente en 1 unidad (respetando el piso de VEL_MIN_AUTO).
    -> Si está a distancia Manhattan 1 de una estación, incrementa su combustible en REFUELLO_ESTACION litros.
*/
void aplicar_interaccion_auto_objeto(vehiculo_t *auto_actual, objeto_t objeto) {
    int idx_trayecto = auto_actual->posicion;
    coordenada_t pos_auto = auto_actual->recorrido[idx_trayecto];
    if (objeto.tipo == TIPO_SEMAFORO && distancia_manhattan(pos_auto, objeto.posicion) == 1) {
        auto_actual->movimientos_frenado = DURACION_FRENADO_SEMAFORO;
    }   
    if (objeto.tipo == TIPO_LOMA_BURRO && son_posiciones_iguales(pos_auto, objeto.posicion)) {
        if (auto_actual->velocidad > VEL_MIN_AUTO) {
            auto_actual->velocidad--;
        }
    }  
    if (objeto.tipo == TIPO_ESTACION && distancia_manhattan(pos_auto, objeto.posicion) == 1) {
        auto_actual->nafta += REFUELLO_ESTACION;
    }
}

/*
* Pre condiciones:
    -> El puntero 'juego' debe apuntar a una estructura 'juego_t' correctamente inicializada en memoria.
* Post condiciones:
    -> Evalúa de forma estructurada las reacciones de la flota automotor al concluir el turno.
    -> Si un vehículo comenzó congelado, decrementa de forma aislada un movimiento de espera.
    -> Si estaba libre, delega secuencialmente el procesamiento de sus interacciones mediante 'aplicar_interaccion_auto_objeto'.
*/
void chequear_reacciones_finales(juego_t *juego) {
    if (juego != PUNTERO_VALIDO) {
        int i = 0;       
        while (i < juego->tope_vehiculos) {
            vehiculo_t *auto_actual = &(juego->vehiculos[i]);         
            if (auto_actual->movimientos_frenado > 0) {
                auto_actual->movimientos_frenado--;
            } else {
                int j = 0;
                while (j < juego->tope_objetos) {
                    aplicar_interaccion_auto_objeto(auto_actual, juego->objetos[j]);
                    j++;
                }
            }
            i++;
        }
    }
}


/*
 * Pre condiciones: 'juego' debe apuntar a una estructura válida.
 * Post condiciones: Avanza 1 casillero todos los vagones en forma de anillo cerrado.
 *                   Si la locomotora alcanza una esquina, activa el frenado por 5 turnos de forma modular.
 */
void mover_monorriel(juego_t *juego){
    if (juego != PUNTERO_VALIDO){
        if (juego->monorriel.movimientos_frenado > 0){
            juego->monorriel.movimientos_frenado--;
        }else{
            desplazar_vagones_monorriel(&(juego->monorriel));
            avanzar_cabeza_monorriel(&(juego->monorriel));
            int indice_nuevo_cabeza = juego->monorriel.posiciones[0];
            coordenada_t coord_nueva_cabeza = juego->monorriel.recorrido[indice_nuevo_cabeza];
            if (es_esquina_monorriel(coord_nueva_cabeza)){
                juego->monorriel.movimientos_frenado = 5;
            }
        }
    }
}

/*
 * Pre condiciones: 'juego' debe apuntar a una estructura válida con todos sus elementos inicializados.
 * Post condiciones: Aplica en cascada las reacciones de Homero con el ancla y las estaciones de servicio de forma estructurada.
 */
void aplicar_reacciones_homero(juego_t *juego){
    if (juego != PUNTERO_VALIDO){
        verificar_recoleccion_ancla(juego);
        verificar_recarga_estaciones(juego);
    }
}

/*
 * Pre condiciones: 'juego' debe apuntar a una estructura válida.
 * Post condiciones: Aplica las reacciones específicas en el segundo paso del turbo de Homero.
 */
void aplicar_reacciones_paso_turbo(juego_t *juego, int nafta_antes_de_moverse){
    int nafta_antes_paso2 = juego->homero.nafta;
    aplicar_reacciones_homero(juego);
    if (juego->homero.nafta > nafta_antes_paso2 && nafta_antes_de_moverse == NAFTA_MAX_HOMERO){
        juego->homero.nafta = nafta_antes_paso2;
    }
}

/*
 * Pre condiciones: 'juego' debe apuntar a una estructura válida.
 * Post condiciones: Ejecuta de forma estructurada los pasos de Homero (1 o 2). Devuelve true si pudo dar al menos un paso.
 */
bool procesar_pasos_homero(juego_t *juego, char movimiento, int pasos, int nafta_antes_de_moverse){
    bool se_movio = false;
    int i = 0;
    bool movimiento_valido = true;
    while (movimiento_valido && (i < pasos)){
        int sig_fil = juego->homero.posicion.fil;
        int sig_col = juego->homero.posicion.col;
        calcular_desplazamiento(movimiento, &sig_fil, &sig_col);
        coordenada_t paso_siguiente = {sig_fil, sig_col};
        if (verificar_paso_valido(paso_siguiente, juego->calles, juego->tope_calles)){
            juego->homero.posicion = paso_siguiente;
            se_movio = true;
            if (i == 1 && pasos == 2){
                aplicar_reacciones_paso_turbo(juego, nafta_antes_de_moverse);
            }else{
                aplicar_reacciones_homero(juego);
            }
            i++;
        }else{
            movimiento_valido = false;
        }
    }
    return se_movio;
}

/*
* Pre condiciones:
    -> 'personaje' debe apuntar a una estructura 'personaje_t' válida en memoria.
* Post condiciones:
    -> Si el personaje no logró efectuar ningún movimiento válido, restaura su posición y combustible a los valores previos al intento.
*/
void verificar_restauracion_personaje(personaje_t *personaje, bool se_movio, coordenada_t pos_inicial, int nafta_inicial) {
    if (!se_movio) {
        personaje->posicion = pos_inicial;
        personaje->nafta = nafta_inicial;
    }
}


/*
* Pre condiciones:
    -> El puntero 'juego' debe apuntar a una estructura 'juego_t' correctamente inicializada en memoria.
    -> 'movimiento' debe ser un carácter de dirección válido (COMANDO_ARRIBA, COMANDO_ABAJO, COMANDO_IZQUIERDA o COMANDO_DERECHA).
* Post condiciones:
    -> Coordina el desplazamiento de Homero casillero por casillero según el estado de su turbo.
    -> Devuelve true si el personaje logró avanzar al menos un paso por las calles del terreno.
    -> Devuelve false en caso contrario, restaurando de forma segura su estado inicial.
*/
bool mover_homero(juego_t *juego, char movimiento){
    bool se_movio_al_menos_un_paso = false;
    if (juego != PUNTERO_VALIDO){
        int pasos = 0;
        int gasto_nafta = 0;
        obtener_parametros_movimiento(&(juego->homero), &pasos, &gasto_nafta);     
        int nafta_antes_de_moverse = juego->homero.nafta;
        coordenada_t pos_inicial = juego->homero.posicion;        
        juego->homero.nafta -= gasto_nafta;
        if (juego->homero.nafta < 0){
            juego->homero.nafta = 0;
        }

        int i = 0;
        bool paso_valido = true;
        while (paso_valido && (i < pasos)){
            int sig_fil = juego->homero.posicion.fil;
            int sig_col = juego->homero.posicion.col;
            calcular_desplazamiento(movimiento, &sig_fil, &sig_col);
            coordenada_t paso_siguiente = {sig_fil, sig_col};
            
            if (verificar_paso_valido(paso_siguiente, juego->calles, juego->tope_calles)){
                juego->homero.posicion = paso_siguiente;
                se_movio_al_menos_un_paso = true;
                if (i == 1 && pasos == 2){
                    int nafta_antes_paso2 = juego->homero.nafta;
                    aplicar_reacciones_homero(juego);
                    if (juego->homero.nafta > nafta_antes_paso2 && nafta_antes_de_moverse == NAFTA_MAX_HOMERO){
                        juego->homero.nafta = nafta_antes_paso2;
                    }
                } else {
                    aplicar_reacciones_homero(juego);
                }
                i++;
            } else {
                paso_valido = false;
            }
        }
        verificar_restauracion_personaje(&(juego->homero), se_movio_al_menos_un_paso, pos_inicial, nafta_antes_de_moverse);
    }
    return se_movio_al_menos_un_paso;
}


/*
 * Pre condiciones: 'juego' debe apuntar a una estructura 'juego_t' válida en memoria.
 *                  'movimiento' debe ser un carácter de dirección válido (W, A, S, D).
 * Post condiciones: Ejecuta de forma secuencial la simulación del turno: mueve a Homero,
 *                   y si tuvo éxito, avanza el tráfico, el monorriel, gestiona el turbo,
 *                   genera autos nuevos cada 5 movimientos y procesa las reacciones en el mapa.
 */
void mover_entidades(juego_t *juego, char movimiento){
    if (juego != PUNTERO_VALIDO){
        bool exito_movimiento = mover_homero(juego, movimiento);
        if (exito_movimiento){
            actualizar_y_mover_autos(juego);
            mover_monorriel(juego);
            juego->movs_realizados++;
            if (juego->homero.movs_con_turbo > 0){
                juego->homero.movs_con_turbo--;
            }
            if (juego->movs_realizados % FRECUENCIA_NUEVO_AUTO == 0){
                inicializar_auto_individual(&(juego->vehiculos[juego->tope_vehiculos]));
                juego->tope_vehiculos++;
            }
            chequear_reacciones_finales(juego);
        }
    }
}


 /*
* Pre condiciones:
    -> El puntero 'juego' debe apuntar a una estructura 'juego_t' correctamente inicializada en memoria.
* Post condiciones:
    -> Si el carácter ingresado se encuentra en el rango de minúsculas (entre MINUSCULA_A y MINUSCULA_Z), lo transforma a su correspondiente letra mayúscula.
    -> Redirige el comando hacia la rama de habilidades (COMANDO_GRUA, COMANDO_POLICIA o COMANDO_TURBO) o 
      hacia la simulación de movimiento del terreno utilizando los comandos de dirección válidos.
*/
void realizar_jugada(juego_t *juego, char movimiento){
    if (juego != PUNTERO_VALIDO){
        if (movimiento >= MINUSCULA_A && movimiento <= MINUSCULA_Z) {
            movimiento = (char)(movimiento - MINUSCULA_A + MAYUSCULA_A);
        }
        if (movimiento == COMANDO_GRUA || movimiento == COMANDO_POLICIA || movimiento == COMANDO_TURBO) {
            ejecutar_habilidad(juego, movimiento);
        }
        else if (movimiento == MOVER_ARRIBA || movimiento == MOVER_ABAJO || movimiento == MOVER_IZQUIERDA || movimiento == MOVER_DERECHA) {
            mover_entidades(juego, movimiento);
        }
    }
}


/*
 * Pre condiciones: 'matriz' debe estar declarada previamente. 'terreno' representa el carácter de fondo por defecto.
 * Post condiciones: Inicializa la grilla bidimensional de Springfield con el carácter base indicado.
 */
void inicializar_matriz_pantalla(char matriz[MAX_FILAS][MAX_COLUMNAS], char terreno){
    int fila = 0;
    while (fila < MAX_FILAS){
        int columna = 0;
        while (columna < MAX_COLUMNAS){
            matriz[fila][columna] = terreno;
            columna++;
        }
        fila++;
    }
}

/*
 * Pre condiciones: Recibe una estructura de tipo juego_t correctamente inicializada.
 * Post condiciones: Construye la grilla en memoria ordenando por capas de prioridad e imprime el HUD en pantalla de forma modular.
 */
void mostrar_juego(juego_t juego){
    char matriz_pantalla[MAX_FILAS][MAX_COLUMNAS];
    inicializar_matriz_pantalla(matriz_pantalla, 'X');
    int i = 0;
    while (i < juego.tope_calles){
        matriz_pantalla[juego.calles[i].fil][juego.calles[i].col] = ' ';
        i++;
    }
    i = 0;
    while (i < juego.tope_objetos){
        matriz_pantalla[juego.objetos[i].posicion.fil][juego.objetos[i].posicion.col] = juego.objetos[i].tipo;
        i++;
    }
    if (!juego.homero.recolecto_ancla){
        matriz_pantalla[juego.ancla.fil][juego.ancla.col] = TIPO_ANCLA;
    }
    i = 0;
    while (i < juego.monorriel.tope_posiciones){
        int idx = juego.monorriel.posiciones[i];
        matriz_pantalla[juego.monorriel.recorrido[idx].fil][juego.monorriel.recorrido[idx].col] = 'M';
        i++;
    }
    i = 0;
    while (i < juego.tope_vehiculos){
        int idx = juego.vehiculos[i].posicion;
        matriz_pantalla[juego.vehiculos[i].recorrido[idx].fil][juego.vehiculos[i].recorrido[idx].col] = 'A';
        i++;
    }
    matriz_pantalla[juego.homero.posicion.fil][juego.homero.posicion.col] = 'H';
    int fila = 0;
    while (fila < MAX_FILAS){
        int columna = 0;
        while (columna < MAX_COLUMNAS){
            printf("%c ", matriz_pantalla[fila][columna]);
            columna++;
        }
        printf("\n");
        fila++;
    }
    printf("\n=========================================\n");
    printf(" Nafta disponible: %d L | Movimientos: %d\n", juego.homero.nafta, juego.movs_realizados);
    printf(" ¿Tiene el ancla?: %s\n", juego.homero.recolecto_ancla ? "SÍ" : "NO");
    printf(" Grúas: %d | Policías: %d | Turbos: %d\n",
           juego.homero.gruas_disponibles, juego.homero.policias_disponibles, juego.homero.turbos_disponibles);
    printf("=========================================\n");
}

/*
 * Pre condiciones: Recibe una estructura de tipo juego_t válida.
 * Post condiciones: Devuelve 1 si Homero ganó, -1 si perdió (choques, falta de nafta o victoria del monorriel) o 0 si sigue en curso de forma estructurada pura.
 */
int estado_juego(juego_t juego){
    int resultado_partida = JUEGO_SIGUE;
    coordenada_t pos_h = juego.homero.posicion;
    coordenada_t meta_estacion = {1, 1};
    if (juego.homero.recolecto_ancla && son_posiciones_iguales(pos_h, meta_estacion)){
        resultado_partida = JUEGO_GANADO;
    }
    if (resultado_partida == JUEGO_SIGUE){
        int idx_cabeza = juego.monorriel.posiciones[0];
        coordenada_t pos_cabeza_m = juego.monorriel.recorrido[idx_cabeza];
        if (son_posiciones_iguales(pos_cabeza_m, meta_estacion) && juego.movs_realizados > 0){
            resultado_partida = JUEGO_PERDIDO;
        }
        if (juego.homero.nafta <= 0){
            resultado_partida = JUEGO_PERDIDO;
        }
        int i = 0;
        while (resultado_partida == JUEGO_SIGUE && (i < juego.monorriel.tope_posiciones)){
            int idx_vagon = juego.monorriel.posiciones[i];
            coordenada_t pos_vagon = juego.monorriel.recorrido[idx_vagon];
            if (son_posiciones_iguales(pos_h, pos_vagon)){
                resultado_partida = JUEGO_PERDIDO;
            }else{
                i++;
            }
        }
        int j = 0;
        while (resultado_partida == JUEGO_SIGUE && (j < juego.tope_vehiculos)){
            int idx_auto = juego.vehiculos[j].posicion;
            coordenada_t pos_auto = juego.vehiculos[j].recorrido[idx_auto];
            if (son_posiciones_iguales(pos_h, pos_auto)){
                resultado_partida = JUEGO_PERDIDO;
            }else{
                j++;
            }
        }
    }
    return resultado_partida;
}
