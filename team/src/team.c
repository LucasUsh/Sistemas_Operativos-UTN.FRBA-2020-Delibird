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

void actualizar_estado(t_entrenador* entrenador, estado_code estado){

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

void generar_y_enviar_catch(t_entrenador* entrenador){

	t_Catch mensaje;
	mensaje.pokemon.nombre = entrenador->pokemon_destino->nombre;
	mensaje.pokemon.size_Nombre = string_length(entrenador->pokemon_destino->nombre);  // creo que hay que sumar 1
	mensaje.posicion = entrenador->pokemon_destino->posicion;

	printf("Se generó el mensaje CATCH %s %d %d\n", mensaje.pokemon.nombre, mensaje.posicion.X, mensaje.posicion.Y);
	int id = (rand() % (10)) + 1;  // numero random entre 1 y 10

	printf("La id correlativo es: %d\n", id);

	t_respuesta* respuesta = malloc(sizeof(t_respuesta));
	respuesta->id_entrenador = entrenador->id;
	respuesta->id_respuesta = id;

	list_add(mensajes_catch_esperando_respuesta, respuesta);
	//printf("mensajes CATCH esperando respuesta: %d\n", mensajes_catch_esperando_respuesta->elements_count);
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

int32_t conexion_broker()
{
	int32_t socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
	return socket;
}

void generar_y_enviar_get(t_list* objetivo_global){

	int id = (rand() % (10)) + 1; // numero random entre 1 y 10

	for(int i = 0; i < objetivo_global->elements_count; i++){
		t_pokemon_team* pokemon_actual = list_get(objetivo_global, i);

		t_Get* mensaje_get = sizeof(typeof(t_Get));
		mensaje_get->pokemon.nombre = pokemon_actual->nombre;
		mensaje_get->pokemon.size_Nombre = string_length(pokemon_actual->nombre) + 1;


		//conectarse al broker y mandar el mensaje;


		int32_t socket_broker = conexion_broker();

		if(socket_broker == 0){
			log_error(logger,"Error al conectar al Broker\n");
		} else {
			log_info(logger, "mandando el mensaje GET %s...\n", mensaje_get->pokemon.nombre);
			t_paquete* paquete = malloc(sizeof(t_paquete));
			int bytes = 0;
			void* dato_a_enviar = serializar_paquete_get(paquete, &bytes, mensaje_get);

		}


		log_info(logger, "mandando el mensaje GET %s...\n", mensaje_get->pokemon.nombre);
		t_respuesta* respuesta = malloc(sizeof(t_respuesta));
		respuesta->id_entrenador=99;//no importa el id del entrenador
		respuesta->id_respuesta = id++;
		printf("Recibí el ID: %d\n", respuesta->id_respuesta);
		list_add(mensajes_get_esperando_respuesta, respuesta);

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

void hilo_recibidor_mensajes_localized(void* l_entrenadores){
	t_list* entrenadores = (t_list*)l_entrenadores;

	while(1){
		sleep(5);
		// esto simula que recibí un mensaje localized
		t_Localized* mensaje = simular_localized(1);

		printf("Se recibió un LOCALIZED %s", mensaje->pokemon.nombre);

		int id = (rand() % (15)) + 1; // genero el id acá para probar

		if(localized_valido(mensaje, id, mensajes_get_esperando_respuesta, pokemones_recibidos, objetivo_global)){
			printf("A WILD %s WAS LOCALIZED!!!!\n", mensaje->pokemon.nombre);
			list_add(pokemones_recibidos, mensaje->pokemon.nombre);
			// No sé si debería comparar todas las posiciones, con todos los entrenadores
			// y obtener el de distancia mas corta entre todas las posiciones y todos los entrenadores.
			// Por ahora vamos con la primera:
			t_posicion* posicion = list_get(mensaje->listaPosiciones, 0);

			t_entrenador* entrenador_mas_cercano = get_entrenador_planificable_mas_cercano(entrenadores, *posicion);

			if(entrenador_mas_cercano != NULL){

				//Si hay entrenadores planificables, planifico esa posicion
				list_remove(mensaje->listaPosiciones, 0);


				//agrego a mi mapa todas las otras posiciones
				if(mensaje->listaPosiciones->elements_count > 0){
					ubicar_pokemones_localized(mensaje);
				}

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


void hilo_recibidor_mensajes_appeared(void* l_entrenadores){
	t_list* entrenadores = (t_list*)l_entrenadores;

	while(1){

		t_Appeared* mensaje = simular_appeared();
		printf("se recibió un mensaje APPEARED: %s\n", mensaje->pokemon.nombre);
		//printf("***************************************\n");

		if(appeared_valido(mensaje, pokemones_recibidos, objetivo_global)){
			printf("A WILD %s APPEARED!!!!\n", mensaje->pokemon.nombre);

			list_add(pokemones_recibidos, mensaje->pokemon.nombre);
			t_entrenador* entrenador_mas_cercano = get_entrenador_planificable_mas_cercano(entrenadores, mensaje->posicion);

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

		//printf("***************************************\n");

		sleep(5);

	}
}

void hilo_recibidor_mensajes_caught(void* l_entrenadores){
	t_list* entrenadores = (t_list*)l_entrenadores;

	while(1){
		sleep(5);
		// esto simula que recibí un mensaje localized
		t_Caught* mensaje = generar_caught();
		int id = (rand() % (10)) + 1; // genero el id acá para probar pero se recibe antes


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

void hilo_recibidor_mensajes_full(void* l_entrenadores, t_paquete* paquete){
	while(1){
		switch(paquete->codigo_operacion){
		case APPEARED_POKEMON:
			printf("Recibí un APPEARED\n");
			hilo_recibidor_mensajes_appeared(l_entrenadores);
			break;
		case LOCALIZED_POKEMON:
			printf("Recibí un LOCALIZED\n");
			t_respuesta* respuesta = get_respuesta(paquete->buffer->id_Mensaje, mensajes_get_esperando_respuesta);
			if(respuesta != NULL){
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

char* get_config_path(char* entrenador){

	 if(entrenador == NULL){
		printf("falta definir el team\n");
		return 0;
	}

	char* cfg_path = string_new();
	string_append(&cfg_path, "/home/utnso/workspace/tp-2020-1c-5rona/team/config/");
	string_append(&cfg_path, entrenador);
	string_append(&cfg_path, ".config");

	return cfg_path;
}

void inicializar_team(char* entrenador){
	cola_ready = list_create();
	objetivo_global = list_create();
	pokemones_recibidos = list_create();
	pokemones_ubicados = list_create();
	mensajes_get_esperando_respuesta = list_create();
	mensajes_catch_esperando_respuesta = list_create();
	sem_init(&s_cola_ready_con_items, 0, 0);
	srand(time(NULL));
	printf("el entrenador que se va a cargar es el de la config: %s\n", entrenador);
	config = config_create(get_config_path(entrenador));
	logger = log_create("/home/utnso/workspace/tp-2020-1c-5rona/team/team.log", "Team", 1, LOG_LEVEL_INFO);
	IP_BROKER = config_get_string_value(config, "IP_BROKER");
	PUERTO_BROKER = config_get_string_value(config, "PURTO_BROKER");

	return;
}




int32_t main(int32_t argc, char** argv){

	inicializar_team(argv[1]);
    int32_t cantidad_entrenadores = array_length(config_get_array_value(config, "POKEMON_ENTRENADORES"));


    algoritmo = get_algoritmo(config);
    t_list* entrenadores = get_entrenadores(config, cantidad_entrenadores);

    printf("En este team hay %d entrenadores\n", cantidad_entrenadores);


    objetivo_global = get_objetivo_global(entrenadores);


    int32_t socket_broker = conexion_broker();

	if(socket_broker == 0){
		log_error(logger,"Error al conectar al Broker\n");
	} else {
		log_info(logger, "Conexion con el Broker correcta\n");
		generar_y_enviar_get(objetivo_global);
	}

    pthread_t p_generador_mensajes_localized;
    pthread_create(&p_generador_mensajes_localized, NULL, (void*)hilo_recibidor_mensajes_localized, (void*)entrenadores);


    pthread_t p_generador_mensajes_appeared;
	pthread_create(&p_generador_mensajes_appeared, NULL, (void*)hilo_recibidor_mensajes_appeared, (void*)entrenadores);


	pthread_t p_generador_mensajes_caught;
	pthread_create(&p_generador_mensajes_caught, NULL, (void*)hilo_recibidor_mensajes_caught, (void*)entrenadores);

    pthread_t p_planificador;
	pthread_create(&p_planificador, NULL, (void*)hilo_planificador, (void*)entrenadores);



	int32_t socketEscucha = crear_socket_escucha("127.0.0.2", "5002");
	log_info(logger, "Creado socket de escucha \n");

	if(socketEscucha == -1){
		log_error(logger, "Fallo al crear socket de escucha = -1\n");
		return EXIT_FAILURE;
	}

	while(socketEscucha != -1){
		int32_t socket_cliente = (int32_t)recibir_cliente(socketEscucha);

		if(socket_cliente != -1){
			log_info(logger, "Se conecto un proceso \n");
		}
	}

    printf("End\n");

    return EXIT_SUCCESS;
}

