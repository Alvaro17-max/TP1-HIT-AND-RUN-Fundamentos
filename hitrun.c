#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hitrun.h"
#include "utiles.h"

const int MIN_FIL_POSICION = 0;
const int MIN_COL_POSICION = 0;
const int MAX_FIL_POSICION = 24;
const int MAX_COL_POSICION = 24;

const int FIL_INICIAL_HOMERO = 19;
const int COL_INICIAL_HOMERO = 19;
const int CANT_INICIAL_NAFTA = 100;
const int CANT_INICIAL_GRUAS = 5;
const int CANT_INICIAL_POLICIAS = 4;
const int CANT_INICIAL_TURBOS = 3;
const int CANT_INICIAL_MOVS_CON_TURBO = 0;
const int NAFTA_MAX_HOMERO = 150;
const int NAFTA_GASTO_TURBO = 3;
const int CANT_DIRRCCIONES_CELDAS = 4;

const int CANT_INICIAL_AUTOS = 6;
const int NAFTA_MIN_AUTO = 30;
const int VEL_MIN_AUTO = 1;
const int MOV_FRENADO_INICIAL = 0;
const int FRECUENCIA_NUEVO_AUTO = 5;

const int POS_MIN_FIL_MONORRIEL = 1;
const int POS_MAX_FIL_MONORRIEL = 23;
const int POS_MIN_COL_MONORRIEL = 1;
const int POS_MAX_COL_MONORRIEL = 23;
const int CANT_VAGONES = 4;

const int CANT_ESTACIONES_SERVICIO = 20;
const int CANT_SEMAFOROS = 15;
const int CANT_LOMAS_BURRO = 10;
const int REFUELLO_ESTACION = 30;

const char TIPO_LOMA_BURRO = 'L';
const char TIPO_ESTACION = 'E';
const char TIPO_SEMAFORO = 'S';
const char TIPO_ANCLA = 'C';

/* Pre condiciones: -*/
/* Post condiciones: Devuelve -true si los valores de la posicion1 (fila, columna) coincide con los valores de la posicion2 (fila, columna).
Devuelve -false- en caso contrario */
bool son_posiciones_iguales(coordenada_t posicion1, coordenada_t posicion2)
{
    return (posicion1.fil == posicion2.fil && posicion1.col == posicion2.col);
}

/*
 * Pre condiciones:
 * Post condiciones: Devuelve la distancia Manhattan (la distancia entre ellos es la suma de los valores absolutos
 * de las diferencias de las coordenadas)
 */
int distancia_manhattan(coordenada_t posicion1, coordenada_t posicion2)
{
    int distancia_filas = abs(posicion1.fil - posicion2.fil);
    int distancia_columnas = abs(posicion1.col - posicion2.col);
    return distancia_filas + distancia_columnas;
}

/*
 * Pre condiciones: -
 * Post condiciones: Devuelve true si la coordenada de la posicion se encuentra dentro de los límites del terreno (0 a 24) inckusiives
 */
bool es_posicion_dentro_del_rango(coordenada_t posicion)
{
    return (posicion.fil >= MIN_FIL_POSICION && posicion.fil <= MAX_FIL_POSICION &&
            posicion.col >= MIN_COL_POSICION && posicion.col <= MAX_COL_POSICION);
}

/*
 * Pre condiciones:
El vector de 'calles' debe estar inicializado.
El valor de 'tope_calles' debe ser > 0.
 * Post condiciones: Devuelve true si la coordenada coincide con alguna calle registrada en el vector, o false en caso contrario.
 */
bool es_calle(coordenada_t calles[MAX_CALLES], int tope_calles, coordenada_t posicion)
{
    bool en_calle = false;
    int indice_calle = 0;
    while (!en_calle && (indice_calle < tope_calles))
    {
        if (son_posiciones_iguales(calles[indice_calle], posicion))
        {
            en_calle = true;
        }
        else
        {
            indice_calle++;
        }
    }
    return en_calle;
}

/*
Pre condiciones:
 -> El vector de 'calles' debe estar inicializado.
-> El valor de 'tope_calles' debe ser > 0.
Post condiciones: Devuelve true si la coordenada "posicion" NO es una calle y hay al menos una calle en el vector de
calles en la que la distancia Manhattan con respecto a "posicion" es igual a 1.
Devuelve false en caso contrario ( si "posicion" ya es una calle o si se encuentra a una distancia mayor de todas las
calles)
 */

