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
	int posicion_inicial;
	int posicion_final;
	int size;
	bool ocupada;
	int indiceParticion; //valor que asigna Broker a partir de una variable global
	int ramaBuddy; // 0 si es la de la izquierda, 1 si es la de la derecha
} t_particion;

t_list* tabla_particiones;

t_particion* crearParticion(int inicio, int fin, bool ocupada);
t_particion generarParticionDinamicamente(int32_t sizeMensaje, int32_t sizeMinParticion);
bool particionCandidata(t_particion* particion, int32_t sizeMensaje);
t_particion* getParticionFirstFit(int32_t sizeMensaje);
t_particion* getParticionBestFit(int32_t sizeMensaje);
void dividirParticionDinamica(int indiceParticion, t_particion* particionOriginal, int32_t sizeMsg);
bool particionCandidataVictima(t_particion* particion);
t_particion * seleccionarVictimaFIFO();
t_particion * seleccionarVictimaLRU();
int obtenerPosicion(t_particion * particion);
t_particion * consolidarParticion(t_particion * particion);
int tamanioMinimo(int tamanioSolicitadoEnBytes);
void inicializarMemoriaBS(t_config* config);
t_particion * generarParticionBS(t_particion* particionInicial);




#endif /* PARTICIONES_H_ */
