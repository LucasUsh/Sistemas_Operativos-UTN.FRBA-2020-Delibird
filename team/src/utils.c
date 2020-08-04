#include "utils.h"

char** get_array_by_index(char** array_from_config, int32_t index){

	 /* EJEMPLO CON POKEMONES
	  * array_from_config:
	  *  0 -> Pikachu|Squirtle|Pidgey
	  *  1 -> Squirtle|Charmander
	  *  2 -> Bulbasaur
	  */

	//value_string = Pikachu|Squirtle|Pidgey
	int array_l = array_length(array_from_config);

	if(index >= array_l ){
		return NULL;
	}

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

	char* stralpha= config_get_string_value(config, "ALPHA");
	double alpha;
	sscanf(stralpha, "%lf", &alpha);
	algoritmo.alpha = alpha;


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
	entrenador->ocupado = false;
	entrenador->semaforo = (sem_t*)malloc(sizeof(sem_t));
	entrenador->estimacion_anterior= 0;
	entrenador->ciclos = 0;

	sem_init(entrenador->semaforo, 0, 0);
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

	//return sqrt(pow(distX,2) + pow(distY,2));
	return abs(distX) + abs(distY);
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

t_list* get_pokemones_capturados_sumarizados_global(t_list* entrenadores){

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

t_list* get_pokemones_capturados_sumarizados_entrenador(t_entrenador* entrenador){

	t_list* pokemones_capturados = list_create();

	for(int j = 0; j < entrenador->pokemones->elements_count; j++){
		t_pokemon_team* pokemon_actual = list_get(entrenador->pokemones, j);

		list_add(pokemones_capturados, pokemon_actual);
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
		pthread_mutex_lock(&mutex_list_entrenadores);
		entrenador = list_get(entrenadores, i);
		pthread_mutex_unlock(&mutex_list_entrenadores);

		if((entrenador->estado == BLOCKED || entrenador->estado == NEW) && !entrenador->ocupado && (puede_capturar_pokemones(entrenador))){
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

	//free(entrenador);

	return entrenador_cercano;
}


bool alguien_yendo(t_pokemon_team* pokemon){

	/*
	 *
	 * if !pokemon.planificable return true
	 *
	 *
	 * */



	for(int i = 0; i < entrenadores->elements_count; i++){
		pthread_mutex_lock(&mutex_list_entrenadores);
		t_entrenador* e = list_get(entrenadores, i);
		pthread_mutex_unlock(&mutex_list_entrenadores);

		if(e->pokemon_destino != NULL &&
		   e->pokemon_destino->nombre == pokemon->nombre &&
		   e->pokemon_destino->posicion.X == pokemon->posicion.X &&
	       e->pokemon_destino->posicion.Y == pokemon->posicion.Y) return true;
	}

	return false;
}

int get_cantidad_entrenadores_yendo(char* pokemon){

	bool _yendo(void* entrenador){
		t_entrenador* e = (t_entrenador*)entrenador;
		return e->pokemon_destino != NULL &&
				e->pokemon_destino->nombre == pokemon;
	}

	pthread_mutex_lock(&mutex_list_entrenadores);
	t_list* filtrados = list_filter(entrenadores, _yendo);
	pthread_mutex_unlock(&mutex_list_entrenadores);

	return filtrados->elements_count;
}


bool pokemon_necesario(t_pokemon_team* pokemon){

	if(alguien_yendo(pokemon)) return false;

	t_list* pokemones_capturados = get_pokemones_capturados_sumarizados_global(entrenadores);
	int cantidad_capturados = get_cantidad_by_nombre_pokemon(pokemon->nombre, pokemones_capturados);
	int necesito_capturar = get_cantidad_by_nombre_pokemon(pokemon->nombre, objetivo_global);
	int estan_yendo_a_buscar = get_cantidad_entrenadores_yendo(pokemon->nombre);

	return (cantidad_capturados + estan_yendo_a_buscar) < necesito_capturar;
}

t_pokemon_team* get_pokemon_necesario_mas_cercano(t_list* pokemones_ubicados, t_posicion posicion_entrenador){

	t_pokemon_team* pokemon_cercano = malloc(sizeof(t_pokemon_team));
	int32_t distancia_mas_chica = -1;

	for(int i = 0; i < pokemones_ubicados->elements_count; i++){

		pthread_mutex_lock(&mutex_pokemones_ubicados);
		t_pokemon_team* pokemon = list_get(pokemones_ubicados, i);
		pthread_mutex_unlock(&mutex_pokemones_ubicados);

		if(pokemon_necesario(pokemon) && pokemon->planificable){
			int32_t distancia_pokemon = get_distancia_entre_puntos(posicion_entrenador, pokemon->posicion);

			if(distancia_mas_chica == -1){
				distancia_mas_chica = distancia_pokemon;
				pokemon_cercano= pokemon;
			} else if (distancia_pokemon < distancia_mas_chica) {
				distancia_mas_chica = distancia_pokemon;
				pokemon_cercano = pokemon;
			}
		}
	}

	if(distancia_mas_chica == -1) return NULL; // no hay pokemones ubicados

	return pokemon_cercano;
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

int get_cantidad_by_nombre_pokemon(char* pokemon, t_list* pokemones){
	for(int j = 0; j < pokemones->elements_count; j++){
		t_pokemon_team* objetivo_actual = list_get(pokemones, j);

		if(string_equals_ignore_case(pokemon, objetivo_actual->nombre)){
			return objetivo_actual->cantidad;
		}
	}

	return 0;
};

t_pokemon_team* get_pokemon_by_nombre(char* nombre_pokemon, t_list* pokemones){
	for(int j = 0; j < pokemones->elements_count; j++){
		t_pokemon_team* pokemon = list_get(pokemones, j);

		if(string_equals_ignore_case(nombre_pokemon, pokemon->nombre)){
			return pokemon;
		}
	}

	return NULL;
};

t_pokemon_team* remove_pokemon_by_nombre(char* nombre_pokemon, t_list* pokemones){
	for(int j = 0; j < pokemones->elements_count; j++){
		t_pokemon_team* pokemon = list_get(pokemones, j);

		if(string_equals_ignore_case(nombre_pokemon, pokemon->nombre)){

			if(pokemon->cantidad > 1){
				pokemon->cantidad--;
			} else {
				list_remove(pokemones, j);
			}

			return pokemon;
		}
	}

	return NULL;
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

bool puedo_capturar(char* pokemon, t_list* entrenadores, t_list* objetivo_global){

	t_list* pokemones_capturados = get_pokemones_capturados_sumarizados_global(entrenadores);
	int cantidad_capturados = get_cantidad_by_nombre_pokemon(pokemon, pokemones_capturados);
	int necesito_capturar = get_cantidad_by_nombre_pokemon(pokemon, objetivo_global);

	return cantidad_capturados < necesito_capturar;
};

bool appeared_valido(t_Appeared* mensaje, t_list* entrenadores, t_list* objetivo_global){

	bool en_objetivo = esta_en_objetivos_globales(mensaje->pokemon.nombre, objetivo_global);

	bool puedo = puedo_capturar(mensaje->pokemon.nombre, entrenadores, objetivo_global);

	return en_objetivo && puedo;
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


int32_t reconectar(int32_t socket){
	log_info(logger, "reintentado contectarse al Broker...");
	sleep(TIEMPO_RECONEXION);
	socket = crear_conexion(IP_BROKER,PUERTO_BROKER);
	return socket;
}

bool cumplio_objetivo(t_entrenador* entrenador){
	entrenador->pokemones = sumarizar_pokemones(entrenador->pokemones);
	for(int i = 0; i < entrenador->objetivo->elements_count; i++){
		t_pokemon_team* pokemon_actual = list_get(entrenador->objetivo, i);
		int cantidad_capturados = get_cantidad_by_nombre_pokemon(pokemon_actual->nombre, entrenador->pokemones);
		int cantidad_objetivo = pokemon_actual->cantidad;

		if(cantidad_objetivo != cantidad_capturados){
			 return false;
		}
	}

	return true;
}


t_posicion avanzar(t_posicion posicion, int32_t posX, int32_t posY){
	int32_t nuevaPosicionX = posicion.X + posX;
	int32_t nuevaPosicionY = posicion.Y + posY;

	posicion.X = nuevaPosicionX;
	posicion.Y = nuevaPosicionY;

	return posicion;
}


void show_semaforo(sem_t* semaforo){
	int i = 0;
	sem_getvalue(semaforo, &i);
	printf("El valor del semaforo es : %d\n", i);
	return;
}


t_list* pokemones_de_mas(t_entrenador* entrenador){
	t_list* pokemons_de_mas = list_create();
	for(int i = 0; i < entrenador->pokemones->elements_count ; i++){
		t_pokemon_team* pokemon = list_get(entrenador->pokemones, i);
		int cantidad_capturados = pokemon->cantidad;
		int cantidad_en_objetivos = get_cantidad_by_nombre_pokemon(pokemon->nombre, entrenador->objetivo);

		int de_mas = cantidad_capturados - cantidad_en_objetivos;

		if(de_mas > 0) list_add(pokemons_de_mas, pokemon);
	}

	return pokemons_de_mas;
}

t_list* objetivos_pendientes(t_entrenador* entrenador){
	t_list* objetivos_pendiente = list_create();
	for(int i = 0; i < entrenador->objetivo->elements_count ; i++){
		t_pokemon_team* objetivo = list_get(entrenador->objetivo, i);
		int cantidad_en_objetivos = objetivo->cantidad;
		int cantidad_capturados = get_cantidad_by_nombre_pokemon(objetivo->nombre, entrenador->pokemones);

		int pendientes = cantidad_en_objetivos - cantidad_capturados;

		if(pendientes > 0) list_add(objetivos_pendiente, objetivo);
	}

	return objetivos_pendiente;
}



/*
 * e2 tiene un pokemon que le sirve a e1
 * */
t_pokemon_team* pokemon_que_sirve(t_entrenador* e1, t_entrenador* e2){
	for(int i = 0; i < e1->objetivo->elements_count; i++){
		t_pokemon_team* objetivo_actual = list_get(e1->objetivo, i);

		t_pokemon_team* pokemon_sirve = get_pokemon_by_nombre(objetivo_actual->nombre, e2->pokemones);
		if (pokemon_sirve != NULL) return pokemon_sirve;

	}

	return NULL;
}

/*
 * e2 tiene un pokemon que le sirve a e1
 * */
t_pokemon_team* pokemon_que_sirve_intercambio(t_entrenador* e1, t_entrenador* e2){
	for(int i = 0; i < e1->objetivo->elements_count; i++){
		t_pokemon_team* objetivo_actual = list_get(e1->objetivo, i);

		//si mi objetivo está en los pokemones que le sobran al otro entrenador
		t_pokemon_team* pokemon_sirve = get_pokemon_by_nombre(objetivo_actual->nombre, pokemones_de_mas(e2));
		if (pokemon_sirve != NULL) return pokemon_sirve;

	}

	return NULL;
}

/*






t_Localized* generar_localized(char* pokemon, int cant_posiciones){
	t_list* listaPosiciones = list_create();
	t_Localized* mensaje = malloc(sizeof(t_Localized)); // reemplazar por funcion get size Localized de broker

	for(int i = 0; i < cant_posiciones; i++){
		t_posicion* posicion = malloc(sizeof(t_posicion));
		posicion->X = (rand() % (10)) + 1; // numero random entre 1 y 10
		posicion->Y = (rand() % (10)) + 1;

		list_add(listaPosiciones, posicion);
	}

	mensaje->listaPosiciones = listaPosiciones;
	mensaje->pokemon.nombre = pokemon;
	mensaje->pokemon.size_Nombre = string_length(pokemon); // creo que hay que sumar 1

	return mensaje;
}

t_Caught* generar_caught(){
	t_Caught* mensaje = malloc(sizeof(t_Caught)); // reemplazar por funcion get size caught de broker

	mensaje->fueAtrapado = (rand() % (1)) + 1; // numero random entre 1 y 0

	return mensaje;
}

t_list* simular_list_localized(int cant_mensajes){

	srand(time(NULL));
	t_list* mensajes_localized = list_create();
	t_list* nombre_pokemones = get_nombres_pokemon();


	for(int i = 0; i < cant_mensajes; i++){
		//el nombre lo obtengo de forma aleatoria
		char* nombre_pokemon = get_nombre_aleatorio(nombre_pokemones);
		printf("%s\n", nombre_pokemon);
		//con el nombre, genero tambien X cantidad de posiciones aleatorias
		t_Localized* mensaje_localized = generar_localized(nombre_pokemon, 1);

		list_add(mensajes_localized, mensaje_localized);
	}

	return mensajes_localized;

}

t_Localized* simular_localized(int cantidad_posiciones){

	srand(time(NULL));

	t_list* nombre_pokemones = get_nombres_pokemon();
	char* nombre_pokemon = get_nombre_aleatorio(nombre_pokemones);
	t_Localized* mensaje = generar_localized(nombre_pokemon, cantidad_posiciones);

	return mensaje;

}

*/