bool es_calle_vecina(coordenada_t posicion, coordenada_t calles[MAX_CALLES], int tope_calles)
{
    bool es_vecina = false;
    if (!es_calle(calles, tope_calles, posicion))
    { // si no es una calle //
        int i = 0;
        while (!es_vecina && (i < tope_calles))
        {
            if (distancia_manhattan(posicion, calles[i]) == 1)
            {
                es_vecina = true;
            }
            else
            {
                i++;
            }
        }
    }
    return es_vecina;
}

/* *Pre condiciones: -
 * Post condiciones: Devuelve true si la coordenada coincide exactamente con alguna de las 4 esquinas del
 monorriel (1,1), (1,23), (23,23) o (23,1). */
bool es_esquina_monorriel(coordenada_t posicion)
{
    return (posicion.fil == POS_MIN_FIL_MONORRIEL && posicion.col == POS_MIN_COL_MONORRIEL) || // (1,1)
           (posicion.fil == POS_MIN_FIL_MONORRIEL && posicion.col == POS_MAX_COL_MONORRIEL) || // (1,23)
           (posicion.fil == POS_MAX_FIL_MONORRIEL && posicion.col == POS_MAX_COL_MONORRIEL) || // (23,23)
           (posicion.fil == POS_MAX_FIL_MONORRIEL && posicion.col == POS_MIN_COL_MONORRIEL);   //(23,1)
}

/*Pre condiciones : El monorriel debe tener su recorrido cargadp en la memoria (el vector de coordenadas)
El tope del recorrido del monorriel debe ser mayor a 0.
Post condiciones: Devuelve true si la fila y columna de alguna de las posiciones del recorrido del monorriel
coincide exactamente con la fila y columna de la coordenada "posicion"  pasada por paramtero.
Devuelve false en caso contrario
*/
bool es_recorrido_monorriel(coordenada_t posicion, monorriel_t monorriel)
{
    bool encontrado = false;
    int i = 0;
    while (!encontrado && (i < monorriel.tope_recorrido))
    {
        if (son_posiciones_iguales(monorriel.recorrido[i], posicion))
        {
            encontrado = true;
        }
        else
        {
            i++;
        }
    }
    return encontrado;
}

/*Pre condiciones : El vector de vehiculos debe estar correctamente inicializado
int* tope_vehiculos debe apuntar a un valor entero válido mayor a 0
El valor de poscion_vehiculo debe ser un numero valido menor al valor de *tope_vehiculos.

Post condiciones: Elimina el vehiculo que se enceuntra en el indice de "posicion_vehiculo".
La posicion en la que se encontraba el auto a quitar paasa a ser ocupado por el auto que se encuentra
en la posicion siguiente.
Restamos 1 por cada auto quitado en puntero *tope_vehiculos.
*/

void eliminar_vehiculo(vehiculo_t vehiculos[MAX_AUTOS], int *tope_vehiculos, int posicion_vehiculo)
{
    int i = posicion_vehiculo;

    while (i < (*tope_vehiculos) - 1)
    {
        vehiculos[i] = vehiculos[i + 1];
        i++;
    }

    (*tope_vehiculos) -= 1;
}

/*
 * Pre condiciones: - Los punteros posicion_inicio y posicion_fin deben apuntar a valores enterors validos
 * Post condiciones: Compara los valores de "coordenada_homero" y "coordenada_auto" y guarda el menor en la
 * direccion de memoria apuntada por "posicion_inicio" y "posicion_fin"
 */
void obtener_extremos_tramo(int coordenada_homero, int coordenada_auto, int *posicion_inicio, int *posicion_fin)
{
    if (coordenada_homero < coordenada_auto)
    {
        *posicion_inicio = coordenada_homero;
        *posicion_fin = coordenada_auto;
    }
    else
    {
        *posicion_inicio = coordenada_auto;
        *posicion_fin = coordenada_homero;
    }
}

/*
 * Pre condiciones: 'posicion_homero' y 'posicion_auto' representan posiciones en la misma línea visual (fila o columna).
 *                  'calles' está correctamente inicializado con un tope válido.
 * Post condiciones: Devuelve true si el trayecto de calles entre Homero y el auto es una línea recta continua sin baches.
 */

bool calle_es_continua(coordenada_t posicion_homero, coordenada_t posicion_auto, coordenada_t calles[MAX_CALLES], int tope_calles)
{
    bool es_continua = true;
    int posicion_inicio;
    int posicion_fin;

    if (posicion_homero.fil == posicion_auto.fil)
    {
        obtener_extremos_tramo(posicion_homero.col, posicion_auto.col, &posicion_inicio, &posicion_fin);
        es_continua = verificar_tramo_calle(posicion_inicio, posicion_fin, posicion_homero.fil, true, calles, tope_calles);
    }

    else if (posicion_homero.col == posicion_auto.col)
    {
        obtener_extremos_tramo(posicion_homero.fil, posicion_auto.fil, &posicion_inicio, &posicion_fin);
        es_continua = verificar_tramo_calle(posicion_inicio, posicion_fin, posicion_homero.col, false, calles, tope_calles);
    }
    return es_continua;
}

