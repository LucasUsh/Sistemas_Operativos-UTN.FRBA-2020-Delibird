/*
 * pokemon_utils.h
 *
 *  Created on: 4 jun. 2020
 *      Author: utnso
 */



#ifndef POKEMON_UTILS_H_

#define POKEMON_UTILS_H_
#include "../../OurLibraries/UniversoPokemon/universo.h"
#include <commons/collections/list.h>
#include <commons/config.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "team.h"
t_pokemon_team* get_pokemon(char* pokemon_name);
t_list* 		get_pokemones(t_config* config, int32_t index);
void 			mostrar_pokemon(t_pokemon_team* pokemon_actual, t_list* objetivo_global);

#endif /* POKEMON_UTILS_H_ */
