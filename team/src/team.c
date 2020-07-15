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

//constantes una vez que se inicialicen
int32_t cantidad_entrenadores = 0;
t_list* objetivo_global;
t_algoritmo algoritmo;

//colas de planificacion
t_list* cola_ready;

//semaforos
sem_t s_cola_ready_con_items;

//colas de mensajes
t_list* mensajes_posiciones_planificables; // podría ser pokemones en el mapa
t_list* mensajes_get_esperando_respuesta; // seguramente algun id o algo
t_list* pokemones_en_mapa;

t_posicion avanzar(t_posicion posicion, int32_t posX, int32_t posY){
	int32_t nuevaPosicionX = posicion.X + posX;
	int32_t nuevaPosicionY = posicion.Y + posY;

	posicion.X = nuevaPosicionX;
	posicion.Y = nuevaPosicionY;

	return posicion;
}

void actualizar_estado(t_entrenador* entrenador, estado_code estado){

}

void planificar_fifo(){

	printf("planificando FIFO...\n");
	while(cola_ready->elements_count > 0){
		printf("proximo entrenador..\n");

		// en fifo, el proximo entrenador es el que esté primero en la cola de ready
		t_entrenador* entrenador = list_remove(cola_ready, 0);
		entrenador->estado = EXEC;

		int32_t posicion_final_X = entrenador->posicion_destino.X - entrenador->posicion.X;
		int32_t posicion_final_Y = entrenador->posicion_destino.Y - entrenador->posicion.Y;

		printf("posicion vieja: x-> %d, y-> %d\n", entrenador->posicion.X, entrenador->posicion.Y);

		entrenador->posicion = avanzar(entrenador->posicion, posicion_final_X , posicion_final_Y);

		printf("posicion nueva: x-> %d, y-> %d\n", entrenador->posicion.X, entrenador->posicion.Y);

		entrenador->estado = BLOCKED;
		//una vez que lo muevo llamo al broker y hago el catch

	}

	return;
}

void planificar_rr(t_list* entrenadores, int32_t quantum){
	// en fifo, el proximo entrenador es el que esté primero en la cola de ready
	printf("planificando RR...\n");
	while(cola_ready->elements_count > 0){
	t_entrenador* entrenador = list_remove(cola_ready, 0);
	entrenador->estado = EXEC;

	//no se q paso pero hay que hacer RR de nuevo :c

	int32_t posicion_final_X = entrenador->posicion_destino.X - entrenador->posicion.X;
	int32_t posicion_final_Y = entrenador->posicion_destino.Y - entrenador->posicion.Y;

	printf("posicion vieja: x-> %d, y-> %d\n", entrenador->posicion.X, entrenador->posicion.Y);

	entrenador->posicion = avanzar(entrenador->posicion, posicion_final_X , posicion_final_Y);

	printf("posicion nueva: x-> %d, y-> %d\n", entrenador->posicion.X, entrenador->posicion.Y);

	entrenador->estado = BLOCKED;


	//una vez que lo muevo llamo al broker y hago el catch
	}

	return;
}

void planificar_sjfsd(){
	return;
}

void planificar_sjfcd(){
	return;
}

void planificar(t_algoritmo algoritmo, t_list* entrenadores)
{

	switch(algoritmo.algoritmo_code){
	case FIFO:
		planificar_fifo();
		break;
	case RR:
		planificar_rr(entrenadores, algoritmo.quantum);
		break;
	case SJFSD:
		planificar_sjfsd();
		break;
	case SJFCD:
		planificar_sjfcd();
		break;
	default:
		return;
	}

}

void show_entrenadores(t_algoritmo algoritmo, t_list* entrenadores, t_list* objetivo_global){
	printf("PLANIFICACION: \n	ALGORITMO: %s, QUANTUM: %d\n", algoritmo.algoritmo_string, algoritmo.quantum);
	    printf("********************\n");


	    void _mostrar_pokemon(void* elemento){
	    	return mostrar_pokemon((t_pokemon_team*)elemento, objetivo_global);
	    }


	    for(int32_t i = 0; i < entrenadores->elements_count; i++){
	    	t_entrenador* entrenador_actual = list_get(entrenadores, i);
	    	printf("|ENTRENADOR %d\n|----------------\n|POSICION: (%d,%d)\n",
	    			i + 1,
					entrenador_actual->posicion.X,
					entrenador_actual->posicion.Y);


	    	printf("|POKEMONES:\n");
	    	list_iterate(entrenador_actual->pokemones, _mostrar_pokemon);

	    	printf("|OBJETIVOS:\n");
	    	list_iterate(entrenador_actual->objetivo, _mostrar_pokemon);


			printf("********************\n");

		}

		printf("|OBJETIVO GLOBAL:\n");
		list_iterate(objetivo_global, _mostrar_pokemon);

/*
		t_posicion* posicion;
		posicion->X = 4;
		posicion->Y = 4;
*/
		t_posicion posicion;
			posicion.X = 4;
			posicion.Y = 4;


		t_entrenador* entrenador_mas_cercano = get_entrenador_planificable_mas_cercano(entrenadores, posicion);

		printf("XXXXXXXXxx: %d", entrenador_mas_cercano->posicion.X);
		printf("XXXXXXXXxx: %d", entrenador_mas_cercano->posicion.Y);

		printf("********************\n");
}

