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
t_list* cola_ready;

t_config* get_config(char* name){
	char* cfg_path = string_new();
	string_append(&cfg_path, "../config/");
	string_append(&cfg_path, name);

	return config_create(cfg_path);
}

int array_length(char** value_array){
	int i = 0;

	while(value_array[i] != NULL){
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

	return 0;
}

t_algoritmo* get_algoritmo(t_config* config){
	t_algoritmo* algoritmo = (t_algoritmo*)malloc(sizeof(t_algoritmo));

	char* algoritmo_string = config_get_string_value(config, "ALGORITMO_PLANIFICACION");

	algoritmo->algoritmo_string = algoritmo_string;
	algoritmo->algoritmo_code = get_algoritmo_code(algoritmo_string);
	algoritmo->retardo = atoi(config_get_string_value(config, "RETARDO_CICLO_CPU"));


	if(algoritmo->algoritmo_code == RR){
		algoritmo->quantum = atoi(config_get_string_value(config, "QUANTUM"));
	} else {
		algoritmo->quantum = 0;
	}

	return algoritmo;
}

char** get_array_by_index(char** array_from_config, int index){

	 /* EJEMPLO CON POKEMONES
	  * array_from_config:
	  *  0 -> Pikachu|Squirtle|Pidgey
	  *  1 -> Squirtle|Charmander
	  *  2 -> Bulbasaur
	  */

	//value_string = Pikachu|Squirtle|Pidgey
	char* value_string = array_from_config[index];
	/*
	  * value_array:
	  *  0 -> Pikachu
	  *  1 -> Squirtle
	  *  2 -> Pidgey
	  */
	char** value_array = string_split(value_string, "|");

	return value_array;
}

t_pokemon* get_pokemon(char* pokemon_name){
	t_pokemon* pokemon = malloc(sizeof(t_pokemon));

	pokemon->cantidad = 1;
	pokemon->nombre = pokemon_name;

	return pokemon;
}

t_list* get_pokemones(t_config* config, int index){
	t_list* pokemones_list = list_create();

	char** pokemon_entrenadores = config_get_array_value(config, "POKEMON_ENTRENADORES");
	char** pokemones = get_array_by_index(pokemon_entrenadores,index);

	int i = 0;

	while(pokemones[i] != NULL){
		t_pokemon* pokemon = get_pokemon(pokemones[i]);
		list_add_in_index(pokemones_list, i, pokemon);
		i++;
	}

	return pokemones_list;
}

t_list* get_objetivos(t_config* config, int index){
	t_list* objetivos = list_create();

	char** pokemon_entrenadores = config_get_array_value(config, "OBJETIVOS_ENTRENADORES");
	char** pokemones = get_array_by_index(pokemon_entrenadores,index);

	int i = 0;

	while(pokemones[i] != NULL){
		t_pokemon* pokemon = get_pokemon(pokemones[i]);
		list_add_in_index(objetivos, i, pokemon);
		i++;
	}

	return objetivos;
}

t_posicion* get_posicion(t_config* config, int index){
	t_posicion* posicion = (t_posicion*)malloc(sizeof(t_posicion));

	char** posiciones = get_array_by_index(config_get_array_value(config, "POSICIONES_ENTRENADORES"),
			index);

	posicion->X = atoi(posiciones[0]);
	posicion->Y = atoi(posiciones[1]);


	return posicion;
}

t_list* sumarizar_pokemones(t_list* lista_pokemones_sin_sumarizar){
	t_list* lista_sumarizada = list_create();
	for(int i = 0; i < lista_pokemones_sin_sumarizar->elements_count; i++){
		t_pokemon* pokemon = list_get(lista_pokemones_sin_sumarizar, i);
		int pokemon_encontrado = 0;

		if(lista_sumarizada->elements_count == 0){
			list_add(lista_sumarizada, pokemon);
		} else {

			for(int j = 0; j < lista_sumarizada->elements_count; j++){
				t_pokemon* pokemon_sumarizado = list_get(lista_sumarizada, j);

				if(string_equals_ignore_case(pokemon_sumarizado->nombre, pokemon->nombre)){
					pokemon_sumarizado->cantidad++;
					pokemon_encontrado = 1;
				}
			}

			if(!pokemon_encontrado){
				list_add(lista_sumarizada, pokemon);
			}
		}
	}

	return lista_sumarizada;

}

t_entrenador* get_entrenador(t_config* config, int index){
	t_entrenador* entrenador = (t_entrenador*)malloc(sizeof(t_entrenador));

	entrenador->estado = NEW;
	entrenador->posicion = get_posicion(config, index);

	entrenador->pokemones = sumarizar_pokemones(get_pokemones(config, index));
	entrenador->objetivo = sumarizar_pokemones(get_objetivos(config, index));

	return entrenador;
}



t_list* get_entrenadores(t_config* config, int cantidadEntrenadores){
	t_list* entrenadores_list = list_create();

	int i;
	for(i=0 ; i < cantidadEntrenadores; i++){

		//cada entrenador es un hilo
		t_entrenador* entrenador = malloc(sizeof(t_entrenador));
		entrenador = get_entrenador(config, i);


		////////////// SECCION CRITICA////////////////////
		list_add(cola_ready, entrenador);
		//////////////////////////////////////////////////


		list_add_in_index(entrenadores_list, i, entrenador);

	}

	return entrenadores_list;

}

int get_distancia_entre_puntos(t_posicion pos1, t_posicion pos2){
	//raiz de (x2-x1)^2 + (y2-y1)^2

	int distX = pos2.X - pos1.X;
	int distY = pos2.Y - pos1.Y;

	return sqrt(pow(distX,2) + pow(distY,2));

}

t_entrenador* get_entrenador_mas_cercano(t_list* entrenadores, t_posicion posicion_pokemon){

	t_entrenador* entrenador_cercano;
	t_entrenador* entrenador;
	int distancia_mas_chica = 0;

	for(int i = 0; i < entrenadores->elements_count; i++){

		entrenador = list_get(entrenadores, i);

		int distancia_entrenador = get_distancia_entre_puntos(*entrenador->posicion, posicion_pokemon);


		if(distancia_mas_chica == 0){
			distancia_mas_chica = distancia_entrenador;
			entrenador_cercano= entrenador;
		} else if (distancia_entrenador < distancia_mas_chica) {
			distancia_mas_chica = distancia_entrenador;
			entrenador_cercano = entrenador;
		}
	}


	return entrenador_cercano;
}




t_entrenador* avanzar(t_entrenador* entrenador, int posX, int posY){
	int nuevaPosicionX = entrenador->posicion->X + posX;
	int nuevaPosicionY = entrenador->posicion->Y + posY;

	entrenador->posicion->X = nuevaPosicionX;
	entrenador->posicion->Y = nuevaPosicionY;

	return entrenador;
}

int main(int argc, char** argv)
{
	cola_ready = list_create();
	t_list* objetivo_global = list_create();

    printf("el entrenador que se va a cargar es el de la config: %s\n", argv[1] );
    //char* config_name = argv[1];

    //t_config* entrenador_config = config_create(argv[1]);
    t_config* entrenador_config = config_create("/home/utnso/workspace/tp-2020-1c-5rona/team/config/entrenador1.config");


    int cantidadEntrenadores = array_length(config_get_array_value(entrenador_config, "POKEMON_ENTRENADORES"));
    printf("En este team hay %d entrenadores\n", cantidadEntrenadores);


    t_algoritmo* algoritmo = get_algoritmo(entrenador_config);
    t_list* entrenadores = get_entrenadores(entrenador_config, cantidadEntrenadores);


    printf("PLANIFICACION: \n	ALGORITMO: %s, QUANTUM: %d\n", algoritmo->algoritmo_string, algoritmo->quantum);
    printf("********************\n");

    for(int i = 0; i < cantidadEntrenadores; i++){
    	t_entrenador* entrenador_actual = list_get(entrenadores, i);
    	printf("|ENTRENADOR %d\n|----------------\n|POSICION: (%d,%d)\n",
    			i + 1,
				entrenador_actual->posicion->X,
				entrenador_actual->posicion->Y);


    	printf("|POKEMONES:\n");
    	int j = 0;
    	for(j=0; j < entrenador_actual->pokemones->elements_count; j++){
    		t_pokemon* pokemon_actual = list_get(entrenador_actual->pokemones, j);
    		printf("| 	nombre: %s, cantidad: %d\n",
					pokemon_actual->nombre,
					pokemon_actual->cantidad );
    	}

    	printf("|OBJETIVOS:\n");
    	    	int y = 0;
    	    	for(y=0; y < entrenador_actual->objetivo->elements_count; y++){
    	    		t_pokemon* pokemon_actual = list_get(entrenador_actual->objetivo, y);
    	    		list_add(objetivo_global, pokemon_actual);
    	    		printf("| 	nombre: %s, cantidad: %d\n",
    						pokemon_actual->nombre,
    						pokemon_actual->cantidad );
    	    	}

    printf("********************\n");




    }


    objetivo_global = sumarizar_pokemones(objetivo_global);

	printf("|OBJETIVO GLOBAL:\n");
			int b = 0;
			for(b=0; b < objetivo_global->elements_count; b++){
				t_pokemon* pokemon_actual = list_get(objetivo_global, b);
				printf("| 	nombre: %s, cantidad: %d\n",
						pokemon_actual->nombre,
						pokemon_actual->cantidad );
			}

	printf("********************\n");

    printf("End");

	return EXIT_SUCCESS;
}

void planificar_fifo(t_pokemon* pokemon_a_capturar){

	// en fifo, el proximo entrenador es el que esté primero en la cola de ready
	t_entrenador* entrenador = list_get(cola_ready, 0);

	return;
}

void planificar_rr(t_pokemon* pokemon_a_capturar){
	return;
}

void planificar_sjfsd(t_pokemon* pokemon_a_capturar){
	return;
}

void planificar_sjfcd(t_pokemon* pokemon_a_capturar){
	return;
}

void planificar(t_algoritmo* algoritmo, t_pokemon* pokemon_a_capturar){
	switch(algoritmo->algoritmo_code){
	case FIFO:
		planificar_fifo(pokemon_a_capturar);
		break;
	case RR:
		planificar_rr(pokemon_a_capturar);
		break;
	case SJFSD:
		planificar_sjfsd(pokemon_a_capturar);
		break;
	case SJFCD:
		planificar_sjfcd(pokemon_a_capturar);
		break;
	default:
		return;
	}

}






