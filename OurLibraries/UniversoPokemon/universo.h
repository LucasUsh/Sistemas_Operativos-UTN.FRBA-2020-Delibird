/*
 * universo.h
 *
 *  Created on: 2 may. 2020
 *      Author: utnso
 */

#ifndef UNIVERSO_H_
#define UNIVERSO_H_

#include<commons/collections/list.h>


/****************************************/
/* UNIVERSO POKEMON*/
/****************************************/

typedef struct {
	int size_Nombre;
	char * nombre;
	int cantidad; // creo que no deberia estar pero no quiero romper nada de Team asi que lo dejo :p
} t_pokemon;

typedef struct {
	int X;
	int Y;
}t_posicion;

typedef struct {
	t_pokemon * pokemones;/* aca iria un t_list * listaPokemones y cuando haces list_add le pasas
								como parametro el tipo t_pokemon*/
} t_objetivo;



/****************************************/
/* PAYLOADS DE MENSAJES*/
/****************************************/

typedef struct {
	t_pokemon pokemon;
	t_posicion posicion;
	int cant;
} t_New; // codigo_operacion= 1

typedef struct {
	t_pokemon pokemon;
	int cantUbicaciones;
	t_list * listaPosiciones;
} t_Localized; // codigo_operacion= 2

typedef struct {
	t_pokemon pokemon;
} t_Get; // codigo_operacion= 3

typedef struct {
	t_pokemon pokemon;
	t_posicion posicion;
} t_Appeared; // codigo_operacion= 4

typedef struct {
	t_pokemon pokemon;
	t_posicion posicion;
} t_Catch; // codigo_operacion= 5

typedef struct {
	int fueAtrapado; // 1 cuando fue atrapado, 0 cuando no fue atrapado
} t_Caught; // codigo_operacion= 6

int universo_init();

#endif /* UNIVERSO_H_ */
