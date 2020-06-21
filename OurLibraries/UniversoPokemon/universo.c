/*
 * universo.c
 *
 *  Created on: 2 may. 2020
 *      Author: utnso
 */


#include "universo.h"

int32_t universo_init(){
	return 0;
}

int32_t tamanio_new (t_New* nuevo_pokemon) {
	int32_t tam = 0;

	tam += nuevo_pokemon->pokemon.size_Nombre;

	tam += sizeof(nuevo_pokemon->pokemon.size_Nombre);

	tam += sizeof(nuevo_pokemon->posicion);

	tam += sizeof(nuevo_pokemon->cant);

	return tam;
}