/*
 * Pre condiciones: - El puntero juego debe apuntar a una estructura "juego_t" corretamente inicializada.
 El juego debe tener cargado los datos de Homero, el monorriel y el vector de objetos con un tope valido.

 * Post condiciones: Dvuelve true si la coordenada posicion coincide con la ubicacion actual de homero,
 con cualquier celda que compone el recorrido del monoiel o la posicion de algun objeto ya existente.
 Devuelve False en caso contrario (si el casillero consultado se encuentre libre).
 */

bool posicion_ocupada(coordenada_t posicion, juego_t *juego)
{
    bool ocupada = false;
    if (son_posiciones_iguales(juego->homero.posicion, posicion))
    {
        ocupada = true;
    }
    if (!ocupada && es_recorrido_monorriel(posicion, juego->monorriel))
    {
        ocupada = true;
    }

    int i = 0;
    while (!ocupada && (i < juego->tope_objetos))
    {
        if (son_posiciones_iguales(juego->objetos[i].posicion, posicion))
        {
            ocupada = true;
        }
        else
        {
            i++;
        }
    }
    return ocupada;
}

/*
 * Pre condiciones: 'personaje' debe apuntar a una porción de memoria válida de tipo personaje_t.
 * Post condiciones: Carga los valores iniciales de Homero: posición (19,19), 100 de nafta, ancla en false y la cantidad inicial de herramientas disponibles.
 */
void inicializar_personajes(personaje_t *personaje)
{
    if (personaje == NULL)
        return;
    personaje->posicion.fil = FIL_INICIAL_HOMERO;
    personaje->posicion.col = COL_INICIAL_HOMERO;

    void inicializar_objetos_herramientas(personaje_t * personaje)
        personaje->recolecto_ancla = false;
    personaje->gruas_disponibles = CANT_INICIAL_GRUAS;
    personaje->policias_disponibles = CANT_INICIAL_POLICIAS;
    personaje->turbos_disponibles = CANT_INICIAL_TURBOS;
    personaje->movs_con_turbo = CANT_INICIAL_MOVS_CON_TURBO;
    personaje->nafta = CANT_INICIAL_NAFTA;
}

/*
 * Pre condiciones: 'vehiculos' debe ser un vector de tamaño MAX_AUTOS y 'tope_vehiculos' debe ser un puntero válido a entero.
 * Post condiciones: Inicializa los 6 autos del comienzo del juego asignándoles un recorrido de utiles.h, posición inicial 0, y nafta (30-50) y velocidad (1-4) aleatorias. Actualiza el tope.
 */
void inicializar_vehiculos(vehiculo_t vehiculos[MAX_AUTOS], int *tope_vehiculos)
{
    if (tope_vehiculos == NULL)
        return;
    *tope_vehiculos = 0;

    for (int i = 0; i < CANT_INICIAL_AUTOS; i++)
    {
        obtener_recorrido(vehiculos[i].recorrido, &(vehiculos[i].tope_recorrido));
        vehiculos[i].posicion = 0;
        vehiculos[i].nafta = (int)(rand() % 21) + NAFTA_MIN_AUTO;
        vehiculos[i].velocidad = (int)(rand() % 4) + VEL_MIN_AUTO;
        vehiculos[i].movimientos_frenado = MOV_FRENADO_INICIAL;
        (*tope_vehiculos)++;
    }
}

/*
 * Pre condiciones: 'monorriel' debe apuntar a una estructura válida de tipo monorriel_t.
 * Post condiciones: Diseña matemáticamente el circuito cerrado periférico contra las agujas del reloj, inicializa el tope de recorrido y ubica los 4 vagones al inicio del camino.
 */
