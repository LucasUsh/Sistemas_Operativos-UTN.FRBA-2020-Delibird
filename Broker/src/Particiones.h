/*
 * particiones.h
 *
 *  Created on: 24 jun. 2020
 *      Author: utnso
 */

#ifndef PARTICIONES_H_
#define PARTICIONES_H_


#include<stdlib.h>
#include<stdint.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include <commons/collections/list.h>
#include "Broker.h"

typedef struct {
	op_code codigo_operacion; //de la cola a la que pertenece el mensaje
	int32_t posicion_inicial;
	int32_t posicion_final;
	int32_t size;
	bool ocupada;
	double id; //valor que asigna Broker a partir de una variable global
	int32_t ramaBuddy; // 0 si no es Buddy System, 1 si es la de la izquierda, 2 si es la de la derecha
} t_particion;

typedef enum {
	BS    = 1,
	PARTICIONES = 2
} algoritmoMemoria;

typedef enum {
	FIFO    = 1,
	LRU = 2
} algoritmoReemplazo;

typedef enum {
	FF    = 1,
	BF = 2
} algoritmoParticionLibre;

t_list* tabla_particiones;

t_particion* crearParticion(int inicio, int fin, bool ocupada, int ramaBuddy);

void administrarMensaje(char algoritmoMemoria, info_mensaje mensaje, int32_t frecuenciaCompactacion, char algoritmoReemplazo, char algoritmoParticionLibre);

bool particionCandidata(t_particion* particion, int32_t sizeMensaje);
t_particion* getParticionFirstFit(int32_t sizeMensaje);
t_particion* getParticionBestFit(int32_t sizeMensaje);

void generarParticionDinamica(t_particion* particionOriginal, int32_t sizeMsg);
bool particionCandidataVictima(t_particion* particion);
void liberar(char algoritmoReemplazo);
t_particion * algoritmoFIFO();
t_particion * algoritmoLRU();

int obtenerPosicion(t_particion * particion);
t_particion * consolidarParticion(t_particion * particion, int posicion);
t_particion * consolidarParticionBS(t_particion * particion, int posicion);

int32_t tamanioMinimo(int32_t sizeMsg);
void generarParticionBS(t_particion* particionInicial);
void algoritmoBuddySystem(info_mensaje mensaje, char algoritmoReemplazo);
void algoritmoParticionDinamica(info_mensaje mensaje, int32_t frecuenciaCompactacion, char algoritmoReemplazo, char algoritmoParticionLibre);
void algoritmoLiberacion(int32_t frecuenciaCompactacion, char algoritmoReemplazo);
void algoritmoLiberacionBS(char algoritmoReemplazo);




#endif /* PARTICIONES_H_ */
