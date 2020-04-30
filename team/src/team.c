/*
 ============================================================================
 Name        : team.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "team.h"


int cantidadEntrenadores = 0;

t_config* get_config(char* name){
	char* cfg_path = string_new();
	string_append(&cfg_path, "../config/");
	string_append(&cfg_path, name);

	return config_create(cfg_path);
}


int get_cantidad_entrenadores(char* pokemones){

	char** lsPokemones = string_split(pokemones, ",");
	int i = 0;

	while(lsPokemones[i] != NULL){
		i++;
	}

	return i;
}


int get_algoritmo_code(char* algoritmo){
	if(string_equals_ignore_case(algoritmo, "RR")){
		return RR;
	} else if (string_equals_ignore_case(algoritmo, "SJF-SD")){
		return SJFSD;
	} else if (string_equals_ignore_case(algoritmo, "SJF-CD")){
		return SJFCD;
	} else if (string_equals_ignore_case(algoritmo, "FIFO")){
		return FIFO;
	}
}

t_algoritmo* get_algoritmo(t_config* config){
	t_algoritmo* algoritmo = (t_algoritmo*)malloc(sizeof(t_algoritmo));

	char* algoritmo_string = config_get_string_value(config, "ALGORITMO_PLANIFICACION");

	algoritmo->algoritmo_string = algoritmo_string;
	algoritmo->algoritmo_code = get_algoritmo_code(algoritmo_string);
	algoritmo->retardo = atoi(config_get_string_value(config, "RETARDO_CICLO_CPU"));


	if(algoritmo->algoritmo_code == RR){
		algoritmo->quantum = atoi(config_get_string_value(config, "QUANTUM"));
	}

	return algoritmo;
}


t_list* get_pokemones(char* pokemones_string, int index){
	t_list* pokemones = list_create();

	return pokemones;
}

t_list* get_objetivos(char* objetivos_string, int index){
	t_list* objetivos = list_create();

	return objetivos;
}

t_posicion* get_posicion(char* posicion_string, int index){
	t_posicion* posicion = (t_posicion*)malloc(sizeof(t_posicion));


	return posicion;
}

t_entrenador* get_entrenador(t_config* config, int index){
	t_entrenador* entrenador = (t_entrenador*)malloc(sizeof(t_entrenador));

	entrenador->algoritmo = get_algoritmo(config);
	entrenador->estado = NEW;
	//entrenador->pokemones = get_pokemones(config_get_string_value(config, "POKEMON_ENTRENADORES"), index);
	//entrenador->posicion = get_posicion(config_get_string_value(config, "POSICIONES_ENTRENADORES"), index);
	//entrenador->objetivo = get_objetivos(config_get_string_value(config, "OBJETIVOS_ENTRENADORES"), index);

	return entrenador;
}

t_list* get_entrenadores(t_config* config, int cantidadEntrenadores){
	t_list* entrenadores = list_create();

	int i;
	for(i=0 ; i < cantidadEntrenadores; i++){
		t_entrenador* entrenador = malloc(sizeof(t_entrenador));
		entrenador = get_entrenador(config, i);
		list_add_in_index(entrenadores, i, entrenador);
	}

	return entrenadores;

}



int main(int argc, char** argv)
{
    printf("el entrenador que se va a cargar es el de la config: %s\n", argv[1] );

    //char* config_name = argv[1];

    //t_config* entrenador_config = config_create(argv[1]);
    t_config* entrenador_config = config_create("/home/utnso/tp-2020-1c-5rona/team/config/entrenador1.config");


    int cantidadEntrenadores = get_cantidad_entrenadores(config_get_string_value(entrenador_config, "POKEMON_ENTRENADORES"));
    printf("En este team hay %d entrenadores\n", cantidadEntrenadores);


    t_list* entrenadores = get_entrenadores(entrenador_config, cantidadEntrenadores);


    for(int i = 0; i < cantidadEntrenadores; i++){
    	t_entrenador* entrenador_actual = list_get(entrenadores, i);
    	printf("entrenador %d planificacion: %s, quantum: %d\n", i, entrenador_actual->algoritmo->algoritmo_string, entrenador_actual->algoritmo->algoritmo_code);
    }


	return EXIT_SUCCESS;
}