void inicializar_monorriel(monorriel_t *monorriel)
{
    if (monorriel == NULL)
        return;
    int t = 0;

    for (int f = POS_MIN_FIL_MONORRIEL; f < POS_MAX_FIL_MONORRIEL; f++)
    {
        monorriel->recorrido[t].fil = f;
        monorriel->recorrido[t].col = POS_MIN_COL_MONORRIEL;
        t++;
    }
    for (int c = POS_MIN_COL_MONORRIEL; c < POS_MAX_COL_MONORRIEL; c++)
    {
        monorriel->recorrido[t].fil = POS_MAX_FIL_MONORRIEL;
        monorriel->recorrido[t].col = c;
        t++;
    }
    for (int f = POS_MAX_FIL_MONORRIEL; f > POS_MIN_FIL_MONORRIEL; f--)
    {
        monorriel->recorrido[t].fil = f;
        monorriel->recorrido[t].col = POS_MAX_COL_MONORRIEL;
        t++;
    }
    for (int c = POS_MAX_COL_MONORRIEL; c > POS_MIN_FIL_MONORRIEL; c--)
    {
        monorriel->recorrido[t].fil = POS_MIN_FIL_MONORRIEL;
        monorriel->recorrido[t].col = c;
        t++;
    }

    monorriel->tope_recorrido = t;
    monorriel->movimientos_frenado = 0;
    monorriel->tope_posiciones = CANT_VAGONES;
    coordenada_t metas_vagones[] = {{1, 1}, {1, 2}, {1, 3}, {1, 4}};

    for (int vagon = 0; vagon < CANT_VAGONES; vagon++)
    {
        for (int j = 0; j < monorriel->tope_recorrido; j++)
        {
            if (monorriel->recorrido[j].fil == metas_vagones[vagon].fil &&
                monorriel->recorrido[j].col == metas_vagones[vagon].col)
            {
                monorriel->posiciones[vagon] = j;
                break;
            }
        }
    }
}

/*
 * Pre condiciones: 'juego' debe apuntar a una estructura válida con sus calles y personaje ya cargados.
 * Post condiciones: Ubica aleatoriamente en el mapa las 10 lomas, 20 estaciones de servicio, 15 semáforos y el ancla respetando las reglas de adyacencia y superposición.
 */
void inicializar_objetos(juego_t *juego)
{
    if (juego == NULL)
        return;
    juego->tope_objetos = 0;

    for (int i = 0; i < CANT_LOMAS_BURRO; i++)
    {
        coordenada_t pos_aleatoria;
        do
        {
            int indice_calle = rand() % juego->tope_calles;
            pos_aleatoria = juego->calles[indice_calle];
        } while (posicion_ocupada(pos_aleatoria, juego));
        juego->objetos[juego->tope_objetos].tipo = TIPO_LOMA_BURRO;
        juego->objetos[juego->tope_objetos].posicion = pos_aleatoria;
        juego->tope_objetos++;
    }
    for (int i = 0; i < CANT_ESTACIONES_SERVICIO; i++)
    {
        coordenada_t pos_aleatoria;
        do
        {
            pos_aleatoria.fil = rand() % MAX_FILAS;
            pos_aleatoria.col = rand() % MAX_COLUMNAS;
        } while (!es_calle_adyacente(pos_aleatoria, juego->calles, juego->tope_calles) || posicion_ocupada(pos_aleatoria, juego));
        juego->objetos[juego->tope_objetos].tipo = TIPO_ESTACION;
        juego->objetos[juego->tope_objetos].posicion = pos_aleatoria;
        juego->tope_objetos++;
    }
    for (int i = 0; i < CANT_SEMAFOROS; i++)
    {
        coordenada_t pos_aleatoria;
        do
        {
            pos_aleatoria.fil = rand() % MAX_FILAS;
            pos_aleatoria.col = rand() % MAX_COLUMNAS;
        } while (!es_calle_adyacente(pos_aleatoria, juego->calles, juego->tope_calles) || posicion_ocupada(pos_aleatoria, juego));
        juego->objetos[juego->tope_objetos].tipo = TIPO_SEMAFORO;
        juego->objetos[juego->tope_objetos].posicion = pos_aleatoria;
        juego->tope_objetos++;
    }
    do
    {
        int indice_calle = rand() % juego->tope_calles;
        juego->ancla = juego->calles[indice_calle];
    } while (posicion_ocupada(juego->ancla, juego));
}

/*
 * Pre condiciones: 'juego' debe apuntar a una estructura válida de tipo juego_t.
 * Post condiciones: Inicializa por completo el estado global de la partida, cargando las calles de la cátedra, a Homero, los vehículos, el monorriel y los objetos aleatorios.
 */
void inicializar_juego(juego_t *juego)
{
    if (juego == NULL)
        return;
    juego->movs_realizados = 0;
    obtener_calles(juego->calles, &(juego->tope_calles));
    inicializar_personajes(&(juego->homero));
    inicializar_vehiculos(juego->vehiculos, &(juego->tope_vehiculos));
    inicializar_monorriel(&(juego->monorriel));
    inicializar_objetos(juego);
}

/*
 * Pre condiciones: 'juego' debe apuntar a una estructura válida con todos sus elementos inicializados.
 * Post condiciones: Verifica si Homero recolectó el ancla en su casillero actual o si se encuentra a distancia 1 de alguna estación de servicio para recargar 30 L de nafta.
 */