void generar_y_enviar_get(t_list* objetivo_global){
	int i=0;
	int id = (rand() % (10)) + 1; // numero random entre 1 y 10

	for(i = 0; i < objetivo_global->elements_count; i++){
		t_pokemon_team* pokemon_actual = list_get(objetivo_global, i);

		t_Get mensaje_get;
		mensaje_get.pokemon.nombre = pokemon_actual->nombre;
		mensaje_get.pokemon.size_Nombre = string_length(pokemon_actual->nombre); // +1?

		printf("mandando el mensaje GET %s...\n", mensaje_get.pokemon.nombre);
		//conectarse al broker y mandar el mensaje;

		id++;
		printf("Recibí el ID: %d\n", id);
		list_add(mensajes_get_esperando_respuesta, id);

	}

	printf("Hay %d mensajes esperando respuesta\n", mensajes_get_esperando_respuesta->elements_count);

	return;
}

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

t_Appeared* generar_appeared(char* pokemon){
	t_Appeared* mensaje = malloc(sizeof(t_Appeared)); // reemplazar por funcion get size Localized de broker

	t_posicion posicion;
	posicion.X = (rand() % (10)) + 1; // numero random entre 1 y 10
	posicion.Y = (rand() % (10)) + 1;

	mensaje-> posicion = posicion;
	mensaje->pokemon.nombre = pokemon;
	mensaje->pokemon.size_Nombre = string_length(pokemon); // creo que hay que sumar 1

	return mensaje;
}

t_list* simular_list_localized(int cant_mensajes){
	/*
	 * El objetivo de esta funcion es generar mensajes localized de forma aleatoria
	 * */
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
	/*
	 * El objetivo de esta funcion es generar un mensaje localized de forma aleatoria
	 * */

	srand(time(NULL));

	t_list* nombre_pokemones = get_nombres_pokemon();
	char* nombre_pokemon = get_nombre_aleatorio(nombre_pokemones);
	t_Localized* mensaje = generar_localized(nombre_pokemon, cantidad_posiciones);

	return mensaje;

}

t_Appeared* simular_appeared(){
	/*
	 * El objetivo de esta funcion es generar un mensaje localized de forma aleatoria
	 * */

	srand(time(NULL));

	t_list* nombre_pokemones = get_nombres_pokemon();
	char* nombre_pokemon = get_nombre_aleatorio(nombre_pokemones);
	t_Appeared* mensaje = generar_appeared(nombre_pokemon);

	return mensaje;

}

void show_cola_ready(){
	for(int i = 0; i < cola_ready->elements_count; i++){
		t_entrenador* entrenador_actual = list_get(cola_ready, i);

		printf("posicion en la lista: %d\n", i);
		printf("posicion actual: X:%d, Y:%d\n", entrenador_actual->posicion.X, entrenador_actual->posicion.Y);
		printf("posicion destino: X:%d, Y:%d\n", entrenador_actual->posicion_destino.X, entrenador_actual->posicion_destino.Y);
	}
}

void hilo_planificador(void* l_entrenadores){

	while(1){
		printf("esperando que llegue algo a la cola de ready....\n");
		sem_wait(&s_cola_ready_con_items);
		printf("arrancó el planificador: %s (%d)\n", algoritmo.algoritmo_string, algoritmo.algoritmo_code);
		planificar(algoritmo, (t_list*)l_entrenadores);
	}
}


void ubicar_pokemones(t_Localized* pokemones_a_ubicar){
	for(int i = 0; i < pokemones_a_ubicar->listaPosiciones->elements_count; i++){
		t_pokemon_team* pokemon_ubicado = malloc(sizeof(t_pokemon));
		t_posicion* posicion = list_get(pokemones_a_ubicar->listaPosiciones, i);

		pokemon_ubicado->cantidad = 1;
		pokemon_ubicado->posicion = *posicion;
		pokemon_ubicado->nombre = pokemones_a_ubicar->pokemon.nombre;

		list_add(pokemones_en_mapa, pokemon_ubicado);
	}
}

