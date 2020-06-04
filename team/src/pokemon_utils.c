/*
 * pokemon_utlis.c
 *
 *  Created on: 4 jun. 2020
 *      Author: utnso
 */
#include "pokemon_utils.h"

t_pokemon* get_pokemon(char* pokemon_name){
	t_pokemon* pokemon = malloc(sizeof(t_pokemon));

	pokemon->cantidad = 1;
	pokemon->nombre = pokemon_name;

	return pokemon;
}


t_list* get_pokemones(t_config* config, int index){
	t_list* pokemones_list = list_create();

	char** pokemon_entrenadores = config_get_array_value(config, "POKEMON_ENTRENADORES");
	char** pokemones = get_array_by_index(pokemon_entrenadores, index);

	int i = 0;

	while(pokemones[i] != NULL){
		t_pokemon* pokemon = get_pokemon(pokemones[i]);
		list_add_in_index(pokemones_list, i, pokemon);
		i++;
	}

	return pokemones_list;
}

void mostrar_pokemon(t_pokemon* pokemon_actual, t_list* objetivo_global){
	printf("| 	nombre: %s, cantidad: %d\n",
			pokemon_actual->nombre,
			pokemon_actual->cantidad );
}