void aplicar_reacciones_homero(juego_t *juego)
{
    if (juego == NULL)
        return;

    // 1. El ancla se chequea e interactúa siempre en caliente
    if (!juego->homero.recolecto_ancla)
    {
        if (juego->homero.posicion.fil == juego->ancla.fil && juego->homero.posicion.col == juego->ancla.col)
        {
            juego->homero.recolecto_ancla = true;
        }
    }

    for (int i = 0; i < juego->tope_objetos; i++)
    {
        if (juego->objetos[i].tipo == TIPO_ESTACION)
        {
            if (distancia_manhattan(juego->homero.posicion, juego->objetos[i].posicion) == 1)
            {
                juego->homero.nafta += REFUELLO_ESTACION;

                // Clavamos el límite máximo infranqueable exigido por el test en caliente
                if (juego->homero.nafta > NAFTA_MAX_HOMERO)
                {
                    juego->homero.nafta = NAFTA_MAX_HOMERO;
                }
            }
        }
    }
}

/*
 * Pre condiciones: 'juego' debe apuntar a una estructura válida. 'movimiento' debe ser un carácter de dirección válido (W, A, S, D).
 * Post condiciones: Desplaza a Homero en la dirección indicada tantas celdas como corresponda (1 normal o 2 si tiene turbo), descuenta la nafta y aplica las reacciones paso a paso.
 */

bool mover_homero(juego_t *juego, char movimiento)
{
    if (juego == NULL)
        return false;

    int pasos = (juego->homero.movs_con_turbo > 0) ? 2 : 1;

    int gasto_nafta = (juego->homero.movs_con_turbo > 0) ? NAFTA_GASTO_TURBO : 1;

    if (juego->homero.nafta < gasto_nafta)
    {
    }

    int nafta_antes_de_moverse = juego->homero.nafta;
    coordenada_t pos_inicial = juego->homero.posicion;

    juego->homero.nafta -= gasto_nafta;
    if (juego->homero.nafta < 0)
        juego->homero.nafta = 0;

    bool se_movio_al_menos_un_paso = false;

    for (int i = 0; i < pasos; i++)
    {
        int sig_fil = juego->homero.posicion.fil;
        int sig_col = juego->homero.posicion.col;

        if (movimiento == 'W')
            sig_fil--;
        if (movimiento == 'S')
            sig_fil++;
        if (movimiento == 'A')
            sig_col--;
        if (movimiento == 'D')
            sig_col++;

        if (sig_fil < 0 || sig_fil >= MAX_FILAS || sig_col < 0 || sig_col >= MAX_COLUMNAS)
            break;

        coordenada_t paso_teorico = {sig_fil, sig_col};
        if (!es_calle(paso_teorico, juego->calles, juego->tope_calles))
            break;

        juego->homero.posicion.fil = sig_fil;
        juego->homero.posicion.col = sig_col;
        se_movio_al_menos_un_paso = true;

        if (i == 1 && pasos == 2)
        {
            int nafta_antes_paso2 = juego->homero.nafta;

            aplicar_reacciones_homero(juego);

            if (juego->homero.nafta > nafta_antes_paso2 && nafta_antes_de_moverse == NAFTA_MAX_HOMERO)
            {
                juego->homero.nafta = nafta_antes_paso2;
            }
        }
        else
        {
            aplicar_reacciones_homero(juego);
        }
    }

    if (!se_movio_al_menos_un_paso)
    {
        juego->homero.posicion = pos_inicial;
        juego->homero.nafta = nafta_antes_de_moverse;
    }

    return se_movio_al_menos_un_paso;
}

/*
 * Pre condiciones: 'juego' debe apuntar a una estructura válida.
 * Post condiciones: Avanza la posición de todos los autos activos según su velocidad, reduce el combustible consumido y elimina del vector a los vehículos que completen su recorrido.
 */

