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
#include "pokemon_utils.h"


int32_t cantidadEntrenadores = 0;
t_list* cola_ready;
//t_list* mensajes_localized_parseados;



int32_t get_cantidad_pokemon(t_list* list_pokemones){

	int32_t total_pokemon = 0 ;

	int32_t _sumar_pokemon(void* element){
		total_pokemon += ((t_pokemon_team*)element)->cantidad;
		return total_pokemon;
	}

	list_iterate(list_pokemones, (void*)_sumar_pokemon);

	return total_pokemon;
}

bool puede_capturar_pokemones(t_entrenador* entrenador){
	return get_cantidad_pokemon(entrenador->pokemones) < get_cantidad_pokemon(entrenador->objetivo);
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

t_algoritmo* get_algoritmo(t_config* config)
{
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

t_posicion* get_posicion(t_config* config, int32_t index){
	t_posicion* posicion = (t_posicion*)malloc(sizeof(t_posicion));

	char** posiciones = get_array_by_index(config_get_array_value(config, "POSICIONES_ENTRENADORES"),
			index);

	posicion->X = atoi(posiciones[0]);
	posicion->Y = atoi(posiciones[1]);


	return posicion;
}

t_list* sumarizar_pokemones(t_list* lista_pokemones_sin_sumarizar){
	t_list* lista_sumarizada = list_create();
	for(int32_t i = 0; i < lista_pokemones_sin_sumarizar->elements_count; i++){
		t_pokemon_team* pokemon = list_get(lista_pokemones_sin_sumarizar, i);
		int32_t pokemon_encontrado = 0;

		if(lista_sumarizada->elements_count == 0){
			list_add(lista_sumarizada, pokemon);
		} else {

			for(int32_t j = 0; j < lista_sumarizada->elements_count; j++){
				t_pokemon_team* pokemon_sumarizado = list_get(lista_sumarizada, j);

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

t_entrenador* get_entrenador(t_config* config, int32_t index){
	t_entrenador* entrenador = (t_entrenador*)malloc(sizeof(t_entrenador));

	entrenador->estado = NEW;
	entrenador->posicion = get_posicion(config, index);

	entrenador->pokemones = sumarizar_pokemones(get_pokemones(config, index));
	entrenador->objetivo = sumarizar_pokemones(get_objetivos(config, index));

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

t_entrenador* get_entrenador_mas_cercano(t_list* entrenadores, t_posicion* posicion_pokemon){

	t_entrenador* entrenador_cercano = malloc(sizeof(t_entrenador));
	t_entrenador* entrenador= malloc(sizeof(t_entrenador));
	int32_t distancia_mas_chica = -1;

	for(int32_t i = 0; i < entrenadores->elements_count; i++){

		entrenador = list_get(entrenadores, i);

		if(entrenador->estado == BLOCKED || entrenador->estado == NEW ){
			int32_t distancia_entrenador = get_distancia_entre_puntos(*entrenador->posicion, *posicion_pokemon);

			if(distancia_mas_chica == -1){
				distancia_mas_chica = distancia_entrenador;
				entrenador_cercano= entrenador;
			} else if (distancia_entrenador < distancia_mas_chica) {
				distancia_mas_chica = distancia_entrenador;
				entrenador_cercano = entrenador;
			}
		}
	}
	return entrenador_cercano;
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

t_posicion* avanzar(t_posicion* posicion, int32_t posX, int32_t posY){
	int32_t nuevaPosicionX = posicion->X + posX;
	int32_t nuevaPosicionY = posicion->Y + posY;

	posicion->X = nuevaPosicionX;
	posicion->Y = nuevaPosicionY;

	return posicion;
}
void show_entrenadores(t_algoritmo* algoritmo, t_list* entrenadores, t_list* objetivo_global){
	printf("PLANIFICACION: \n	ALGORITMO: %s, QUANTUM: %d\n", algoritmo->algoritmo_string, algoritmo->quantum);
	    printf("********************\n");


	    void _mostrar_pokemon(void* elemento){
	    	return mostrar_pokemon((t_pokemon_team*)elemento, objetivo_global);
	    }


	    for(int32_t i = 0; i < entrenadores->elements_count; i++){
	    	t_entrenador* entrenador_actual = list_get(entrenadores, i);
	    	printf("|ENTRENADOR %d\n|----------------\n|POSICION: (%d,%d)\n",
	    			i + 1,
					entrenador_actual->posicion->X,
					entrenador_actual->posicion->Y);


	    	printf("|POKEMONES:\n");
	    	list_iterate(entrenador_actual->pokemones, _mostrar_pokemon);

	    	printf("|OBJETIVOS:\n");
	    	list_iterate(entrenador_actual->objetivo, _mostrar_pokemon);


			printf("********************\n");

		}

		printf("|OBJETIVO GLOBAL:\n");
		list_iterate(objetivo_global, _mostrar_pokemon);

		printf("********************\n");
}

void generar_y_enviar_get(t_list* objetivo_global){
	int i=0;
	for(i = 0; i < objetivo_global->elements_count; i++){
		t_pokemon_team* pokemon_actual = list_get(objetivo_global, i);

		t_Get mensaje_get;
		mensaje_get.pokemon.nombre = pokemon_actual->nombre;
		mensaje_get.pokemon.size_Nombre = string_length(pokemon_actual->nombre); // +1?

		printf("mandando el mensaje GET %s\n", mensaje_get.pokemon.nombre);
		//conectarse al broker y mandar el mensaje;
	}

	return;
}


t_Localized* generar_localized(char* pokemon){
	t_list* listaPosiciones = list_create();
	t_Localized* mensaje = malloc(sizeof(t_Localized)); // reemplazar por funcion get size Localized
	int cantPosiciones =  (rand() % (3)) + 1; //nro random entre 1y 3
	int i = 0;

	for(i = 0; i < cantPosiciones; i++){
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

t_list* get_nombres_pokemon(){
	t_list* nombre_pokemones = list_create();

	list_add(nombre_pokemones, "Charmander");
	list_add(nombre_pokemones, "Pikachu");
	list_add(nombre_pokemones, "Squirtle");
	list_add(nombre_pokemones, "Pidgey");
	list_add(nombre_pokemones, "Rattata");
	list_add(nombre_pokemones, "Bulbasaur");

	return nombre_pokemones;

}

char* get_nombre_aleatorio(t_list* nombre_pokemones){
	int i =  (rand() % (nombre_pokemones->elements_count - 1));
	return list_get(nombre_pokemones, i);
}

t_list* simular_localized(){
	/*
	 * El objetivo de esta funcion es generar mensajes localized de forma aleatoria
	 * primero se agarran 3 nombres de pokemon random de la lista nombres_pokemon
	 * */
	srand(time(NULL));
	t_list* mensajes_localized = list_create();
	int cantidad_pokemon = (rand() % (7)) + 1; //nro random entre 1 y 3 para tener pocos mensajes
	t_list* nombre_pokemones = get_nombres_pokemon();

	//por ej: si cantidad_pokemon = 2, genero 2 mensajes.

	int i;
	for(i = 0; i < cantidad_pokemon; i++){
		//el nombre lo obtengo de forma aleatoria
		char* nombre_pokemon = get_nombre_aleatorio(nombre_pokemones);

		//con el nombre, genero tambien X cantidad de posiciones aleatorias
		t_Localized* mensaje_localized = generar_localized(nombre_pokemon);

		list_add(mensajes_localized, mensaje_localized);
	}

	return mensajes_localized;

}


t_list* filtrar_localized_repetidos(t_list* mensajes_localized){
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
	printf("%d\n", objetivo_global->elements_count);
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


int32_t main(int32_t argc, char** argv)
{
	cola_ready = list_create();
	t_list* objetivo_global = list_create();


    printf("el entrenador que se va a cargar es el de la config: %s\n", argv[1] );
    //char* config_name = argv[1];

    //t_config* entrenador_config = config_create(argv[1]);
    t_config* entrenador_config = config_create("/home/utnso/workspace/tp-2020-1c-5rona/team/config/entrenador1.config");


    int32_t cantidadEntrenadores = array_length(config_get_array_value(entrenador_config, "POKEMON_ENTRENADORES"));
    printf("En este team hay %d entrenadores\n", cantidadEntrenadores);

    t_algoritmo* algoritmo = get_algoritmo(entrenador_config);
    t_list* entrenadores = get_entrenadores(entrenador_config, cantidadEntrenadores);


    objetivo_global = get_objetivo_global(entrenadores);
    //show_entrenadores(algoritmo, entrenadores, objetivo_global);


    generar_y_enviar_get(objetivo_global);

    t_list* mensajes_localized_parseados = simular_localized();

    t_list* mensajes_localized_filtrados = filtrar_localized_objetivo_global(filtrar_localized_repetidos(mensajes_localized_parseados), objetivo_global);

    printf("***************************************** LLEGARON LOS SIGUIENTES MENSAJES LOCALIZED \n");

    for(int j = 0; j < mensajes_localized_parseados->elements_count ; j++){
    	t_Localized* mensaje = list_get(mensajes_localized_parseados, j);
    	printf("pokemon: %s\n", mensaje->pokemon.nombre);
    	printf("cant de posiciones: %d\n", mensaje->listaPosiciones->elements_count);

    	for(int m=0; m < mensaje->listaPosiciones->elements_count; m++){
    		t_posicion* posicion = list_get(mensaje->listaPosiciones, m);
    		printf("Pos %d:\n", m);
    		printf("posX: %d\n", posicion->X);
    		printf("posY: %d\n", posicion->Y);
    	}

    	printf("*****************************************\n");

    }


    printf("***************************************** SE FILTRARON LOS SIGUIENTES MENSAJES LOCALIZED \n");

    for(int j = 0; j < mensajes_localized_filtrados->elements_count ; j++){
    	t_Localized* mensaje_filtrado = list_get(mensajes_localized_filtrados, j);
    	printf("pokemon: %s\n", mensaje_filtrado->pokemon.nombre);
    	printf("cant de posiciones: %d\n", mensaje_filtrado->listaPosiciones->elements_count);

    	for(int m=0; m < mensaje_filtrado->listaPosiciones->elements_count; m++){
    		t_posicion* posicion = list_get(mensaje_filtrado->listaPosiciones, m);
    		printf("Pos %d:\n", m);
    		printf("posX: %d\n", posicion->X);
    		printf("posY: %d\n", posicion->Y);
    	}

    	printf("*****************************************\n");

    }

    printf("End");

	return EXIT_SUCCESS;
}



void planificar_fifo(){

	// en fifo, el proximo entrenador es el que esté primero en la cola de ready
	t_entrenador* entrenador = list_remove(cola_ready, 0);
	entrenador->estado = EXEC;

	int32_t posicion_final_X = entrenador->posicion_destino->X - entrenador->posicion->X;
	int32_t posicion_final_Y = entrenador->posicion_destino->Y - entrenador->posicion->Y;

	printf("posicion vieja: x-> %d, y-> %d\n", entrenador->posicion->X, entrenador->posicion->Y);

	entrenador->posicion = avanzar(entrenador->posicion, posicion_final_X , posicion_final_Y);

	printf("posicion nueva: x-> %d, y-> %d\n", entrenador->posicion->X, entrenador->posicion->Y);

	//una vez que lo muevo llamo al broker y hago el catch

	return;
}


void simular_fifo(t_list* entrenadores)
{
	printf("simulando fifo... \n");
	t_posicion* posPok = (t_posicion*)malloc(sizeof(t_posicion));

	posPok->X = 3;
	posPok->Y = 5;

	t_entrenador* entrenador_mas_cercano = get_entrenador_mas_cercano(entrenadores, posPok);

	entrenador_mas_cercano->estado = READY;
	entrenador_mas_cercano->posicion_destino = posPok;

	list_add(cola_ready, entrenador_mas_cercano);


	printf("elementos en ready: %d\n", cola_ready->elements_count);

	planificar_fifo();

	printf("elementos en ready: %d\n", cola_ready->elements_count);

}


void planificar_rr(t_pokemon_team* pokemon_a_capturar){
	// en fifo, el proximo entrenador es el que esté primero en la cola de ready
	t_entrenador* entrenador = list_remove(cola_ready, 0);
	entrenador->estado = EXEC;

	int32_t posicion_final_X = entrenador->posicion_destino->X - entrenador->posicion->X;
	int32_t posicion_final_Y = entrenador->posicion_destino->Y - entrenador->posicion->Y;

	printf("posicion vieja: x-> %d, y-> %d\n", entrenador->posicion->X, entrenador->posicion->Y);

	entrenador->posicion = avanzar(entrenador->posicion, posicion_final_X , posicion_final_Y);

	printf("posicion nueva: x-> %d, y-> %d\n", entrenador->posicion->X, entrenador->posicion->Y);

	//una vez que lo muevo llamo al broker y hago el catch

	return;
}

void planificar_sjfsd(t_pokemon_team* pokemon_a_capturar){
	return;
}

void planificar_sjfcd(t_pokemon_team* pokemon_a_capturar){
	return;
}

void planificar(t_algoritmo* algoritmo, t_pokemon_team* pokemon_a_capturar)
{
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
