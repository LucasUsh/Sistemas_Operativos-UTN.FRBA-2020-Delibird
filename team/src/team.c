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

//semaforos
sem_t s_cola_ready_con_items;

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


void planificar_fifo(){

	printf("planificando FIFO...\n");
	//while(cola_ready->elements_count > 0){
		printf("proximo entrenador..\n");

		// en fifo, el proximo entrenador es el que esté primero en la cola de ready
		t_entrenador* entrenador = list_remove(cola_ready, 0);
		entrenador->estado = EXEC;

		int32_t posicion_final_X = entrenador->pokemon_destino->posicion.X - entrenador->posicion.X;
		int32_t posicion_final_Y = entrenador->pokemon_destino->posicion.Y - entrenador->posicion.Y;

		printf("posicion vieja: x-> %d, y-> %d\n", entrenador->posicion.X, entrenador->posicion.Y);

		entrenador->posicion = avanzar(entrenador->posicion, posicion_final_X , posicion_final_Y);

		printf("posicion nueva: x-> %d, y-> %d\n", entrenador->posicion.X, entrenador->posicion.Y);

		entrenador->estado = BLOCKED;
		//una vez que lo muevo llamo al broker y hago el catch

	//}

	return;
}

void planificar_rr(t_list* entrenadores, int32_t quantum){
	// en rr, el proximo entrenador es el que esté primero en la cola de ready
	//printf("planificando RR...\n");
	//while(cola_ready->elements_count > 0){

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
			entrenador->estado = EXIT; // este blocked en realidad permite que se pueda seguir planificando, cuando no.
			generar_y_enviar_catch(entrenador);
			break;
		} else {
			printf("**me cansé, lo dejo a otro**\n");
			entrenador->estado = READY;
			list_add(cola_ready, entrenador);
			sem_post(&s_cola_ready_con_items);
			break;
		}
		sleep(5);
	}

	return;

	//}
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

void generar_y_enviar_get(){
	for(int i = 0; i < objetivo_global->elements_count; i++){
		int32_t socket = conexion_broker();

		if(socket == 0){
			log_error(logger,"Error al conectar al Broker...");
			//logica para reintentar
			return;
		}

		int32_t operacion = 0;
		int32_t id_mensaje = 0;
		int32_t tamanio_estructura = 0;
		t_pokemon_team* pokemon = list_get(objetivo_global, i);

		log_info(logger,"Conectado al Broker");
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

void generar_y_enviar_catch(t_entrenador* entrenador){

	int32_t socket = conexion_broker();

	if(socket == 0){
		log_error(logger,"Error al conectar al Broker...");
		//logica de reintentar
		return;
	}

	int32_t operacion = 0;
	int32_t id_mensaje = 0; // esto creo que habria que cambiarlo
	int32_t tamanio_estructura = 0;

	log_info(logger,"Conectado al Broker para enviar Catch");
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
				}
			}
		}
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

