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
int32_t socket_escucha_team;


//colas de planificacion
t_list* cola_ready;
t_list* entrenadores;

//semaforos
sem_t s_cola_ready_con_items;
sem_t s_procesos_en_exec;
sem_t s_reconnect_broker;

//colas de mensajes
t_list* pokemones_recibidos; // registro para saber si rechazar appeareds y localizeds
t_list* pokemones_ubicados; // estos son los pokemones capturables. Esta lista varía con el tiempo.
t_list* mensajes_get_esperando_respuesta; // seguramente algun id o algo
t_list* mensajes_catch_esperando_respuesta; // seguramente algun id o algo

t_posicion avanzar(t_posicion posicion, int32_t posX, int32_t posY){
	int32_t nuevaPosicionX = posicion.X + posX;
	int32_t nuevaPosicionY = posicion.Y + posY;

	posicion.X = nuevaPosicionX;
	posicion.Y = nuevaPosicionY;

	return posicion;
}

bool generar_y_enviar_catch(t_entrenador* entrenador){

	int32_t socket = conexion_broker();

	if(socket == 0){
		log_error(logger,"Error al conectar al Broker...");
		//logica de reintentar
		return false;
	}

	int32_t operacion = 0;
	int32_t id_mensaje = 0; // esto creo que habria que cambiarlo
	int32_t tamanio_estructura = 0;

	log_debug(logger,"Conectado al Broker para enviar CATCH");
	enviar_handshake(PROCESS_ID, socket);
	if(recv(socket, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
		if(operacion == ACK){ // Confirmacion de que la identificacion (handshake) fue recibida
			recv(socket, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
			recv(socket, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
			printf("El broker me reconocio\n");

			char* pokemon = entrenador->pokemon_destino->nombre;
			char* posX = string_itoa(entrenador->pokemon_destino->posicion.X);
			char* posY = string_itoa(entrenador->pokemon_destino->posicion.Y);

			enviar_catch_pokemon(pokemon, posX, posY, string_itoa(0), socket);

			if(recv(socket, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){// Esperamos confirmacion de recepcion del mensaje
				if(operacion == ACK){
					recv(socket, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
					recv(socket, &id_mensaje, sizeof(int32_t), MSG_WAITALL); //recibo el paquete, aca llega el id_mensaje asignado por Broker

					t_respuesta* respuesta = malloc(sizeof(respuesta));
					respuesta->id_entrenador = entrenador->id;
					respuesta->id_respuesta = id_mensaje;
					list_add(mensajes_catch_esperando_respuesta, respuesta);
				}
			}
		}
	}

	return true;
}

void planificar_fifo(){

	printf("planificando FIFO...\n");

	// en fifo, el proximo entrenador es el que esté primero en la cola de ready
	t_entrenador* entrenador = list_remove(cola_ready, 0);

	int cantidad_a_moverse = get_distancia_entre_puntos(entrenador->posicion, entrenador->pokemon_destino->posicion);
	printf("voy a ejecutar %d veces!", cantidad_a_moverse);
	for(int i = 0; i < cantidad_a_moverse + 1; i++){
		sem_post(entrenador->semaforo);
	}


/*
	int32_t posicion_final_X = entrenador->pokemon_destino->posicion.X - entrenador->posicion.X;
	int32_t posicion_final_Y = entrenador->pokemon_destino->posicion.Y - entrenador->posicion.Y;

	printf("posicion vieja: x-> %d, y-> %d\n", entrenador->posicion.X, entrenador->posicion.Y);

	entrenador->posicion = avanzar(entrenador->posicion, posicion_final_X , posicion_final_Y);

	printf("posicion nueva: x-> %d, y-> %d\n", entrenador->posicion.X, entrenador->posicion.Y);

	entrenador->ocupado = true;
	entrenador->estado = BLOCKED; // blockeado hasta que reciba una respuesta al catch

	if(!generar_y_enviar_catch(entrenador)){//esto quiere decir que no se pudo conectar al broker
		list_add(entrenador->pokemones, entrenador->pokemon_destino);
		entrenador->pokemon_destino = NULL;
		log_info(logger, "POKEMON CAPTURADO!\n");

		if(cumplio_objetivo(entrenador)){
			log_info(logger, "el entrenador cumplio sus objetivos, pasandolo a EXIT\n");
			entrenador->estado = EXIT;
			entrenador->ocupado = false;
		} else {
			log_info(logger, "el entrenador aún no cumplio sus objetivos, pasandolo a BLOCKED\n");
			entrenador->estado = BLOCKED;
			entrenador->ocupado=false;

			if(puede_capturar_pokemones(entrenador)){
				printf("el entrenador puede capturar mas pokemones\n");
			} else {
				printf("el entrenador no puede capturar mas pokemones\n");
			}
		}
	}

*/

	return;
}

void planificar_rr(){

	printf("planificando RR...\n");
	t_entrenador* entrenador = list_remove(cola_ready, 0);
	entrenador->estado = EXEC;
	printf("TRAINER %d IS MOVING TO CATCH A %s\n", entrenador->id, entrenador->pokemon_destino->nombre);


	printf("posicion inicial X: %d\n", entrenador->posicion.X);
	printf("posicion inicial Y: %d\n", entrenador->posicion.Y);

	printf("posicion final X: %d\n", entrenador->pokemon_destino->posicion.X);
	printf("posicion final Y: %d\n", entrenador->pokemon_destino->posicion.Y);

	for(int i = 0; i < algoritmo.quantum; i++){

		int32_t posicion_final_X = entrenador->pokemon_destino->posicion.X - entrenador->posicion.X;
		int32_t posicion_final_Y = entrenador->pokemon_destino->posicion.Y - entrenador->posicion.Y;

		if(posicion_final_X != 0){
			if(posicion_final_X < 0){
				entrenador->posicion = avanzar(entrenador->posicion, -1, 0);
			} else {
				entrenador->posicion = avanzar(entrenador->posicion, 1, 0);
			}

			printf("**Avanzo 1 paso en X**\n");
		}


		if(posicion_final_Y != 0){
			if(posicion_final_Y < 0){
				entrenador->posicion = avanzar(entrenador->posicion, 0, -1);
			} else {
				entrenador->posicion = avanzar(entrenador->posicion, 0, 1);
			}
			printf("**Avanzo 1 paso en Y**\n");
		}

		posicion_final_X = entrenador->pokemon_destino->posicion.X - entrenador->posicion.X;
		posicion_final_Y = entrenador->pokemon_destino->posicion.Y - entrenador->posicion.Y;


		if(posicion_final_X == 0 && posicion_final_Y == 0){
			printf("**Llegué a destino, hago el catch**\n");
			entrenador->ocupado = true;
			entrenador->estado = BLOCKED; //blocked hasta que reciba la respuesta
			generar_y_enviar_catch(entrenador);

			break;
		} else {
			printf("**me cansé, lo dejo a otro**\n");
			entrenador->estado = READY;
			list_add(cola_ready, entrenador);
			sem_post(&s_cola_ready_con_items);
			break;
		}

	}

	return;

}

void planificar_sjfsd(){
	return;
}

void planificar_sjfcd(){
	return;
}

void planificar()
{

	switch(algoritmo.algoritmo_code){
	case FIFO:
		planificar_fifo();
		break;
	case RR:
		planificar_rr();
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

void show_entrenadores(){

	printf("PLANIFICACION: \n	ALGORITMO: %s, QUANTUM: %d\n", algoritmo.algoritmo_string, algoritmo.quantum);
	    printf("********************\n");


	    void _mostrar_pokemon(void* elemento){
	    	return mostrar_pokemon((t_pokemon_team*)elemento, objetivo_global);
	    }


	    for(int32_t i = 0; i < entrenadores->elements_count; i++){
	    	t_entrenador* entrenador_actual = list_get(entrenadores, i);
	    	printf("|ENTRENADOR %d\n|----------------\n|POSICION: (%d,%d)\n",
	    			i,
					entrenador_actual->posicion.X,
					entrenador_actual->posicion.Y);
	    	printf("|ESTADO %d\n", entrenador_actual->estado);
	    	if(entrenador_actual->ocupado) printf("|ESTA OCUPADO\n");

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

void generar_y_enviar_get(){
	for(int i = 0; i < objetivo_global->elements_count; i++){
		int32_t socket = conexion_broker();

		if(socket == 0){
			log_error(logger,"Error al conectar al Broker para enviar GET...");
			//logica para reintentar
			return;
		}

		log_info(logger,"Conectado al Broker para enviar GET");

		int32_t operacion = 0;
		int32_t id_mensaje = 0;
		int32_t tamanio_estructura = 0;
		t_pokemon_team* pokemon = list_get(objetivo_global, i);

		enviar_handshake(PROCESS_ID, socket);

		if(recv(socket, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
			if(operacion == ACK){
				recv(socket, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
				recv(socket, &id_mensaje, sizeof(int32_t), MSG_WAITALL);

				enviar_get_pokemon(pokemon->nombre, "0", socket);
				if(recv(socket, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
					if(operacion == ACK){
						recv(socket, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
						recv(socket, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
					}
				}
			}
		}

		liberar_conexion(socket);
	}

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
	t_Appeared* mensaje = malloc(sizeof(t_Appeared)); // reemplazar por funcion get size appeared de broker

	t_posicion posicion;
	posicion.X = (rand() % (10)) + 1; // numero random entre 1 y 10
	posicion.Y = (rand() % (10)) + 1;

	mensaje-> posicion = posicion;
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
	 * El objetivo de esta funcion es generar un mensaje appeared de forma aleatoria
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
		printf("posicion destino: X:%d, Y:%d\n", entrenador_actual->pokemon_destino->posicion.X, entrenador_actual->pokemon_destino->posicion.Y);
	}
}

void hilo_planificador(){

	while(1){ // while no haya otro en EXEC (semaforo?)
		printf("esperando que llegue algo a la cola de ready....\n");
		sem_wait(&s_cola_ready_con_items);
		printf("llegó algo a la cola de READY\n");
		//printf("arrancó el planificador: %s (%d)\n", algoritmo.algoritmo_string, algoritmo.algoritmo_code);
		//sem_wait(&s_procesos_en_exec);
		planificar();
	}
}

void ubicar_pokemones_localized(t_Localized* pokemones_a_ubicar){
	for(int i = 0; i < pokemones_a_ubicar->listaPosiciones->elements_count; i++){
		t_pokemon_team* pokemon_ubicado = malloc(sizeof(t_pokemon));
		t_posicion* posicion = list_get(pokemones_a_ubicar->listaPosiciones, i);

		pokemon_ubicado->cantidad = 1;
		pokemon_ubicado->posicion = *posicion;
		pokemon_ubicado->nombre = pokemones_a_ubicar->pokemon.nombre;

		list_add(pokemones_ubicados, pokemon_ubicado);
	}
}

void recibidor_mensajes_localized(void* args){
	t_args_mensajes* arg = (t_args_mensajes*)args;
	t_list* entrenadores = arg->entrenadores;
	t_Localized* mensaje = (t_Localized*)arg->mensaje;
	t_respuesta* respuesta = arg->respuesta;

	while(1){
		// esto simula que recibí un mensaje localized
		//t_Localized* mensaje = simular_localized(1);

		//int id = (rand() % (15)) + 1; // genero el id acá para probar

		printf("Se recibió un LOCALIZED %s (id: %d)\n", mensaje->pokemon.nombre, respuesta->id_respuesta);

		if(localized_valido(mensaje, respuesta->id_respuesta, mensajes_get_esperando_respuesta, pokemones_recibidos, objetivo_global)){
			printf("A WILD %s WAS LOCALIZED!!!!\n", mensaje->pokemon.nombre);
			list_add(pokemones_recibidos, mensaje->pokemon.nombre);


			//tengo que planificar tantas posiciones como pokemon necesite.
			//Ej, si me llegan 3 posiciones y necesito 2, solo planifico 2 pero las otras las guardo en memoria
			//Si necesito 2 posiciones y llega 1, planifico 1.

			int cantidad_pokemones_global = get_cantidad_by_nombre_pokemon(mensaje->pokemon.nombre, objetivo_global);

			// me quedo con la menor de las cantidades
			int cantidad_a_planificar = 0;
			if(mensaje->listaPosiciones->elements_count <= cantidad_pokemones_global ){
				cantidad_a_planificar = mensaje->listaPosiciones->elements_count;
			} else {
				cantidad_a_planificar = cantidad_pokemones_global;
			}

			for(int i = 0; i < cantidad_a_planificar; i++){
				t_posicion* posicion = list_get(mensaje->listaPosiciones, i);

				t_entrenador* entrenador_mas_cercano = get_entrenador_planificable_mas_cercano(entrenadores, *posicion);

				if(entrenador_mas_cercano != NULL){

					//Si hay entrenadores planificables, planifico esa posicion que siempre va a ser la 0
					list_remove(mensaje->listaPosiciones, 0);

					t_pokemon_team* pokemon_destino = get_pokemon_team(mensaje->pokemon.nombre, *posicion);

					entrenador_mas_cercano->estado = READY;
					entrenador_mas_cercano->pokemon_destino = pokemon_destino;

					list_add(cola_ready, entrenador_mas_cercano);
					sem_post(&s_cola_ready_con_items);
				} else {
					ubicar_pokemones_localized(mensaje);
				}
			}
		}
	}
}

void recibidor_mensajes_appeared(void* args){
	t_args_mensajes* arg = (t_args_mensajes*)args;
	t_list* l_entrenadores = arg->entrenadores;
	t_Appeared* mensaje = (t_Appeared*)arg->mensaje;


	printf("se recibió un mensaje APPEARED: %s\n", mensaje->pokemon.nombre);

	if(appeared_valido(mensaje, l_entrenadores, objetivo_global)){
		printf("A WILD %s APPEARED!!!!\n", mensaje->pokemon.nombre);

		list_add(pokemones_recibidos, mensaje->pokemon.nombre);
		t_entrenador* entrenador_mas_cercano = get_entrenador_planificable_mas_cercano((t_list*)l_entrenadores, mensaje->posicion);

		if(entrenador_mas_cercano != NULL){
			entrenador_mas_cercano->estado = READY;

			t_pokemon_team* pokemon_destino = get_pokemon_team(mensaje->pokemon.nombre, mensaje->posicion);
			pokemon_destino->planificable = false;
			entrenador_mas_cercano->pokemon_destino = pokemon_destino;
			entrenador_mas_cercano->ocupado = true;

			list_add(cola_ready, entrenador_mas_cercano);
			printf("el entrenador %d fue agregado a la cola READY\n", entrenador_mas_cercano->id);
			sem_post(&s_cola_ready_con_items);

		} else {
			t_pokemon_team* pokemon_ubicado = get_pokemon_team(mensaje->pokemon.nombre, mensaje->posicion);
			pokemon_ubicado->planificable = true;
			list_add(pokemones_ubicados, pokemon_ubicado);
		}
	}

}

void recibidor_mensajes_caught(void* args){
	t_args_mensajes* arg = (t_args_mensajes*)args;
	t_list* l_entrenadores = arg->entrenadores;
	t_Caught* mensaje = (t_Caught*)arg->mensaje;
	t_respuesta* respuesta = arg->respuesta;

	t_entrenador* entrenador = list_get(l_entrenadores, respuesta->id_entrenador);

	printf("TRYING TO CATCH A %s \nTRAINER %d USED A ULTRA BALL\n",entrenador->pokemon_destino->nombre, entrenador->id);
	printf("3...\n2... \n1... \n");
	sleep(3);

	if(mensaje->fueAtrapado){
		printf("GOTCHA! %s WAS CAUGHT!\n", entrenador->pokemon_destino->nombre);
		list_add(entrenador->pokemones, entrenador->pokemon_destino);

		if(cumplio_objetivo(entrenador)){
			entrenador->estado = EXIT;
		} else {
			entrenador->estado = READY;
			entrenador->ocupado = false;
		}
	} else {
		log_info(logger, "OH, NO! THE POKEMON %s BROKE FREE\n", entrenador->pokemon_destino->nombre);
		entrenador->estado = READY;
		entrenador->ocupado = false;
		//cuando falla, tengo que ir a capturar otro pokemon de esa especie.
		//mismo que antes, comparo entre todos los entrenadores disponibles y todas las posiciones de ese pokemon
		//o voy por la primer posicion que encuentre?
	}

	entrenador->estado = BLOCKED;

	return;
}

void hilo_escuchador_mensajes(void* l_entrenadores){
	printf("Escuchando mensajes...\n");
	while(socket_escucha_team != -1){
		int32_t socket_cliente = (int32_t)recibir_cliente(socket_escucha_team);

		if(socket_cliente != -1){
			//log_info(logger, "Se conecto el GAME BOY \n");

			int32_t codigo_operacion = 0;
			int32_t tamanio_estructura = 0;
			int32_t id_mensaje = 0;

			if(recv(socket_cliente, &codigo_operacion, sizeof(int32_t), MSG_WAITALL) == -1)
					codigo_operacion = -1;
			recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
			recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
			printf("llegó algo\n");
			//log_info(logger, "Código de operación %d\n", codigo_operacion);

			switch(codigo_operacion){
				case APPEARED_POKEMON:
					log_debug(logger, "Llego un mensaje APPEARED desde el GAME BOY\n");
					t_Appeared* mensaje_appeared = deserializar_paquete_appeared(&socket_cliente);

					log_debug(logger, "Llego un mensaje Appeared Pokemon con los siguientes datos: %d  %s  %d  %d\n",
							mensaje_appeared->pokemon.size_Nombre,
							mensaje_appeared->pokemon.nombre,
							mensaje_appeared->posicion.X,
							mensaje_appeared->posicion.Y);

					t_args_mensajes* args = malloc(sizeof(t_args_mensajes));
					args->entrenadores = (t_list*)l_entrenadores;
					args->mensaje = mensaje_appeared;
					args->respuesta = NULL;

					pthread_t p_generador_mensajes_appeared;
					pthread_create(&p_generador_mensajes_appeared, NULL, (void*)recibidor_mensajes_appeared, (void*)args);
					pthread_detach(p_generador_mensajes_appeared);

					break;

				case LOCALIZED_POKEMON:
					log_debug(logger, "Recibí un mensaje LOCALIZED desde el GAME BOY\n");
					t_Localized* mensaje_localized = deserializar_paquete_localized(&socket_cliente);

					log_debug(logger, "Llego un mensaje Localized Pokemon con los siguientes datos: %d\n",
							mensaje_localized->pokemon.nombre);

					for(int i = 0; i < mensaje_localized->listaPosiciones->elements_count; i++){
						t_posicion* posicion = list_get(mensaje_localized->listaPosiciones, i);
						log_info(logger, "Pos X: %d\nPos Y: %d\n", posicion->X, posicion->Y);
					}

					t_respuesta* respuesta_get = get_respuesta(id_mensaje, mensajes_get_esperando_respuesta);

					if(respuesta_get != NULL){
						t_args_mensajes* args = malloc(sizeof(t_args_mensajes));
						args->entrenadores = (t_list*)l_entrenadores;
						args->mensaje = mensaje_localized;
						args->respuesta = respuesta_get;



						 pthread_t p_generador_mensajes_localized;
						 pthread_create(&p_generador_mensajes_localized, NULL, (void*)recibidor_mensajes_localized, (void*)args);
					}

					break;

				case CAUGHT_POKEMON:
					log_info(logger, "Recibí un mensaje CAUGHT desde el GAME BOY\n");
					t_Caught* mensaje_caught = deserializar_paquete_caught(&socket_cliente);

					log_info(logger, "Llego un mensaje Caught Pokemon con los siguientes datos: %d\n",
												mensaje_caught->fueAtrapado);

					t_respuesta* respuesta_catch = get_respuesta(id_mensaje, mensajes_catch_esperando_respuesta);

					//debería ser un hilo
					if(respuesta_catch != NULL){

						t_args_mensajes* args = malloc(sizeof(t_args_mensajes));
						args->entrenadores = (t_list*)l_entrenadores;
						args->mensaje = mensaje_localized;
						args->respuesta = respuesta_catch;

						pthread_t p_generador_mensajes_caught;
						pthread_create(&p_generador_mensajes_caught, NULL, (void*)recibidor_mensajes_caught, (void*)args);

					}

					break;
				default:
					printf("no me interesa el mensaje");
					return;
			}
		}
	}
}

char* get_config_path(char* entrenador){
	char* cfg_path = string_new();
	string_append(&cfg_path, "/home/utnso/workspace/tp-2020-1c-5rona/team/config/");
	string_append(&cfg_path, entrenador);
	string_append(&cfg_path, ".config");

	return cfg_path;
}


void hilo_suscribirse_appeared(void* l_entrenadores){

	int32_t socket_suscripcion = conexion_broker();
	if(socket_suscripcion == 0){
		log_error(logger,"Error al conectar al Broker para suscribirse a Appeared...");
		//logica para reintentar
		return;
	}

	log_info(logger,"Conectado al Broker para suscribirse a APPEARED...\n");

	int32_t operacion = 0;
	int32_t id_mensaje = 0;
	int32_t tamanio_estructura = 0;

	if(recv(socket_suscripcion, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
		printf("codigo de op respuesta: %d\n", operacion);
		if(operacion == ACK){
			printf("el broker me dio el OK\n");
			recv(socket_suscripcion, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
			recv(socket_suscripcion, &id_mensaje, sizeof(int32_t), MSG_WAITALL);

			//voy a ir recibiendo mensajes APPEARED
			log_info(logger, "Esperando que llegue algún APPEARED de la cola de suscripcion...\n");
			while(recv(socket_suscripcion, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
				recv(socket_suscripcion, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
				recv(socket_suscripcion, &id_mensaje, sizeof(int32_t), MSG_WAITALL);

				enviar_handshake(PROCESS_ID, socket_suscripcion);

				if(operacion == APPEARED_POKEMON){
					log_debug(logger, "Llego un mensaje APPEARED desde el GAME BOY\n");
					t_Appeared* mensaje_appeared = deserializar_paquete_appeared(&socket_suscripcion);

					log_debug(logger, "Llego un mensaje Appeared Pokemon con los siguientes datos: %d  %s  %d  %d\n",
							mensaje_appeared->pokemon.size_Nombre,
							mensaje_appeared->pokemon.nombre,
							mensaje_appeared->posicion.X,
							mensaje_appeared->posicion.Y);

					t_args_mensajes* args = malloc(sizeof(t_args_mensajes));
					args->entrenadores = (t_list*)l_entrenadores;
					args->mensaje = mensaje_appeared;
					args->respuesta = NULL;

					pthread_t p_generador_mensajes_appeared;
					pthread_create(&p_generador_mensajes_appeared, NULL, (void*)recibidor_mensajes_appeared, (void*)args);
				}
			}
		}
	} else {
		log_error(logger, "el hilo murió\n");
		return;
	}

	return;
}

void hilo_suscribirse_caught(void* l_entrenadores){

	int32_t socket_suscripcion = conexion_broker();

	if(socket_suscripcion == 0){
		log_error(logger,"Error al conectar al Broker para suscribirse a CAUGHT...");
		//logica para reintentar (signal a un hilo?)
		return;
	}

	log_info(logger,"Conectado al Broker para suscribirse a CAUGHT...\n");

	int32_t operacion = 0;
	int32_t id_mensaje = 0;
	int32_t tamanio_estructura = 0;

	if(recv(socket_suscripcion, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
		printf("codigo de op respuesta: %d\n", operacion);
		if(operacion == ACK){
			printf("el broker me dio el OK\n");
			recv(socket_suscripcion, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
			recv(socket_suscripcion, &id_mensaje, sizeof(int32_t), MSG_WAITALL);

			//voy a ir recibiendo mensajes CAUGHT
			log_info(logger, "Esperando que llegue algún CAUGHT de la cola de suscripcion...\n");
			while(recv(socket_suscripcion, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
				recv(socket_suscripcion, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
				recv(socket_suscripcion, &id_mensaje, sizeof(int32_t), MSG_WAITALL);

				enviar_handshake(PROCESS_ID, socket_suscripcion);

				if(operacion == CAUGHT_POKEMON){
					log_debug(logger, "Recibí un mensaje CAUGHT\n");
					t_Caught* mensaje_caught = deserializar_paquete_caught(&socket_suscripcion);

					log_debug(logger, "Llego un mensaje Caught Pokemon con los siguientes datos: %d\n",
												mensaje_caught->fueAtrapado);

					t_respuesta* respuesta_catch = get_respuesta(id_mensaje, mensajes_catch_esperando_respuesta);

					//debería ser un hilo
					if(respuesta_catch != NULL){

					t_args_mensajes* args = malloc(sizeof(t_args_mensajes));
					args->entrenadores = (t_list*)l_entrenadores;
					args->mensaje = mensaje_caught;
					args->respuesta = respuesta_catch;

					pthread_t p_generador_mensajes_caught;
					pthread_create(&p_generador_mensajes_caught, NULL, (void*)recibidor_mensajes_caught, (void*)args);


						//recibidor_mensajes_caught(l_entrenadores, mensaje_caught); // cambiar nombre a la funcion
					}
				}
			}
		}
	} else {
		log_error(logger, "el hilo murió\n");
		return;
	}

	return;
}

void hilo_suscribirse_localized(void* l_entrenadores){

	int32_t socket_suscripcion = conexion_broker();

	if(socket_suscripcion == 0){
		log_error(logger,"Error al conectar al Broker para suscribirse a Appeared...");
		//logica para reintentar
		return;
	}

	log_info(logger,"Conectado al Broker para suscribirse a LOCALIZED...\n");


	int32_t operacion = 0;
	int32_t id_mensaje = 0;
	int32_t tamanio_estructura = 0;

	if(recv(socket_suscripcion, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
		printf("codigo de op respuesta: %d\n", operacion);
			if(operacion == ACK){
				printf("el broker me dio el OK\n");
				recv(socket_suscripcion, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
				recv(socket_suscripcion, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
				//voy a ir recibiendo mensajes LOCALIZED
				log_info(logger, "Esperando que llegue algún LOCALIZED de la cola de suscripcion...");
				while(recv(socket_suscripcion, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
					log_debug(logger, "Recibí un mensaje LOCALIZED\n");
					t_Localized* mensaje_localized = deserializar_paquete_localized(&socket_suscripcion);

					log_debug(logger, "Llego un mensaje LOCALIZED Pokemon con los siguientes datos: %d\n",
							mensaje_localized->pokemon.nombre);

					for(int i = 0; i < mensaje_localized->listaPosiciones->elements_count; i++){
						t_posicion* posicion = list_get(mensaje_localized->listaPosiciones, i);
						log_info(logger, "Pos X: %d\nPos Y: %d\n", posicion->X, posicion->Y);
					}

					t_respuesta* respuesta_get = get_respuesta(id_mensaje, mensajes_get_esperando_respuesta);

					t_args_mensajes* args = malloc(sizeof(t_args_mensajes));
					args->entrenadores = (t_list*)l_entrenadores;
					args->mensaje = mensaje_localized;
					args->respuesta = respuesta_get;

					pthread_t p_generador_mensajes_localized;
					pthread_create(&p_generador_mensajes_localized, NULL, (void*)recibidor_mensajes_localized, (void*)args);

				}
		}
	}else {
		log_error(logger, "el hilo murió\n");
		return;
	}

	return;
}

int inicializar_team(char* entrenador){

	cola_ready = list_create();
	objetivo_global = list_create();
	pokemones_recibidos = list_create();
	pokemones_ubicados = list_create();
	mensajes_get_esperando_respuesta = list_create();
	mensajes_catch_esperando_respuesta = list_create();
	entrenadores = list_create();
	sem_init(&s_cola_ready_con_items, 0, 0);
	//sem_init(&s_procesos_en_exec, 0, 1);
	srand(time(NULL));
	config = config_create(get_config_path(entrenador));
	printf("el entrenador que se va a cargar es el de la config: %s\n", entrenador);
	char* log_path = config_get_string_value(config, "LOG_FILE");
	logger = log_create(log_path, "Team", 1, LOG_LEVEL_INFO);
	IP_BROKER = config_get_string_value(config, "IP_BROKER");
	PUERTO_BROKER = config_get_string_value(config, "PUERTO_BROKER");
	PROCESS_ID = atoi(config_get_string_value(config, "PROCESS_ID"));
	algoritmo = get_algoritmo(config);

	return 1;
}

bool esta_en_deadlock(t_entrenador* entrenador){
	return ((t_entrenador*)entrenador)->estado == BLOCKED &&
			!entrenador->ocupado &&
			!cumplio_objetivo((t_entrenador*)entrenador) &&
			!puede_capturar_pokemones((t_entrenador*)entrenador);
}

void entrenador(void* index){

	t_entrenador* entrenador = list_get(entrenadores, (int)index);

	//NEW
	printf("este hilo maneja al entrenador %d\n", entrenador->id);
	while(!cumplio_objetivo(entrenador)){

		sem_wait(entrenador->semaforo);
		//mientras no llegue a destino ejecuto
		while(entrenador->posicion.X != entrenador->pokemon_destino->posicion.X ||
				entrenador->posicion.Y != entrenador->pokemon_destino->posicion.Y){
			sem_wait(entrenador->semaforo);//EXEC
			int i = 1;
			//AL MENOS EN FIFO ES ASI: poner logica del algoritmo de planificacion acá o que lo maneje el planificador?
			printf("soy el entrenador %d y voy a EXEC\n", entrenador->id);
			printf("me voy a mover desde %d, %d\n", entrenador->posicion.X, entrenador->posicion.Y);
			printf("me voy a mover hasta %d, %d\n", entrenador->pokemon_destino->posicion.X, entrenador->pokemon_destino->posicion.Y);


			int32_t posicion_final_X = entrenador->pokemon_destino->posicion.X - entrenador->posicion.X;
			int32_t posicion_final_Y = entrenador->pokemon_destino->posicion.Y - entrenador->posicion.Y;

			if(posicion_final_X != 0){
				if(posicion_final_X < 0){
					entrenador->posicion = avanzar(entrenador->posicion, -1, 0);
				} else {
					entrenador->posicion = avanzar(entrenador->posicion, 1, 0);
				}

				printf("**Avanzo 1 paso en X**\n");
				continue;
			}


			if(posicion_final_Y != 0){
				printf("A PUNTO DE AVANZAR EN Y");
				if(posicion_final_Y < 0){
					entrenador->posicion = avanzar(entrenador->posicion, 0, -1);
				} else {
					entrenador->posicion = avanzar(entrenador->posicion, 0, 1);
				}
				printf("**Avanzo 1 paso en Y**\n");
				continue;
			}

			i++;
		}

		printf("LLEGUÉ A DESTINO!! X: %d, Y: %d\n", entrenador->posicion.X, entrenador->posicion.Y);

		if(!generar_y_enviar_catch(entrenador)){//esto quiere decir que no se pudo conectar al broker
			list_add(entrenador->pokemones, entrenador->pokemon_destino);
			entrenador->pokemon_destino = NULL;
			log_info(logger, "POKEMON CAPTURADO!\n");
			entrenador->estado = BLOCKED;
			entrenador->ocupado=false;

			if(cumplio_objetivo(entrenador)){
				log_info(logger, "el entrenador %d cumplio sus objetivos, pasandolo a EXIT\n", entrenador->id);
				entrenador->estado = EXIT;
				entrenador->ocupado = false;
			} else {
				log_info(logger, "el entrenador %d aún no cumplio sus objetivos, pasandolo a BLOCKED\n", entrenador->id);
				entrenador->estado = BLOCKED;
				entrenador->ocupado=false;

				if(puede_capturar_pokemones(entrenador)){
					printf("el entrenador puede capturar mas pokemones\n");
				} else {
					printf("el entrenador no puede capturar mas pokemones\n");
				}
			}
		} else {
			log_info(logger, "ESPERANDO A VER SI LO CAPTURÉ!\n");
			entrenador->estado = BLOCKED;
			entrenador->ocupado=true;
		}

	}


	/*
	while(cumplio_objetivo(entrenador)){

	}
	//EXEC se mueve
	while(entrenador.posicion != entrenador.posicionDestino){
		sem_wait(s_entrenador);

		movete(1);
	}
	*/

	//catch

	//exit



}


void reconectar_broker(){
	while(1){
		sem_wait(&s_reconnect_broker);
		int socket = conexion_broker();
		int tiempo_reconexion = atoi(config_get_string_value(config, "TIEMPO_RECONEXION"));
		while(socket == 0){
			printf("intentando reconcetar... \n");
			sleep(tiempo_reconexion);
		}

		printf("reconectado correctamente!");
	}
}

int32_t main(int32_t argc, char** argv){
	if(!argv[1]){
		printf("Fata definir el team a cargar\n");
		return EXIT_FAILURE;
	}

	inicializar_team(argv[1]);
	log_info(logger, "inicializacion finalizada\n");
    cantidad_entrenadores = array_length(config_get_array_value(config, "POSICIONES_ENTRENADORES"));

    entrenadores = get_entrenadores(config, cantidad_entrenadores);

    printf("En este team hay %d entrenadores\n", cantidad_entrenadores);

    objetivo_global = get_objetivo_global(entrenadores);


/*
    generar_y_enviar_get();

    pthread_t p_suscribirse_appeared;
   	pthread_create(&p_suscribirse_appeared, NULL, (void*)hilo_suscribirse_appeared, (void*)entrenadores);


	pthread_t p_suscribirse_caught;
	pthread_create(&p_suscribirse_caught, NULL, (void*)hilo_suscribirse_caught, (void*)entrenadores);

	pthread_t p_suscribirse_localized;
	pthread_create(&p_suscribirse_localized, NULL, (void*)hilo_suscribirse_localized, (void*)entrenadores);

	pthread_t p_reconnect;
	pthread_create(&p_reconnect, NULL, (void*)reconectar_broker, NULL);

 */

    /* HILO PLANIFICADOR */
    pthread_t p_planificador;
	pthread_create(&p_planificador, NULL, (void*)hilo_planificador, NULL);
	pthread_detach(p_planificador);

	/* UN HILO POR PROCESO */
    for(int x = 0; x < cantidad_entrenadores; x++){
    	pthread_t p_entrenador;
		pthread_create(&p_entrenador, NULL, (void*)entrenador, (void*)x);
		pthread_detach(p_entrenador);
    }

    pthread_t p_escuchador;
    /* HILO ESCUCHADOR DE GAMEBOY */
    char* ip = config_get_string_value(config, "IP");
	char* puerto = config_get_string_value(config, "PUERTO");

	socket_escucha_team = crear_socket_escucha(ip, puerto);

	log_info(logger, "Creado socket de escucha \n");

	if(socket_escucha_team == -1){
		log_error(logger, "Fallo al crear socket de escucha = -1\n");
	} else {
	    pthread_create(&p_escuchador, NULL, (void*)hilo_escuchador_mensajes, (void*)entrenadores);
	}

	pthread_join(p_escuchador, NULL);


	free(objetivo_global);
	free(entrenadores);

	liberar_conexion(socket_escucha_team);

    printf("End\n");

    return EXIT_SUCCESS;
}