void hilo_recibidor_mensajes_localized(void* l_entrenadores){
	t_list* entrenadores = (t_list*)l_entrenadores;

	while(1){

		// esto simula que recibí un mensaje localized
		t_Localized* mensaje = simular_localized(1); // puede haber mas de 1
		int id = (rand() % (15)) + 1; // supongo que llega con un id random, el if debe estar donde lo recibo posta

		printf("llego el mensaje con id: %d\n", id);

		//filtro los mensajes que son una respuesta a un GET
		if(es_respuesta(id, mensajes_get_esperando_respuesta)){
			printf("Es respuesta\n");

			// No sé si debería comparar todas las posiciones, con todos los entrenadores
			//y obtener el de distancia mas corta entre todas las posiciones y todos los entrenadores.
			t_posicion* posicion = list_get(mensaje->listaPosiciones, 0);


			//por ahora vamos con la primera
			t_entrenador* entrenador_mas_cercano = get_entrenador_planificable_mas_cercano(entrenadores, *posicion);

			if(entrenador_mas_cercano != NULL){

				//Si hay entrenadores planificables, planifico esa posicion
				list_remove(mensaje->listaPosiciones, 0);


				//agrego a mi mapa todas las otras posiciones
				if(mensaje->listaPosiciones > 0){
					ubicar_pokemones(mensaje);
				}

				entrenador_mas_cercano->estado = READY;
				entrenador_mas_cercano->posicion_destino = *posicion;

				list_add(cola_ready, entrenador_mas_cercano);
				sem_post(&s_cola_ready_con_items);
			} else {
				ubicar_pokemones(mensaje);
			}
		}
		sleep(5);
	}
}



void hilo_recibidor_mensajes_appeared(void* l_entrenadores){
	t_list* entrenadores = (t_list*)l_entrenadores;

	while(1){

		t_Appeared* mensaje = simular_appeared();
		printf("se generó un mensaje: %s\n", mensaje->pokemon.nombre);
		printf("***************************************\n");
		//deberia filtrar el mensaje por especie que necesito y si todavia quedan pendientes de capturar

		if(filtrar_appeared(mensaje, entrenadores, objetivo_global) != NULL){
			printf("Appeared que sirve\n");

			t_entrenador* entrenador_mas_cercano = get_entrenador_planificable_mas_cercano(entrenadores, mensaje->posicion);

			if(entrenador_mas_cercano != NULL){
				entrenador_mas_cercano->estado = READY;
				entrenador_mas_cercano->posicion_destino = mensaje->posicion;

				list_add(cola_ready, entrenador_mas_cercano);
				sem_post(&s_cola_ready_con_items);
			} else {
				t_pokemon_team* pokemon_ubicado = malloc(sizeof(t_pokemon_team));
				pokemon_ubicado->cantidad = 1;
				pokemon_ubicado->nombre = mensaje->pokemon.nombre;
				pokemon_ubicado->posicion = mensaje->posicion;

				list_add(pokemones_en_mapa, pokemon_ubicado);
			}
		}

		printf("***************************************\n");

		sleep(5);

	}
}

void hilo_recibidor_mensajes_full(void* l_entrenadores, t_paquete* paquete){
	while(1){
		switch(paquete->codigo_operacion){
		case APPEARED_POKEMON:
			printf("Recibí un APPEARED\n");
			break;
		case LOCALIZED_POKEMON:
			printf("Recibí un LOCALIZED\n");
			if(es_respuesta(paquete->buffer->id_Mensaje, mensajes_get_esperando_respuesta)){
				hilo_recibidor_mensajes_localized(l_entrenadores); // cambiar nombre a la funcion
			}
			break;
		case CAUGHT_POKEMON:
			printf("Recibí un CAUGHT\n");
			break;

		default:
			printf("no me interesa el mensaje");
			return;
		}
	}
}

int32_t main(int32_t argc, char** argv)
{
	cola_ready = list_create();
	objetivo_global = list_create();
	mensajes_posiciones_planificables = list_create();
	mensajes_get_esperando_respuesta = list_create();
	pokemones_en_mapa = list_create();
	sem_init(&s_cola_ready_con_items, 0, 0);
	srand(time(NULL));
    printf("el entrenador que se va a cargar es el de la config: %s\n", argv[1] );
    //char* config_name = argv[1];

    //t_config* entrenador_config = config_create(argv[1]);
    t_config* entrenador_config = config_create("/home/utnso/workspace/tp-2020-1c-5rona/team/config/entrenador1.config");


    int32_t cantidad_entrenadores = array_length(config_get_array_value(entrenador_config, "POKEMON_ENTRENADORES"));


    algoritmo = get_algoritmo(entrenador_config);
    t_list* entrenadores = get_entrenadores(entrenador_config, cantidad_entrenadores);

    printf("En este team hay %d entrenadores\n", cantidad_entrenadores);


    objetivo_global = get_objetivo_global(entrenadores);

    generar_y_enviar_get(objetivo_global);


    pthread_t p_generador_mensajes;
    pthread_create(&p_generador_mensajes, NULL, (void*)hilo_recibidor_mensajes_appeared, (void*)entrenadores);

    pthread_t p_planificador;
	pthread_create(&p_planificador, NULL, (void*)hilo_planificador, (void*)entrenadores);


    while(1){
    }

    /*crear un hilo por entrenador*/

    printf("End");

    return EXIT_SUCCESS;
}