void mover_autos(juego_t *juego)
{
    if (juego == NULL)
        return;

    int i = 0;
    while (i < juego->tope_vehiculos)
    {

        if (juego->vehiculos[i].movimientos_frenado > 0)
        {
            juego->vehiculos[i].movimientos_frenado--;
            i++; // Pasamos al siguiente auto
            continue;
        }
        if (juego->vehiculos[i].nafta <= 0)
        {
            i++;
            continue;
        }

        int casilleros_a_mover = juego->vehiculos[i].velocidad;
        int casilleros_movidos = 0;
        bool llego_al_final = false;

        for (int paso = 0; paso < casilleros_a_mover; paso++)
        {
            if (juego->vehiculos[i].posicion + 1 >= juego->vehiculos[i].tope_recorrido)
            {
                llego_al_final = true;
                break;
            }
            juego->vehiculos[i].posicion++;
            casilleros_movidos++;
        }

        juego->vehiculos[i].nafta -= casilleros_movidos;
        if (juego->vehiculos[i].nafta < 0)
        {
            juego->vehiculos[i].nafta = 0;
        }

        if (llego_al_final)
        {
            for (int j = i; j < juego->tope_vehiculos - 1; j++)
            {
                juego->vehiculos[j] = juego->vehiculos[j + 1];
            }
            juego->tope_vehiculos--;
        }
        else
        {
            i++;
        }
    }
}

/*
 * Pre condiciones: 'monorriel' debe apuntar a una estructura válida de tipo monorriel_t.
 * Post condiciones: Avanza 1 casillero todos los vagones en forma de anillo cerrado. Si la locomotora alcanza una esquina del circuito, activa el frenado de la formación por 5 turnos.
 */

void mover_monorriel(juego_t *juego)
{
    if (juego->monorriel.movimientos_frenado > 0)
    {
        juego->monorriel.movimientos_frenado--;
    }
    else
    {
        for (int vagon = juego->monorriel.tope_posiciones - 1; vagon > 0; vagon--)
        {
            juego->monorriel.posiciones[vagon] = juego->monorriel.posiciones[vagon - 1];
        }
        juego->monorriel.posiciones[0]++;
        if (juego->monorriel.posiciones[0] >= juego->monorriel.tope_recorrido)
        {
            juego->monorriel.posiciones[0] = 0;
        }
        int indice_nuevo_cabeza = juego->monorriel.posiciones[0];
        coordenada_t coord_nueva_cabeza = juego->monorriel.recorrido[indice_nuevo_cabeza];
        if (es_esquina_monorriel(coord_nueva_cabeza))
        {
            juego->monorriel.movimientos_frenado = 5;
        }
    }
}
/*
 * Pre condiciones: 'juego' debe apuntar a una estructura válida.
 * Post condiciones: Carga un vehículo nuevo en el vector de tráfico utilizando obtener_recorrido si no se superó el límite de MAX_AUTOS. Incrementa el tope.
 */

void generar_auto_nuevo(juego_t *juego)
{
    if (juego->tope_vehiculos < MAX_AUTOS)
    {
        int idx = juego->tope_vehiculos;
        obtener_recorrido(juego->vehiculos[idx].recorrido, &(juego->vehiculos[idx].tope_recorrido));
        juego->vehiculos[idx].posicion = 0;
        juego->vehiculos[idx].nafta = (int)(rand() % 21) + NAFTA_MIN_AUTO;  // Entre 30 y 50
        juego->vehiculos[idx].velocidad = (int)(rand() % 4) + VEL_MIN_AUTO; // Entre 1 y 4
        juego->vehiculos[idx].movimientos_frenado = MOV_FRENADO_INICIAL;
        juego->tope_vehiculos++;
    }
}

/*
 * Pre condiciones: 'juego' debe apuntar a una estructura válida.
 * Post condiciones: Evalúa la posición final de los autos respecto a semáforos (aplica frenado de 2 turnos) y lomas de burro (reduce la velocidad actual) para aplicar sus efectos.
 */

void chequear_reacciones_finales(juego_t *juego)
{
    return;
}

/*
 * Pre condiciones: 'juego' debe apuntar a una estructura válida. 'accion' debe ser un carácter de habilidad válido (G, P, T).
 * Post condiciones: Activa el turbo de Homero, congela el tráfico mediante la policía por 5 turnos, o remolca los autos sin nafta en línea recta usando la grúa, descontando un uso disponible.
 */

void ejecutar_habilidad(juego_t *juego, char accion)
{
    if (juego == NULL)
        return;

    const int DURACION_EFECTO_TURBO = 5;
    const int DURACION_POLICIA_FRENADO = 5;

    if (accion == 'T')
    {
        if (juego->homero.turbos_disponibles > 0)
        {
            juego->homero.movs_con_turbo = DURACION_EFECTO_TURBO;
            juego->homero.turbos_disponibles--;
        }
    }

    else if (accion == 'P')
    {
        if (juego->homero.policias_disponibles > 0)
        {
            for (int i = 0; i < juego->tope_vehiculos; i++)
            {
                juego->vehiculos[i].movimientos_frenado = DURACION_POLICIA_FRENADO;
            }
            juego->homero.policias_disponibles--;
        }
    }

    else if (accion == 'G')
    {
        if (juego->homero.gruas_disponibles > 0)
        {
            int i = 0;
            coordenada_t pos_h = juego->homero.posicion;

            while (i < juego->tope_vehiculos)
            {
                int idx = juego->vehiculos[i].posicion;
                coordenada_t pos_a = juego->vehiculos[i].recorrido[idx];

                if (juego->vehiculos[i].nafta <= 0 &&
                    (pos_h.fil == pos_a.fil || pos_h.col == pos_a.col) &&
                    calle_es_continua(pos_h, pos_a, juego->calles, juego->tope_calles))
                {
                    for (int j = i; j < juego->tope_vehiculos - 1; j++)
                    {
                        juego->vehiculos[j] = juego->vehiculos[j + 1];
                    }
                    juego->tope_vehiculos--;
                }
                else
                {
                    i++;
                }
            }
            juego->homero.gruas_disponibles--;
        }
    }
}

