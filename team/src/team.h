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


typedef enum {
	RR = 1,
	SJFSD = 2,
	SJFCD = 3
} algoritmo_code;

typedef enum {
	NEW = 1,
	READY = 2,
	BLOCKED = 3,
	EXEC = 4,
	EXIT = 5
} estado_code;

typedef struct
{
	char* position;
	t_list* pokemones;
	t_list* objetivo;
	estado_code estado;
	algoritmo_code algoritmo;

} t_trainer;

typedef struct {
	char* nombre;
	int cantidad;
} t_pokemon;

typedef struct {
	t_pokemon* pokemones;
} t_objetivo;


#define TEAM_H_



#endif /* TEAM_H_ */
