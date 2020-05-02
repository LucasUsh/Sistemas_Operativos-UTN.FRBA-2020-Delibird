/*
 * team.h
 *
 *  Created on: 18 abr. 2020
 *      Author: utnso
 */

#ifndef TEAM_H_
#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include "/home/utnso/workspace/tp-2020-1c-5rona/OurLibraries/Sockets/socketsUnificado.c"

typedef enum {
	RR = 1,
	SJFSD = 2,
	SJFCD = 3,
	FIFO = 4
} algoritmo_code;

typedef enum {
	NEW = 1,
	READY = 2,
	BLOCKED = 3,
	EXEC = 4,
	EXIT = 5
} estado_code;

typedef struct {
	t_posicion* posicion;
	t_list* pokemones; //list of t_pokemon
	t_list* objetivo; //list of t_pokemon
	estado_code estado;

} t_entrenador;

typedef struct {
	int algoritmo_code;
	char* algoritmo_string;
	int quantum;
	int retardo;
} t_algoritmo;


#define TEAM_H_



#endif /* TEAM_H_ */