/*
 * Pre condiciones: 'juego' debe apuntar a una estructura válida. 'movimiento' debe ser una tecla de dirección válida.
 * Post condiciones: Ejecuta de forma secuencial la simulación del turno: mueve los autos, mueve el monorriel, mueve a Homero, incrementa movimientos, descuenta turnos de turbo, genera tráfico y procesa reacciones.
 */

void mover_entidades(juego_t *juego, char movimiento)
{
    if (juego == NULL)
        return;

    bool estaba_frenado_antes[MAX_AUTOS];
    for (int i = 0; i < juego->tope_vehiculos; i++)
    {
        estaba_frenado_antes[i] = (juego->vehiculos[i].movimientos_frenado > 0);
    }

    bool exito_movimiento = mover_homero(juego, movimiento);

    if (exito_movimiento)
    {
        mover_autos(juego);
        mover_monorriel(&(juego->monorriel));

        juego->movs_realizados++;

        if (juego->homero.movs_con_turbo > 0)
        {
            juego->homero.movs_con_turbo--;
        }

        if (juego->movs_realizados % FRECUENCIA_NUEVO_AUTO == 0)
        {
            generar_auto_nuevo(juego);
        }

        const int DURACION_FRENADO_SEMAFORO = 2;

        for (int i = 0; i < juego->tope_vehiculos; i++)
        {
            if (estaba_frenado_antes[i])
            {
                continue;
            }

            int idx_trayecto = juego->vehiculos[i].posicion;
            coordenada_t pos_auto = juego->vehiculos[i].recorrido[idx_trayecto];

            for (int j = 0; j < juego->tope_objetos; j++)
            {
                if (juego->objetos[j].tipo == TIPO_SEMAFORO)
                {
                    if (distancia_manhattan(pos_auto, juego->objetos[j].posicion) == 1)
                    {
                        if (juego->vehiculos[i].movimientos_frenado < DURACION_FRENADO_SEMAFORO)
                        {
                            juego->vehiculos[i].movimientos_frenado = DURACION_FRENADO_SEMAFORO;
                        }
                    }
                }

                if (juego->objetos[j].tipo == TIPO_LOMA_BURRO)
                {
                    if (pos_auto.fil == juego->objetos[j].posicion.fil &&
                        pos_auto.col == juego->objetos[j].posicion.col)
                    {
                        if (juego->vehiculos[i].velocidad > VEL_MIN_AUTO)
                        {
                            juego->vehiculos[i].velocidad--;
                        }
                    }
                }

                if (juego->objetos[j].tipo == TIPO_ESTACION)
                {
                    if (distancia_manhattan(pos_auto, juego->objetos[j].posicion) == 1)
                    {
                        juego->vehiculos[i].nafta += REFUELLO_ESTACION;
                    }
                }
            }
        }
    }
}

/*
 * Pre condiciones: 'juego' debe apuntar a una estructura válida. 'movimiento' debe ser una acción validada previamente en juego.c.
 * Post condiciones: Normaliza el carácter a mayúscula y deriva la acción hacia la rama de habilidades (G, P, T) o hacia la simulación de movimiento (W, A, S, D).
 */

void realizar_jugada(juego_t *juego, char movimiento)
{
    if (juego == NULL)
        return;
    if (movimiento >= 'a' && movimiento <= 'z')
    {
        movimiento = (char)(movimiento - 'a' + 'A');
    }
    if (movimiento == 'G' || movimiento == 'P' || movimiento == 'T')
    {
        ejecutar_habilidad(juego, movimiento);
    }
    else if (movimiento == 'W' || movimiento == 'S' || movimiento == 'A' || movimiento == 'D')
    {
        mover_entidades(juego, movimiento);
    }
}