void hilo_planificador(void* l_entrenadores){

	while(1){
		printf("esperando que llegue algo a la cola de ready....\n");
		sem_wait(&s_cola_ready_con_items);
		printf("arrancó el planificador: %s (%d)\n", algoritmo.algoritmo_string, algoritmo.algoritmo_code);
		planificar(algoritmo, (t_list*)l_entrenadores);
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

void recibidor_mensajes_localized(void* l_entrenadores, t_Localized* mensaje_localized){
	t_list* entrenadores = (t_list*)l_entrenadores;

	while(1){
		sleep(5);
		// esto simula que recibí un mensaje localized
		t_Localized* mensaje = simular_localized(1);

		int id = (rand() % (15)) + 1; // genero el id acá para probar

		printf("Se recibió un LOCALIZED %s (id: %d)\n", mensaje->pokemon.nombre, id);

		if(localized_valido(mensaje, id, mensajes_get_esperando_respuesta, pokemones_recibidos, objetivo_global)){
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

void recibidor_mensajes_appeared(void* l_entrenadores, t_Appeared* mensaje){

	printf("se recibió un mensaje APPEARED: %s\n", mensaje->pokemon.nombre);

	if(appeared_valido(mensaje, pokemones_recibidos, objetivo_global)){
		printf("A WILD %s APPEARED!!!!\n", mensaje->pokemon.nombre);

		list_add(pokemones_recibidos, mensaje->pokemon.nombre);
		t_entrenador* entrenador_mas_cercano = get_entrenador_planificable_mas_cercano((t_list*)l_entrenadores, mensaje->posicion);

		if(entrenador_mas_cercano != NULL){
			entrenador_mas_cercano->estado = READY;

			t_pokemon_team* pokemon_destino = get_pokemon_team(mensaje->pokemon.nombre, mensaje->posicion);
			entrenador_mas_cercano->pokemon_destino = pokemon_destino;

			list_add(cola_ready, entrenador_mas_cercano);
			sem_post(&s_cola_ready_con_items);
		} else {
			t_pokemon_team* pokemon_ubicado = get_pokemon_team(mensaje->pokemon.nombre, mensaje->posicion);

			list_add(pokemones_ubicados, pokemon_ubicado);
		}
	}

}

void recibidor_mensajes_caught(void* l_entrenadores, t_Caught* mensaje_caught){
	t_list* entrenadores = (t_list*)l_entrenadores;

	while(1){
		sleep(5);
		// esto simula que recibí un mensaje localized
		t_Caught* mensaje = generar_caught();
		mensaje_caught = mensaje;
		int id = (rand() % (10)) + 1; // genero el id acá para probar pero se recibe antes

		printf("se recibió un mensaje CAUGHT (id: %d)\n", id);
		t_respuesta* respuesta = get_respuesta(id, mensajes_catch_esperando_respuesta);

		if(respuesta != NULL){

			t_entrenador* entrenador = list_get(entrenadores, respuesta->id_entrenador);

			printf("TRYING TO CATCH A %s (ID: %d)\nTRAINER %d USED A ULTRA BALL\n",entrenador->pokemon_destino->nombre, respuesta->id_respuesta ,respuesta->id_entrenador);
			printf("3...\n2... \n1... \n");
			sleep(3);

			if(mensaje->fueAtrapado){
				printf("GOTCHA! %s WAS CAUGHT!\n", entrenador->pokemon_destino->nombre);
				list_add(entrenador->pokemones, entrenador->pokemon_destino);
			} else {
				printf("OH, NO! THE POKEMON %s BROKE FREE\n", entrenador->pokemon_destino->nombre);
				//cuando falla, tengo que ir a capturar otro pokemon de esa especie.
				//mismo que antes, comparo entre todos los entrenadores disponibles y todas las posiciones de ese pokemon
				//o voy por la primer posicion que encuentre?
			}

			entrenador->estado = BLOCKED;

		}

	}
}

void hilo_escuchador_mensajes(void* l_entrenadores){
	printf("Escuchando mensajes...\n");
	while(socket_escucha_team != -1){
		int32_t socket_cliente = (int32_t)recibir_cliente(socket_escucha_team);

		if(socket_cliente != -1){
			log_info(logger, "Se conecto un proceso \n");

			int32_t codigo_operacion = 0;
			int32_t tamanio_estructura = 0;
			int32_t id_mensaje = 0;

			if(recv(socket_cliente, &codigo_operacion, sizeof(int32_t), MSG_WAITALL) == -1)
					codigo_operacion = -1;
			recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
			recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);

			log_info(logger, "Código de operación %d", codigo_operacion);

			switch(codigo_operacion){
				case APPEARED_POKEMON:
					log_info(logger, "Llego un mensaje APPEARED \n");
					t_Appeared* mensaje_appeared = deserializar_paquete_appeared(&socket_cliente);

					log_info(logger, "Llego un mensaje Appeared Pokemon con los siguientes datos: %d  %s  %d  %d\n",
							mensaje_appeared->pokemon.size_Nombre,
							mensaje_appeared->pokemon.nombre,
							mensaje_appeared->posicion.X,
							mensaje_appeared->posicion.Y);

					//debería ser un hilo
					/*
					    pthread_t p_generador_mensajes_appeared;
						pthread_create(&p_generador_mensajes_appeared, NULL, (void*)hilo_recibidor_mensajes_appeared, (void*)entrenadores);
					*/
					recibidor_mensajes_appeared(l_entrenadores, mensaje_appeared);
					break;

				case LOCALIZED_POKEMON:
					log_info(logger, "Recibí un mensaje LOCALIZED\n");
					t_Localized* mensaje_localized = deserializar_paquete_localized(&socket_cliente);

					log_info(logger, "Llego un mensaje Localized Pokemon con los siguientes datos: %d\n",
							mensaje_localized->pokemon.nombre);

					for(int i = 0; i < mensaje_localized->listaPosiciones->elements_count; i++){
						t_posicion* posicion = list_get(mensaje_localized->listaPosiciones, i);
						log_info(logger, "Pos X: %d\nPos Y: %d\n", posicion->X, posicion->Y);
					}

					t_respuesta* respuesta_get = get_respuesta(id_mensaje, mensajes_get_esperando_respuesta);

					//debería ser un hilo
					/*
					 pthread_t p_generador_mensajes_localized;
					 pthread_create(&p_generador_mensajes_localized, NULL, (void*)hilo_recibidor_mensajes_localized, (void*)entrenadores);
*/
					if(respuesta_get != NULL){
						recibidor_mensajes_localized(l_entrenadores, mensaje_localized);
					}

					break;

				case CAUGHT_POKEMON:
					log_info(logger, "Recibí un mensaje CAUGHT\n");
					t_Caught* mensaje_caught = deserializar_paquete_caught(&socket_cliente);

					log_info(logger, "Llego un mensaje Caught Pokemon con los siguientes datos: %d\n",
												mensaje_caught->fueAtrapado);

					t_respuesta* respuesta_catch = get_respuesta(id_mensaje, mensajes_catch_esperando_respuesta);

					//debería ser un hilo
					/*
						pthread_t p_generador_mensajes_caught;
						pthread_create(&p_generador_mensajes_caught, NULL, (void*)hilo_recibidor_mensajes_caught, (void*)entrenadores);
					*/
					if(respuesta_catch != NULL){
						recibidor_mensajes_caught(l_entrenadores, mensaje_caught); // cambiar nombre a la funcion
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

int inicializar_team(char* entrenador){

	cola_ready = list_create();
	objetivo_global = list_create();
	pokemones_recibidos = list_create();
	pokemones_ubicados = list_create();
	mensajes_get_esperando_respuesta = list_create();
	mensajes_catch_esperando_respuesta = list_create();
	sem_init(&s_cola_ready_con_items, 0, 0);
	srand(time(NULL));
	config = config_create(get_config_path(entrenador));
	printf("el entrenador que se va a cargar es el de la config: %s\n", entrenador);
	logger = log_create("/home/utnso/workspace/tp-2020-1c-5rona/team/team.log", "Team", 1, LOG_LEVEL_INFO);
	IP_BROKER = config_get_string_value(config, "IP_BROKER");
	PUERTO_BROKER = config_get_string_value(config, "PUERTO_BROKER");
	PROCESS_ID = atoi(config_get_string_value(config, "PROCESS_ID"));

	return 1;
}

bool hay_deadlock(t_list* entrenadores){
	//hay deadlock cuando ningun entrenador cumplio sus objetivos y no pueden capturar mas

	//en realidad esta mal, no necesariamente participan todos los entrenadores en un deadlock

	//un entrenador PODRIA estar en deadlock si se cumple lo de abajo, pero no tienen que ser todos
	//podria tener una funcion que sea GET ENTRENADORES EN DEADLOCK

	//se debe cumplir que:
	//el entrenador este blocked
	//tiene pokemon/es que no necesita o en exceso
	//debe haber espera circular


	//O sea, que para que haya deadlock tienen que haber al menos dos entrenadores que cumplen lo de abajo.
	bool _blockeado_sin_poder_hacer_nada(void* entrenador){
		return ((t_entrenador*)entrenador)->estado == BLOCKED &&
				!cumplio_objetivo((t_entrenador*)entrenador) &&
				!puede_capturar_pokemones((t_entrenador*)entrenador);
	}

	return list_all_satisfy(entrenadores, _blockeado_sin_poder_hacer_nada);
}

int32_t main(int32_t argc, char** argv){
	if(!argv[1]){
		printf("Fata definir el team a cargar\n");
		return EXIT_FAILURE;
	}

	inicializar_team(argv[1]);
    int32_t cantidad_entrenadores = array_length(config_get_array_value(config, "POKEMON_ENTRENADORES"));


    algoritmo = get_algoritmo(config);
    t_list* entrenadores = get_entrenadores(config, cantidad_entrenadores);

    printf("En este team hay %d entrenadores\n", cantidad_entrenadores);

    objetivo_global = get_objetivo_global(entrenadores);

	generar_y_enviar_get();

    pthread_t p_planificador;
	pthread_create(&p_planificador, NULL, (void*)hilo_planificador, (void*)entrenadores);


    char* ip = config_get_string_value(config, "IP");
	char* puerto = config_get_string_value(config, "PUERTO");

	socket_escucha_team = crear_socket_escucha(ip, puerto);

	log_info(logger, "Creado socket de escucha \n");

	if(socket_escucha_team == -1){
		log_error(logger, "Fallo al crear socket de escucha = -1\n");
		return EXIT_FAILURE;
	} else {
		  pthread_t p_escuchador;
		  pthread_create(&p_escuchador, NULL, (void*)hilo_escuchador_mensajes, (void*)entrenadores);
	}

	while(1){}

	free(objetivo_global);
	free(entrenadores);

	liberar_conexion(socket_escucha_team);

    printf("End\n");

    return EXIT_SUCCESS;
}




