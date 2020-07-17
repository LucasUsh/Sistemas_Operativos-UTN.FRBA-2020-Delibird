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
	double id_mensaje;
	double id; //valor que asigna Broker a partir de una variable global
	//int32_t ramaBuddy; // 0 si no es Buddy System, 1 si es la de la izquierda, 2 si es la de la derecha
} t_particion;

t_list* tabla_particiones;

t_particion* crearParticion(int inicio, int size, bool ocupada);

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
void algoritmoBuddySystem(info_mensaje * mensaje, int32_t algoritmoReemplazo);
void algoritmoParticionDinamica(info_mensaje * mensaje, int32_t frecuenciaCompactacion, int32_t algoritmoReemplazo, int32_t algoritmoParticionLibre);
void algoritmoLiberacion(int32_t algoritmoReemplazo);
void algoritmoCompactacion();

void guardarMensaje(info_mensaje * mensaje, t_particion * particion);




#endif /* PARTICIONES_H_ */
