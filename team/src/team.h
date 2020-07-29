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
#include <semaphore.h>


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
	int32_t 	algoritmo_code;
	char* 	algoritmo_string;
	int32_t 	quantum;
	int32_t 	retardo;
}t_algoritmo;

typedef struct {
	int32_t id_respuesta;
	int32_t id_entrenador;

} t_respuesta;

typedef struct {
	void* mensaje;
	t_respuesta* respuesta;
}t_args_mensajes;


typedef struct {
	char * 	 	nombre;
	int32_t 	cantidad;
	t_posicion posicion;
	bool planificable;
}t_pokemon_team;

typedef struct {
	t_posicion posicion;
	t_pokemon_team* pokemon_destino;
	t_list* 	pokemones;
	t_list* 	objetivo;
	estado_code estado;
	int32_t id;
	bool ocupado;
	sem_t* semaforo;

} t_entrenador;

typedef struct {
	t_entrenador* entrenador;
	sem_t semaforo_entrenador;
}t_args_entrenador;

t_log* logger;
t_config* config;

//Datos traidos por config
char* IP_BROKER;
char* PUERTO_BROKER;
int32_t PROCESS_ID;
t_list* objetivo_global;
t_list* entrenadores;
t_algoritmo algoritmo;


#endif /* TEAM_H_ */
