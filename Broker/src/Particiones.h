/*
 * Particiones.h
 *
 *  Created on: 22 jun. 2020
 *      Author: utnso
 */

//#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
//#include<string.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include <commons/collections/list.h>


typedef struct {
	int posicion_inicial;
	int posicion_final;
	int size;
	bool ocupada;
	int indiceParticion; //valor que asigna Broker a partir de una variable global
} t_particion;

t_list* tabla_particiones;


