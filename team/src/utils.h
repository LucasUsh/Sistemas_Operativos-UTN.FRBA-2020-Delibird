/*
 * utils.h
 *
 *  Created on: 4 jun. 2020
 *      Author: utnso
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/config.h>
#include "team.h"

char** get_array_by_index(char** array_from_config, int32_t index);
int32_t array_length(char** value_array);
t_config* get_config(char* name);
int32_t get_algoritmo_code(char* algoritmo);
t_algoritmo get_algoritmo(t_config* config);
t_list* get_objetivos(t_config* config, int32_t index);
t_posicion get_posicion(t_config* config, int32_t index);
t_entrenador* get_entrenador(t_config* config, int32_t index);
t_list* get_entrenadores(t_config* config, int32_t cantidadEntrenadores);
int32_t get_distancia_entre_puntos(t_posicion pos1, t_posicion pos2);
t_list* get_objetivo_global(t_list* entrenadores);
t_entrenador* get_entrenador_planificable_mas_cercano(t_list* entrenadores, t_posicion posicion_pokemon);
t_list* filtrar_localized_repetidos(t_list* mensajes_localized);
t_list* filtrar_localized_objetivo_global(t_list* mensajes_localized, t_list* objetivo_global);
t_list* filtrar(t_list* mensajes_localized,t_list* objetivo_global);
bool es_respuesta(int id, t_list* lista_ids);
#endif /* UTILS_H_ */
