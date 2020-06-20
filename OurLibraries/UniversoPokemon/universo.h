/*
 * universo.h
 *
 *  Created on: 2 may. 2020
 *      Author: utnso
 */

#ifndef UNIVERSO_H_
#define UNIVERSO_H_

#include<commons/collections/list.h>
#include<stdint.h>


/****************************************/
/* UNIVERSO POKEMON*/
/****************************************/


typedef struct {
	int32_t X;
	int32_t Y;
} __attribute__((packed)) t_posicion; // ocupa 8 bytes

typedef struct {
	int32_t size_Nombre;
	char * nombre;
	//int32_t cantidad; // creo que no deberia estar pero no quiero romper nada de Team asi que lo dejo :p
	//t_posicion* posicion; //supongo que pasará lo mismo con esto jajajaja
} t_pokemon;
// con esta estructura ocupa maximo 26 bytes, minimo 19 bytes
// nombre mas corto: Mew/Muk (3 bytes); nombre mas largo: Feraligatr (10 bytes)

/* Lauti:
 * podríamos imaginar un caso que envíen laevoluciondepikachu, hay que contemplar casos que pueden enviar cualquier cosa
 * no digo que lo hagan pero no creo que podamos confirmar al 100% el tamaño del tipo de dato, hay que calcularlo
*/




typedef struct {
	t_list pokemones;/* aca iria un t_list * listaPokemones y cuando haces list_add le pasas
								como parametro el tipo t_pokemon*/
} t_objetivo;



/****************************************/
/* PAYLOADS DE MENSAJES*/
/****************************************/

typedef struct {
	t_pokemon pokemon;
	t_posicion posicion;
	int32_t cant;
} t_New; // codigo_operacion= 1
// ocupa entre 35 y 42 bytes

typedef struct {
	t_pokemon pokemon;
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
	int32_t fueAtrapado; // 1 cuando fue atrapado, 0 cuando no fue atrapado
} t_Caught; // codigo_operacion= 6

int32_t universo_init();

#endif /*UNIVERSO_H_*/
