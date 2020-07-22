#include "utils.h"

char** get_array_by_index(char** array_from_config, int32_t index){

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


int32_t array_length(char** value_array){
	int32_t i = 0;

	while(value_array[i] != NULL){
		i++;
	}

	return i;
}

t_config* get_config(char* name){
	char* cfg_path = string_new();
	string_append(&cfg_path, "../config/");
	string_append(&cfg_path, name);

	return config_create(cfg_path);
}

int32_t get_algoritmo_code(char* algoritmo){
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

t_algoritmo get_algoritmo(t_config* config){
	t_algoritmo algoritmo;

	char* algoritmo_string = config_get_string_value(config, "ALGORITMO_PLANIFICACION");

	algoritmo.algoritmo_string = algoritmo_string;
	algoritmo.algoritmo_code = get_algoritmo_code(algoritmo_string);
	algoritmo.retardo = atoi(config_get_string_value(config, "RETARDO_CICLO_CPU"));


	if(algoritmo.algoritmo_code == RR){
		algoritmo.quantum = atoi(config_get_string_value(config, "QUANTUM"));
	} else {
		algoritmo.quantum = 0;
	}

	return algoritmo;
}

t_list* get_objetivos(t_config* config, int32_t index){
	t_list* objetivos = list_create();

	char** pokemon_entrenadores = config_get_array_value(config, "OBJETIVOS_ENTRENADORES");
	char** pokemones = get_array_by_index(pokemon_entrenadores,index);

	int32_t i = 0;

	while(pokemones[i] != NULL){
		t_pokemon_team* pokemon = get_pokemon(pokemones[i]);
		list_add_in_index(objetivos, i, pokemon);
		i++;
	}

	return objetivos;
}

t_posicion get_posicion(t_config* config, int32_t index){
	t_posicion posicion;

	char** posiciones = get_array_by_index(config_get_array_value(config, "POSICIONES_ENTRENADORES"),
			index);

	posicion.X = atoi(posiciones[0]);
	posicion.Y = atoi(posiciones[1]);


	return posicion;
}



t_entrenador* get_entrenador(t_config* config, int32_t index){
	t_entrenador* entrenador = (t_entrenador*)malloc(sizeof(t_entrenador));

	entrenador->estado = NEW;
	entrenador->posicion = get_posicion(config, index);

	entrenador->pokemones = sumarizar_pokemones(get_pokemones(config, index));
	entrenador->objetivo = sumarizar_pokemones(get_objetivos(config, index));
	entrenador->id = index;

	return entrenador;
}



t_list* get_entrenadores(t_config* config, int32_t cantidadEntrenadores){
	t_list* entrenadores_list = list_create();

	int32_t i;
	for(i=0 ; i < cantidadEntrenadores; i++){

		//cada entrenador es un hilo
		t_entrenador* entrenador = malloc(sizeof(t_entrenador));
		entrenador = get_entrenador(config, i);

		list_add_in_index(entrenadores_list, i, entrenador);

	}

	return entrenadores_list;

}

int32_t get_distancia_entre_puntos(t_posicion pos1, t_posicion pos2){
	//raiz de (x2-x1)^2 + (y2-y1)^2

	int32_t distX = pos2.X - pos1.X;
	int32_t distY = pos2.Y - pos1.Y;

	return sqrt(pow(distX,2) + pow(distY,2));

}



t_list* get_objetivo_global(t_list* entrenadores){
	t_list* objetivo_global = list_create();
	int i, j;

    for(i = 0; i < entrenadores->elements_count; i++){
		t_entrenador* entrenador_actual = list_get(entrenadores, i);

		//ver si se puede reemplazar por iterate
		for(j = 0; j < entrenador_actual->objetivo->elements_count; j++){
			t_pokemon_team* objetivo_actual = list_get(entrenador_actual->objetivo, j);
			list_add(objetivo_global, objetivo_actual);
		}
	}

	objetivo_global = sumarizar_pokemones(objetivo_global);

	return objetivo_global;
}

t_list* get_pokemones_capturados(t_list* entrenadores){

	t_list* pokemones_capturados = list_create();

    for(int i = 0; i < entrenadores->elements_count; i++){
		t_entrenador* entrenador_actual = list_get(entrenadores, i);

		for(int j = 0; j < entrenador_actual->pokemones->elements_count; j++){
			t_pokemon_team* pokemon_actual = list_get(entrenador_actual->pokemones, j);

			list_add(pokemones_capturados, pokemon_actual);
		}
	}

    if(pokemones_capturados->elements_count > 0){
    	pokemones_capturados = sumarizar_pokemones(pokemones_capturados);
    }


	return pokemones_capturados;
}


t_entrenador* get_entrenador_planificable_mas_cercano(t_list* entrenadores, t_posicion posicion_pokemon){

	t_entrenador* entrenador_cercano = malloc(sizeof(t_entrenador));
	t_entrenador* entrenador= malloc(sizeof(t_entrenador));
	int32_t distancia_mas_chica = -1;

	for(int32_t i = 0; i < entrenadores->elements_count; i++){

		entrenador = list_get(entrenadores, i);

		if((entrenador->estado == BLOCKED || entrenador->estado == NEW) && (puede_capturar_pokemones(entrenador))){
			int32_t distancia_entrenador = get_distancia_entre_puntos(entrenador->posicion, posicion_pokemon);

			if(distancia_mas_chica == -1){
				distancia_mas_chica = distancia_entrenador;
				entrenador_cercano= entrenador;
			} else if (distancia_entrenador < distancia_mas_chica) {
				distancia_mas_chica = distancia_entrenador;
				entrenador_cercano = entrenador;
			}
		}
	}

	if(distancia_mas_chica == -1) return NULL;

	return entrenador_cercano;
}


t_list* filtrar_localized_repetidos(t_list* mensajes_localized){

	//								**************************************
	// EN REALIDAD, PUEDEN LLEGAR REPETIDOS, HAY QUE COMPARAR SI SE PUEDE SACIAR O NO EL OBJETIVO GLOBAL
	//								**************************************

	t_list* mensajes_filtrados = list_create();
		for(int i = 0; i < mensajes_localized->elements_count; i++){
			t_Localized* mensaje = list_get(mensajes_localized, i);
			bool mensaje_encontrado = false;

			if(mensajes_filtrados->elements_count == 0){
				list_add(mensajes_filtrados, mensaje);
			} else {

				for(int j = 0; j < mensajes_filtrados->elements_count; j++){
					t_Localized* mensaje_filtrado = list_get(mensajes_filtrados, j);
					if(string_equals_ignore_case(mensaje_filtrado->pokemon.nombre, mensaje->pokemon.nombre)){
						mensaje_encontrado=true;
					}
				}

				if(!mensaje_encontrado){
					list_add(mensajes_filtrados, mensaje);
				}
			}
		}

		return mensajes_filtrados;
}

t_list* filtrar_localized_objetivo_global(t_list* mensajes_localized, t_list* objetivo_global){
	t_list* mensajes_filtrados = list_create();

	for(int i = 0; i < mensajes_localized->elements_count; i++){
		t_Localized* mensaje = list_get(mensajes_localized, i);

		for(int j = 0; j < objetivo_global->elements_count; j++){
			t_pokemon_team* objetivo_actual = list_get(objetivo_global, j);
			if(string_equals_ignore_case(mensaje->pokemon.nombre, objetivo_actual->nombre)){
				list_add(mensajes_filtrados, mensaje);
				break;
			}
		}
	}

	return mensajes_filtrados;
}

t_respuesta* get_respuesta(int32_t id, t_list* respuestas){
	for(int i = 0; i < respuestas->elements_count; i++){
		t_respuesta* respuesta = list_get(respuestas, i);
		if(id == respuesta->id_respuesta){
			list_remove(respuestas, i);
			return respuesta;
		}
	}

	printf("No es respuesta, lo voy a rechazar\n");
	return NULL;
}





int get_cantidad_by_nombre_pokemon(char* pokemon, t_list* objetivo_global){
	for(int j = 0; j < objetivo_global->elements_count; j++){
		t_pokemon_team* objetivo_actual = list_get(objetivo_global, j);

		if(string_equals_ignore_case(pokemon, objetivo_actual->nombre)){
			return objetivo_actual->cantidad;
		}
	}

	return 0;
};

bool esta_en_objetivos_globales(char* pokemon, t_list* objetivo_global){
	for(int i = 0; i < objetivo_global->elements_count; i++){
		t_pokemon_team* objetivo_actual = list_get(objetivo_global, i);

		if(string_equals_ignore_case(pokemon, objetivo_actual->nombre)){
			//printf("Está en objetivos globales\n");
			return true;
		}
	}
	printf("No está en objetivos globales, lo voy a rechazar\n");
	return false;
};


bool fue_recibido(char* pokemon, t_list* pokemones_recibidos){

	for(int i = 0; i < pokemones_recibidos->elements_count; i++){
		char* pokemon_actual = list_get(pokemones_recibidos, i);
		if(string_equals_ignore_case(pokemon, pokemon_actual)){
			printf("ya recibí un mensaje con el pokemon %s, lo voy a rechazar\n", pokemon);
			return true;
		}
	}

	//printf("nunca recibí un mensaje con este pokemon\n");
	return false;
};

bool puedo_capturar(char* pokemon, t_list* entrenadores, int necesito_capturar){

	t_list* pokemones_capturados = get_pokemones_capturados(entrenadores);

	for(int j = 0; j < pokemones_capturados->elements_count; j++){
		t_pokemon_team* pokemon_actual = list_get(pokemones_capturados, j);
		if(string_equals_ignore_case(pokemon, pokemon_actual->nombre)){
			//printf("tengo %d capturados\n", pokemon_actual->cantidad);
			return pokemon_actual->cantidad < necesito_capturar;
		}
	}

	return false;
};

bool appeared_valido(t_Appeared* mensaje, t_list* pokemones_recibidos, t_list* objetivo_global){

	bool en_objetivo = esta_en_objetivos_globales(mensaje->pokemon.nombre, objetivo_global);
	bool recibido = fue_recibido(mensaje->pokemon.nombre, pokemones_recibidos);

	return en_objetivo && !recibido;
}



bool localized_valido(t_Localized* mensaje, int id, t_list* gets_enviados, t_list* pokemones_recibidos, t_list* objetivo_global){

	//en realidad el filtro de id ya se hizo antes, lo dejo pa probar
	t_respuesta* respuesta = get_respuesta(id, gets_enviados);
	bool en_objetivo = esta_en_objetivos_globales(mensaje->pokemon.nombre, objetivo_global);
	bool recibido = fue_recibido(mensaje->pokemon.nombre, pokemones_recibidos);

	return (respuesta != NULL) && en_objetivo && !recibido;
}


int32_t conexion_broker()
{
	int32_t socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
	return socket;
}