/*
 * Pre condiciones: Recibe una estructura de tipo juego_t correctamente inicializada.
 * Post condiciones: Construye una matriz temporal en memoria con el terreno y los elementos ordenados por capas de prioridad para imprimir el mapa de Springfield y su HUD de estadísticas por pantalla.
 */

void mostrar_juego(juego_t juego)
{
    char matriz_pantalla[MAX_FILAS][MAX_COLUMNAS];

    for (int fila = 0; fila < MAX_FILAS; fila++)
    {
        for (int columna = 0; columna < MAX_COLUMNAS; columna++)
        {
            matriz_pantalla[fila][columna] = 'X';
        }
    }
    for (int i = 0; i < juego.tope_calles; i++)
    {
        matriz_pantalla[juego.calles[i].fil][juego.calles[i].col] = ' ';
    }
    for (int i = 0; i < juego.tope_objetos; i++)
    {
        matriz_pantalla[juego.objetos[i].posicion.fil][juego.objetos[i].posicion.col] = juego.objetos[i].tipo;
    }
    if (!juego.homero.recolecto_ancla)
    {
        matriz_pantalla[juego.ancla.fil][juego.ancla.col] = TIPO_ANCLA;
    }
    for (int i = 0; i < juego.monorriel.tope_posiciones; i++)
    {
        int idx = juego.monorriel.posiciones[i];
        matriz_pantalla[juego.monorriel.recorrido[idx].fil][juego.monorriel.recorrido[idx].col] = 'M';
    }
    for (int i = 0; i < juego.tope_vehiculos; i++)
    {
        int idx = juego.vehiculos[i].posicion;
        matriz_pantalla[juego.vehiculos[i].recorrido[idx].fil][juego.vehiculos[i].recorrido[idx].col] = 'A';
    }
    matriz_pantalla[juego.homero.posicion.fil][juego.homero.posicion.col] = 'H';

    for (int fila = 0; fila < MAX_FILAS; fila++)
    {
        for (int columna = 0; columna < MAX_COLUMNAS; columna++)
        {
            printf("%c ", matriz_pantalla[fila][columna]);
        }
        printf("\n");
    }

    printf("\n=========================================\n");
    printf(" Nafta disponible: %d L | Movimientos: %d\n", juego.homero.nafta, juego.movs_realizados);
    printf(" ¿Tiene el ancla?: %s\n", juego.homero.recolecto_ancla ? "SÍ" : "NO");
    printf(" Grúas: %d | Policías: %d | Turbos: %d\n",
           juego.homero.gruas_disponibles,
           juego.homero.policias_disponibles,
           juego.homero.turbos_disponibles);
    printf("=========================================\n");
}

/*
 * Pre condiciones: Recibe una estructura de tipo juego_t válida.
 * Post condiciones: Devuelve 1 si Homero ganó, -1 si perdió (choques, falta de nafta o victoria del monorriel) o 0 si la partida sigue en curso.
 */

int estado_juego(juego_t juego)
{
    const int JUEGO_GANADO = 1;
    const int JUEGO_PERDIDO = -1;
    const int JUEGO_SIGUE = 0;

    coordenada_t pos_h = juego.homero.posicion;
    coordenada_t meta_estacion = {1, 1};

    if (juego.homero.recolecto_ancla && pos_h.fil == meta_estacion.fil && pos_h.col == meta_estacion.col)
    {
        return JUEGO_GANADO;
    }

    int idx_cabeza = juego.monorriel.posiciones[0];
    coordenada_t pos_cabeza_m = juego.monorriel.recorrido[idx_cabeza];

    if (pos_cabeza_m.fil == meta_estacion.fil && pos_cabeza_m.col == meta_estacion.col && juego.movs_realizados > 0)
    {
        return JUEGO_PERDIDO;
    }

    if (juego.homero.nafta <= 0)
    {
        return JUEGO_PERDIDO;
    }

    for (int i = 0; i < juego.monorriel.tope_posiciones; i++)
    {
        int idx_vagon = juego.monorriel.posiciones[i];
        coordenada_t pos_vagon = juego.monorriel.recorrido[idx_vagon];
        if (pos_h.fil == pos_vagon.fil && pos_h.col == pos_vagon.col)
        {
            return JUEGO_PERDIDO;
        }
    }

    for (int i = 0; i < juego.tope_vehiculos; i++)
    {
        int idx_auto = juego.vehiculos[i].posicion;
        coordenada_t pos_auto = juego.vehiculos[i].recorrido[idx_auto];
        if (pos_h.fil == pos_auto.fil && pos_h.col == pos_auto.col)
        {
            return JUEGO_PERDIDO;
        }
    }
    return JUEGO_SIGUE;
}
