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
} t_particion;

t_list* tabla_particiones;




#endif /* PARTICIONES_H_ */
