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
	int cantidad;
} t_pokemon;

typedef struct {
	int X;
	int Y;
}t_posicion;

typedef struct {
	t_pokemon* pokemones;
} t_objetivo;



/****************************************/
/* PAYLOADS DE MENSAJES*/
/****************************************/

typedef struct {
	// codigo_operacion= 1;
	t_pokemon pokemon;
	int posX;
	int posY;
	int cant;
} t_New;

typedef struct {
	// codigo_operacion= 2;
	t_pokemon pokemon;
	int cantUbicaciones;
	t_list * listaPosiciones;
} t_Localized;

typedef struct {
	// codigo_operacion= 3;
	t_pokemon pokemon;
} t_Get;

typedef struct {
	// codigo_operacion= 4;
	t_pokemon pokemon;
	int posX;
	int posY;
} t_Appeared;

typedef struct {
	// codigo_operacion= 5;
	t_pokemon pokemon;
	int posX;
	int posy;
} t_Catch;

typedef struct {
	// codigo_operacion= 6;
	int fueAtrapado; // 1 cuando fue atrapado, 0 cuando no fue atrapado
} t_Caught;

int universo_init();

#endif /* UNIVERSO_H_ */
