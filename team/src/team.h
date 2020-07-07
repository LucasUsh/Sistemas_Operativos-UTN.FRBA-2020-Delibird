/*
 * team.h
 *
 *  Created on: 18 abr. 2020
 *      Author: utnso
 */

#ifndef TEAM_H_
#define TEAM_H_
#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include "../../OurLibraries/Sockets/sockets.h"
#include "../../OurLibraries/UniversoPokemon/universo.h"
#include <pthread.h>
#include <math.h>
#include <time.h>

typedef enum {
	RR    = 1,
	SJFSD = 2,
	SJFCD = 3,
	FIFO  = 4
} algoritmo_code;

typedef enum {
	NEW     = 1,
	READY   = 2,
	BLOCKED = 3,
	EXEC 	= 4,
	EXIT 	= 5
} estado_code;

typedef struct {
	t_posicion posicion;
	t_posicion posicion_destino;
	t_list* 	pokemones; //list of t_pokemon
	t_list* 	objetivo; //list of t_pokemon
	estado_code estado;

} t_entrenador;

typedef struct {
	int32_t 	algoritmo_code;
	char* 	algoritmo_string;
	int32_t 	quantum;
	int32_t 	retardo;
}t_algoritmo;

typedef struct {
	//uint32_t size_Nombre;
	char * 	 	nombre;
	uint32_t 	cantidad; // creo que no deberia estar pero no quiero romper nada de Team asi que lo dejo :p
	t_posicion* posicion; //supongo que pasará lo mismo con esto jajajaja
}t_pokemon_team;




#endif /* TEAM_H_ */
