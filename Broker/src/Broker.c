/*
 ============================================================================
 Name        : Broker.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "Broker.h"
#include "Particiones.h"
#include "../../OurLibraries/Sockets/mensajes.h"

int32_t id_mensaje_global = 0;
pthread_mutex_t mutex_id_mensaje;
pthread_mutex_t mutex_list_mensaje;
pthread_mutex_t mutex_guardar_en_memoria;

int32_t main(void) {
	iniciarBroker();
	pthread_mutex_init(&mutex_guardar_en_memoria, NULL);

	signal(SIGUSR1, rutina);

	int32_t socketEscucha = crear_socket_escucha(IP_BROKER, PUERTO_BROKER);
	log_info(logger, "Creado socket de escucha");

	while(socketEscucha != -1){
		int32_t socket_cliente = (int32_t)recibir_cliente(socketEscucha);

		if(socket_cliente != -1){

			int32_t tamanio_estructura = 0;
			int32_t id_mensaje=0;
			int32_t operacion=0;
			pthread_t hilo;
			int32_t id_proceso =0;
			t_estructura_hilo_mensaje estructura_mensaje;

			//HANDSHAKE
			if(recv(socket_cliente, &operacion, sizeof(int32_t), MSG_WAITALL) > 0){
				if(operacion == HANDSHAKE){
					recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
					recv(socket_cliente, &id_proceso, sizeof(int32_t), MSG_WAITALL);
					log_info(logger, "Se conecto el proceso %d", id_proceso);
					//ACK DEL HANDSHAKE
					enviar_ACK(0, socket_cliente);
					//ESPERA EL MENSAJE
					if(recv(socket_cliente, &operacion, sizeof(int32_t), MSG_WAITALL) > 0){
						switch(operacion){
						case SUSCRIPCION_APPEARED:
						case SUSCRIPCION_GET:
						case SUSCRIPCION_LOCALIZED:
						case SUSCRIPCION_CATCH:
						case SUSCRIPCION_CAUGHT:
						case SUSCRIPCION_NEW:
							recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
							recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
							t_estructura_hilo_suscriptor estructura_suscriptor;
							estructura_suscriptor.socket_cliente = socket_cliente;
							estructura_suscriptor.operacion = operacion;
							estructura_suscriptor.id_proceso = id_proceso;
							if (pthread_create(&hilo, NULL, (void*)manejoSuscripcion, &estructura_suscriptor) == 0){
							}else return -1;
							pthread_detach(hilo);
						break;
						case NEW_POKEMON:
							log_info(logger, "Llego un mensaje NEW_POKEMON");
							recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
							recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
							estructura_mensaje.socket_cliente = socket_cliente;
							estructura_mensaje.id_mensaje = id_mensaje;
							estructura_mensaje.id_proceso = id_proceso;
							estructura_mensaje.operacion = operacion;
							if (pthread_create(&hilo, NULL, (void*)manejoMensaje, &estructura_mensaje) == 0){
							}else printf("Fallo al crear el hilo que maneja el mensaje New\n");
							pthread_detach(hilo);
							break;
						case APPEARED_POKEMON:
							log_info(logger, "Llego un mensaje APPEARED_POKEMON");
							recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
							recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
							estructura_mensaje.socket_cliente = socket_cliente;
							estructura_mensaje.id_mensaje = id_mensaje;
							estructura_mensaje.id_proceso = id_proceso;
							estructura_mensaje.operacion = operacion;
							if (pthread_create(&hilo, NULL, (void*)manejoMensaje, &estructura_mensaje) == 0){
							}else printf("Fallo al crear el hilo que maneja el mensaje Appeared\n");
							pthread_detach(hilo);
							break;
						case GET_POKEMON:
							log_info(logger, "Llego un mensaje GET_POKEMON");
							recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
							recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
							estructura_mensaje.socket_cliente = socket_cliente;
							estructura_mensaje.id_mensaje = id_mensaje;
							estructura_mensaje.id_proceso = id_proceso;
							estructura_mensaje.operacion = operacion;
							if (pthread_create(&hilo, NULL, (void*)manejoMensaje, &estructura_mensaje) == 0){
							}else printf("Fallo al crear el hilo que maneja el mensaje Get\n");
							pthread_detach(hilo);
							break;
						case LOCALIZED_POKEMON:
							log_info(logger, "Llego un mensaje LOCALIZED_POKEMON");
							recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
							recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
							estructura_mensaje.socket_cliente = socket_cliente;
							estructura_mensaje.id_mensaje = id_mensaje;
							estructura_mensaje.id_proceso = id_proceso;
							estructura_mensaje.operacion = operacion;
							if (pthread_create(&hilo, NULL, (void*)manejoMensaje, &estructura_mensaje) == 0){
							}else printf("Fallo al crear el hilo que maneja el mensaje Localized\n");
							pthread_detach(hilo);
							break;
						case CATCH_POKEMON:
							log_info(logger, "Llego un mensaje CATCH_POKEMON");
							recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
							recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
							estructura_mensaje.socket_cliente = socket_cliente;
							estructura_mensaje.id_mensaje = id_mensaje;
							estructura_mensaje.id_proceso = id_proceso;
							estructura_mensaje.operacion = operacion;
							if (pthread_create(&hilo, NULL, (void*)manejoMensaje, &estructura_mensaje) == 0){
							}else printf("Fallo al crear el hilo que maneja el mensaje Catch\n");
							pthread_detach(hilo);
							break;
						case CAUGHT_POKEMON:
							log_info(logger, "Llego un mensaje CAUGHT_POKEMON");
							recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
							recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
							estructura_mensaje.socket_cliente = socket_cliente;
							estructura_mensaje.id_mensaje = id_mensaje;
							estructura_mensaje.id_proceso = id_proceso;
							estructura_mensaje.operacion = operacion;
							if (pthread_create(&hilo, NULL, (void*)manejoMensaje, &estructura_mensaje) == 0){
							}else printf("Fallo al crear el hilo que maneja el mensaje Caught\n");
							pthread_detach(hilo);
							break;
							}
						} else printf("Fallo al recibir codigo de operacion = -1\n");
				}else printf("El proceso no se identifico \n");
			}
			else printf("Fallo al recibir codigo de operacion = -1\n");
		}else {
			printf("Fallo al recibir/aceptar al cliente\n");
			sleep(5);
			close(socketEscucha);
			socketEscucha = crear_socket_escucha(IP_BROKER, PUERTO_BROKER);
		}
	sleep(0.25);
	}
	if(socketEscucha == -1){
		printf("Fallo al crear socket de escucha = -1\n");
		sleep(2);
	}

	return EXIT_SUCCESS;

}

int32_t get_id(){
	pthread_mutex_lock(&mutex_id_mensaje);
	++id_mensaje_global;
	pthread_mutex_unlock(&mutex_id_mensaje);
	return id_mensaje_global;
}

void manejoSuscripcion(t_estructura_hilo_suscriptor * estructura_suscriptor){
	int32_t socket_cliente = estructura_suscriptor->socket_cliente;
	int32_t id_proceso = estructura_suscriptor->id_proceso;
	int32_t suscripcion = estructura_suscriptor->operacion;

	t_suscriptor * suscriptor;
	info_mensaje * mensaje;
	t_list * mensajesAEnviar = NULL;
	int32_t tamanio_estructura = 0;
	int32_t id_mensaje;
	int32_t operacion;
	bool fin = false;
	int32_t id_msg_log;

	if(procesoSuscriptoACola(suscripcion, id_proceso)){
		suscriptor = obtenerSuscriptor(id_proceso);
	} else{
		suscriptor = malloc(sizeof(t_suscriptor));
		suscriptor->id = id_proceso;
		suscriptor->op_code = suscripcion;
		list_add(list_suscriptores, suscriptor);

	}
	char cola[9];
	switch(suscripcion){
	case SUSCRIPCION_NEW:
		strcpy(cola, "NEW");
		break;
	case SUSCRIPCION_APPEARED:
		strcpy(cola, "APPEARED");
		break;
	case SUSCRIPCION_GET:
		strcpy(cola, "GET");
		break;
	case SUSCRIPCION_LOCALIZED:
		strcpy(cola, "LOCALIZED");
		break;
	case SUSCRIPCION_CATCH:
		strcpy(cola, "CATCH");
		break;
	case SUSCRIPCION_CAUGHT:
		strcpy(cola, "CAUGHT");
		break;
	default:
		strcpy(cola, "VACIA");
		break;
	}
	log_info(logger, "Proceso suscripto a cola %s", cola);
	enviar_ACK(0, socket_cliente);

	while(fin == false){
		//chequear mensajes nuevos filtrados por operacion. Agregar semaforo?
		mensajesAEnviar = getMensajesAEnviar(suscripcion, id_proceso);
		if(mensajesAEnviar->elements_count > 0){
			for(int i=0; i<mensajesAEnviar->elements_count; i++){
					mensaje = list_get(mensajesAEnviar, i);
					enviarMensaje(suscripcion, mensaje, socket_cliente, id_proceso);
					if(esCorrelativo(mensaje->id_mensaje_correlativo)){
						id_msg_log = mensaje->id_mensaje_correlativo;
					}else id_msg_log = mensaje->id_mensaje;
					mensaje = obtenerMensaje(mensaje->id_mensaje);
					pthread_mutex_lock(&mutex_list_mensaje);
					list_add(mensaje->suscriptoresALosQueSeEnvio, suscriptor);
					pthread_mutex_unlock(&mutex_list_mensaje);
					//esperar ACK:
					if(recv(socket_cliente, &operacion, sizeof(int32_t), MSG_WAITALL) > 0){
						if(operacion == ACK){
							recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
							recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
							mensaje = obtenerMensaje(mensaje->id_mensaje);
							log_info(logger, "Se recibio el ACK del mensaje id: %d", id_msg_log);
							pthread_mutex_lock(&mutex_list_mensaje);
							list_add(mensaje->suscriptoresQueRecibieron, suscriptor);
							pthread_mutex_unlock(&mutex_list_mensaje);
						} else printf("Luego de enviar el mensaje devolvieron una operacion que no era ACK\n");
					} else {
						liberar_conexion(socket_cliente);
						list_destroy(mensajesAEnviar);
						printf("Fallo al recibir codigo de operacion = -1\n");
						fin = true;
						break;
					}
			}
			list_destroy(mensajesAEnviar);
		}else list_destroy(mensajesAEnviar);
	}
}

void manejoMensaje(t_estructura_hilo_mensaje * estructura_mensaje){
	int32_t socket_cliente = estructura_mensaje->socket_cliente;
	int32_t id_proceso = estructura_mensaje->id_proceso;
	int32_t id_mensaje = estructura_mensaje->id_mensaje;
	int32_t operacion = estructura_mensaje->operacion;
	info_mensaje * mensaje;

	switch(operacion){
	case NEW_POKEMON:
		mensaje = recibirMensajeNew(socket_cliente);
		break;
	case APPEARED_POKEMON:
		mensaje = recibirMensajeAppeared(socket_cliente);
		break;
	case GET_POKEMON:
		mensaje = recibirMensajeGet(socket_cliente);
		break;
	case LOCALIZED_POKEMON:
		mensaje = recibirMensajeLocalized(socket_cliente);
		break;
	case CATCH_POKEMON:
		mensaje = recibirMensajeCatch(socket_cliente);
		break;
	case CAUGHT_POKEMON:
		mensaje = recibirMensajeCaught(socket_cliente);
		break;
	default:
		break;
	}

	if(esCorrelativo(id_mensaje)){
		mensaje->id_mensaje_correlativo = id_mensaje;
	}

	pthread_mutex_lock(&mutex_guardar_en_memoria);
	guardarMensajeEnCache(mensaje);
	pthread_mutex_unlock(&mutex_guardar_en_memoria);

	mensaje->process_id=id_proceso;
	enviar_ACK(mensaje->id_mensaje, socket_cliente);
	liberar_conexion(socket_cliente);
}

void guardarMensajeEnCache(info_mensaje* mensaje){
	switch(algMemoria){
	case BS:
		algoritmoBuddySystem(mensaje, algReemplazo);
		break;
	case PARTICIONES:
		algoritmoParticionDinamica(mensaje, frecuenciaCompactacion, algReemplazo, algParticionLibre);
		break;
		}
}

info_mensaje * recibirMensajeNew(int32_t socket_cliente){
	t_New* new = NULL;
	new = deserializar_paquete_new (&socket_cliente);
	/*printf("Llego un mensaje New Pokemon con los siguientes datos: %d  %s  %d  %d  %d \n", new->pokemon.size_Nombre, new->pokemon.nombre,
			new->cant, new->posicion.X, new->posicion.Y);*/
	info_mensaje * mensajeNew = malloc(sizeof(info_mensaje));
	mensajeNew->op_code = NEW_POKEMON;
	mensajeNew->id_mensaje = get_id();
	mensajeNew->id_mensaje_correlativo = 0;
	mensajeNew->process_id = 1;
	mensajeNew->mensaje = new;
	mensajeNew->sizeMsg = getSizeMensajeNew(*new);
	mensajeNew->suscriptoresALosQueSeEnvio = list_create();
	mensajeNew->suscriptoresALosQueSeEnvio->elements_count=0;
	mensajeNew->suscriptoresQueRecibieron = list_create();
	mensajeNew->suscriptoresQueRecibieron->elements_count=0;
	pthread_mutex_lock(&mutex_list_mensaje);
	list_add(list_mensajes, mensajeNew);
	pthread_mutex_unlock(&mutex_list_mensaje);
	return mensajeNew;
}


info_mensaje * recibirMensajeAppeared(int32_t socket_cliente){
	t_Appeared* app = NULL;
	app = deserializar_paquete_appeared(&socket_cliente);
	/*printf("Llego un mensaje Appeared Pokemon con los siguientes datos: %d  %s  %d  %d\n", app->pokemon.size_Nombre, app->pokemon.nombre,
			app->posicion.X, app->posicion.Y);*/
	info_mensaje * mensajeAppeared = malloc(sizeof(info_mensaje));
	mensajeAppeared->op_code = APPEARED_POKEMON;
	mensajeAppeared->id_mensaje = get_id();
	mensajeAppeared->id_mensaje_correlativo = 0;
	mensajeAppeared->process_id = 1;
	mensajeAppeared->mensaje = app;
	mensajeAppeared->sizeMsg = getSizeMensajeAppeared(*app);
	mensajeAppeared->suscriptoresALosQueSeEnvio = list_create();
	mensajeAppeared->suscriptoresALosQueSeEnvio->elements_count=0;
	mensajeAppeared->suscriptoresQueRecibieron = list_create();
	mensajeAppeared->suscriptoresQueRecibieron->elements_count=0;
	pthread_mutex_lock(&mutex_list_mensaje);
	list_add(list_mensajes, mensajeAppeared);
	pthread_mutex_unlock(&mutex_list_mensaje);
	return mensajeAppeared;
}

info_mensaje * recibirMensajeGet(int32_t socket_cliente){
	t_Get* get = NULL;
	get = deserializar_paquete_get(&socket_cliente);
	/*printf("Llego un mensaje Get Pokemon con los siguientes datos: %d  %s\n", get->pokemon.size_Nombre, get->pokemon.nombre);*/
	info_mensaje * mensajeGet = malloc(sizeof(info_mensaje));
	mensajeGet->op_code = GET_POKEMON;
	mensajeGet->id_mensaje = get_id();
	mensajeGet->id_mensaje_correlativo = 0;
	mensajeGet->process_id = 1;
	mensajeGet->mensaje = get;
	mensajeGet->sizeMsg = getSizeMensajeGet(*get);
	mensajeGet->suscriptoresALosQueSeEnvio = list_create();
	mensajeGet->suscriptoresALosQueSeEnvio->elements_count=0;
	mensajeGet->suscriptoresQueRecibieron = list_create();
	mensajeGet->suscriptoresQueRecibieron->elements_count=0;
	pthread_mutex_lock(&mutex_list_mensaje);
	list_add(list_mensajes, mensajeGet);
	pthread_mutex_unlock(&mutex_list_mensaje);
	return mensajeGet;
}

info_mensaje * recibirMensajeLocalized(int32_t socket_cliente){
	t_Localized* localized = NULL;
	localized = deserializar_paquete_localized(&socket_cliente);

	info_mensaje * mensajeLocalized = malloc(sizeof(info_mensaje));
	mensajeLocalized->op_code = LOCALIZED_POKEMON;
	mensajeLocalized->id_mensaje = get_id();
	mensajeLocalized->id_mensaje_correlativo = 0;
	mensajeLocalized->process_id = 1;
	mensajeLocalized->mensaje = localized;
	mensajeLocalized->sizeMsg = getSizeMensajeLocalized(*localized);
	mensajeLocalized->suscriptoresALosQueSeEnvio = list_create();
	mensajeLocalized->suscriptoresALosQueSeEnvio->elements_count=0;
	mensajeLocalized->suscriptoresQueRecibieron = list_create();
	mensajeLocalized->suscriptoresQueRecibieron->elements_count=0;
	pthread_mutex_lock(&mutex_list_mensaje);
	list_add(list_mensajes, mensajeLocalized);
	pthread_mutex_unlock(&mutex_list_mensaje);
	return mensajeLocalized;
}

info_mensaje * recibirMensajeCatch(int32_t socket_cliente){
	t_Catch* catch = NULL;
	catch = deserializar_paquete_catch(&socket_cliente);
	/*printf("Llego un mensaje Catch Pokemon con los siguientes datos: %d  %s  %d  %d \n", catch->pokemon.size_Nombre, catch->pokemon.nombre,
			catch->posicion.X, catch->posicion.Y);*/
	info_mensaje * mensajeCatch = malloc(sizeof(info_mensaje));
	mensajeCatch->op_code = CATCH_POKEMON;
	mensajeCatch->id_mensaje = get_id();
	mensajeCatch->id_mensaje_correlativo = 0;
	mensajeCatch->process_id = 1;
	mensajeCatch->mensaje = catch;
	mensajeCatch->sizeMsg = getSizeMensajeCatch(*catch);
	mensajeCatch->suscriptoresALosQueSeEnvio = list_create();
	mensajeCatch->suscriptoresALosQueSeEnvio->elements_count=0;
	mensajeCatch->suscriptoresQueRecibieron = list_create();
	mensajeCatch->suscriptoresQueRecibieron->elements_count=0;
	pthread_mutex_lock(&mutex_list_mensaje);
	list_add(list_mensajes, mensajeCatch);
	pthread_mutex_unlock(&mutex_list_mensaje);
	return mensajeCatch;
}

info_mensaje * recibirMensajeCaught(int32_t socket_cliente){
	t_Caught* caught = NULL;
	caught = deserializar_paquete_caught(&socket_cliente);
	/*printf("Llego un mensaje Caught Pokemon con los siguientes datos:  %d\n", caught->fueAtrapado);*/
	info_mensaje * mensajeCaught = malloc(sizeof(info_mensaje));
	mensajeCaught->op_code = CAUGHT_POKEMON;
	mensajeCaught->id_mensaje = get_id();
	mensajeCaught->id_mensaje_correlativo = 0;
	mensajeCaught->process_id = 1;
	mensajeCaught->mensaje = caught;
	mensajeCaught->sizeMsg = getSizeMensajeCaught(*caught);
	mensajeCaught->suscriptoresALosQueSeEnvio = list_create();
	mensajeCaught->suscriptoresALosQueSeEnvio->elements_count=0;
	mensajeCaught->suscriptoresQueRecibieron = list_create();
	mensajeCaught->suscriptoresQueRecibieron->elements_count=0;
	pthread_mutex_lock(&mutex_list_mensaje);
	list_add(list_mensajes, mensajeCaught);
	pthread_mutex_unlock(&mutex_list_mensaje);
	return mensajeCaught;
}

void enviarMensaje(op_code operacion, info_mensaje * mensaje, int32_t socket_cliente, int32_t id_proceso){
	int32_t id_mensaje;
	t_New * new;
	t_Appeared * app;
	t_Get * get;
	t_Localized * loc;
	t_Catch * catch;
	t_Caught * caught;

	if(esCorrelativo(mensaje->id_mensaje_correlativo)){
		id_mensaje = mensaje->id_mensaje_correlativo;
	}else id_mensaje = mensaje->id_mensaje;

	switch(operacion){
	case SUSCRIPCION_NEW:
		new = mensaje->mensaje;
		enviar_new_pokemon(new->pokemon.nombre,string_itoa(new->posicion.X),string_itoa(new->posicion.Y),string_itoa(new->cant),string_itoa(id_mensaje), socket_cliente);
		log_info(logger, "Se envio un mensaje NEW_POKEMON con id: %d al proceso %d", id_mensaje, id_proceso);
		if(algReemplazo==LRU) actualizarID(mensaje->id_mensaje);
		free(new);
		break;
	case SUSCRIPCION_APPEARED:
		app = mensaje->mensaje;
		enviar_appeared_pokemon(app->pokemon.nombre, string_itoa(app->posicion.X), string_itoa(app->posicion.Y), string_itoa(id_mensaje), socket_cliente);
		log_info(logger, "Se envio un mensaje APPEARED_POKEMON con id: %d al proceso %d", id_mensaje, id_proceso);
		if(algReemplazo==LRU) actualizarID(mensaje->id_mensaje);
		break;
	case SUSCRIPCION_GET:
		get = mensaje->mensaje;
		enviar_get_pokemon(get->pokemon.nombre,string_itoa(id_mensaje), socket_cliente);
		log_info(logger, "Se envio un mensaje GET_POKEMON con id: %d al proceso %d", id_mensaje, id_proceso);
		if(algReemplazo==LRU) actualizarID(mensaje->id_mensaje);
		break;
	case SUSCRIPCION_LOCALIZED:
		loc = mensaje->mensaje;
		enviar_localized_pokemon(&(loc->pokemon), loc->listaPosiciones, id_mensaje, socket_cliente);
		log_info(logger, "Se envio un mensaje LOCALIZED con id: %d al proceso %d", id_mensaje, id_proceso);
		if(algReemplazo==LRU) actualizarID(mensaje->id_mensaje);
		break;
	case SUSCRIPCION_CATCH:
		catch = mensaje->mensaje;
		enviar_catch_pokemon(catch->pokemon.nombre, string_itoa(catch->posicion.X), string_itoa(catch->posicion.Y), string_itoa(id_mensaje), socket_cliente);
		log_info(logger, "Se envio un mensaje CATCH_POKEMON con id: %d al proceso %d", id_mensaje, id_proceso);
		if(algReemplazo==LRU) actualizarID(mensaje->id_mensaje);
		break;
	case SUSCRIPCION_CAUGHT:
		caught = mensaje->mensaje;
		enviar_caught_pokemon(string_itoa(id_mensaje), string_itoa(caught->fueAtrapado), socket_cliente);
		log_info(logger, "Se envio un mensaje CAUGHT_POKEMON con id: %d al proceso %d", id_mensaje, id_proceso);
		if(algReemplazo==LRU) actualizarID(mensaje->id_mensaje);
		break;
	default:
		break;
	}
}

bool esCorrelativo(int32_t id_mensaje){
	if(id_mensaje != 0){
		return true;
	}
	return false;
}

void iniciarBroker(){
	logger = iniciar_logger();
	config = leer_config();

	log_info(logger, "Iniciando Broker");
	unsigned int miPid= process_getpid();
	log_info(logger, "El PID de Broker es %d", miPid);

	//Lectura de archivo de configuracion
	sizeMemoria = atoi(config_get_string_value(config, "TAMANO_MEMORIA"));
	sizeMinParticion = atoi(config_get_string_value(config, "TAMANO_MINIMO_PARTICION"));

	char * value;
	value= config_get_string_value(config, "ALGORITMO_MEMORIA");
	if(strcmp(value,"BS") == 0){
		algMemoria=BS;
	}else algMemoria=PARTICIONES;

	value = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
	if(strcmp(value,"FIFO") == 0){
		algReemplazo=FIFO;
	}else algReemplazo=LRU;

	value = config_get_string_value(config, "ALGORITMO_PARTICION_LIBRE");
	if(strcmp(value,"FF") == 0){
		algParticionLibre=FF;
	}else algParticionLibre=BF;

	IP_BROKER = config_get_string_value(config, "IP_BROKER");
	PUERTO_BROKER = config_get_string_value(config, "PUERTO_BROKER");
	frecuenciaCompactacion = atoi(config_get_string_value(config, "FRECUENCIA_COMPACTACION"));
	LOG_FILE = config_get_string_value(config, "LOG_FILE");

	//Inicializar memoria
	inicioMemoria = (int32_t)malloc(sizeMemoria); //f00X12345  f00X12345 + 2048
	t_particion* particionInicial = crearParticion(0, sizeMemoria, false);
	tabla_particiones = list_create();
	list_add(tabla_particiones, particionInicial);

	//Crear lista de suscriptores y mensajes
	list_suscriptores = list_create();
	list_mensajes = list_create();

}

t_log* iniciar_logger(void){
	t_log* logger;
	logger = log_create("/home/utnso/workspace/tp-2020-1c-5rona/Broker/Broker.log", "Broker", 1, LOG_LEVEL_INFO);
	if(logger == NULL){
		printf("No pude iniciar el logger\n");
		exit(1);
	}
	log_info(logger, "Inicio log");
	return logger;
}

t_config * leer_config(void){
	t_config * config;
	config = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Broker/Broker.config");
	if(config== NULL){
		printf("No pude leer la config\n");
		exit(2);
	}
	return config;
}

int getMemoriaOcupada(){
	int memoriaOcupada = 0;

	int i = 0;
	for(i = 0; i < tabla_particiones->elements_count; i++){
		t_particion* particion_actual = list_get(tabla_particiones, i);
		memoriaOcupada += particion_actual->size;
	}

	return memoriaOcupada;
}

int getMemoriaDisponible(){
	return sizeMemoria - getMemoriaOcupada();
}

int32_t getSizePokemon(t_pokemon pokemon){
	return sizeof(typeof(pokemon.size_Nombre)) + sizeof(typeof(char)) * pokemon.size_Nombre -1; //-1 para restar el centinela
}

int32_t getSizeMensajeNew(t_New msgNew){
	/*
	 * ‘Pikachu’ 5 10 2
	 * largo del nombre del pokémon
	 * el nombre del pokemon
	 * posición X
 	 * posicion Y
	 * cantidad
	*/
	int32_t sizeMsg = 0;
	sizeMsg +=getSizePokemon(msgNew.pokemon) +
			sizeof(typeof(t_posicion)) +
			sizeof(typeof(msgNew.cant));

	return sizeMsg;
}

int32_t getSizeMensajeLocalized(t_Localized msgLocalized){
	/*
	 * ‘Pikachu’ 3 4 5 1 5 9 3
	 * el largo del nombre del pokémon,
	 * el nombre del pokemon,
	 * la cantidad de posiciones donde se encuentra
	 * y un par de int_32 para cada posición donde se encuentre. (2 * int_32 * cant_posiciones)
	*/

	int32_t sizeMsg = 0;
	sizeMsg += getSizePokemon(msgLocalized.pokemon) + sizeof(typeof(msgLocalized.listaPosiciones->elements_count));
	if(msgLocalized.listaPosiciones->head == 0){
		sizeMsg += sizeof(msgLocalized.listaPosiciones->head);
	}else sizeMsg += sizeof(typeof(t_posicion)) * msgLocalized.listaPosiciones->elements_count;

	return sizeMsg;
}

int32_t getSizeMensajeGet(t_Get msgGet){
	/*
	 * ‘Pikachu’
	 * el largo del nombre del pokémon,
	 * el nombre del pokemon,
	*/

	int32_t sizeMsg = 0;
	sizeMsg += getSizePokemon(msgGet.pokemon);

	return sizeMsg;
}

int32_t getSizeMensajeAppeared(t_Appeared msgAppeared){
	/*
	 * ‘Pikachu’ 1 5
	 * el largo del nombre del pokémon,
	 * el nombre del pokemon,
	 * Pos X
	 * Pos Y
	*/

	int32_t sizeMsg = 0;
	sizeMsg += getSizePokemon(msgAppeared.pokemon)+
				  sizeof(typeof(t_posicion));

	return sizeMsg;
}

int32_t getSizeMensajeCatch(t_Catch msgCatch){
	/*
	 * ‘Pikachu’ 1 5
	 * el largo del nombre del pokémon,
	 * el nombre del pokemon,
	 * Pos X
	 * Pos Y
	*/

	int32_t sizeMsg = 0;
	sizeMsg += getSizePokemon(msgCatch.pokemon)+
				  sizeof(typeof(t_posicion));

	return sizeMsg;
}

int32_t getSizeMensajeCaught(t_Caught msgCaught){
	/*
	 * 0
	 * un uint_32 para saber si se puedo o no atrapar al pokemon
	*/

	return sizeof(typeof(sizeof(msgCaught.fueAtrapado)));
}

void rutina (int n){
	dump = iniciar_dump();
	if(n == SIGUSR1){
		pthread_mutex_lock(&mutex_guardar_en_memoria);
		hacerDump();
		pthread_mutex_unlock(&mutex_guardar_en_memoria);
	}else printf("Signal no reconocida. \n");
}


t_log* iniciar_dump(void){
	t_log* dump;
	dump = log_create("/home/utnso/workspace/tp-2020-1c-5rona/Broker/Broker-dump.log", "Broker", 1, LOG_LEVEL_INFO);
	if(dump == NULL){
		printf("No pude iniciar el dump\n");
		exit(1);
	}
	return dump;
}

void hacerDump(){
	t_particion *particion;
    time_t tiempo = time(0);
    struct tm *tlocal = localtime(&tiempo);
    char fecha[128];
    strftime(fecha,128,"%d/%m/%y %H:%M:%S",tlocal);

	log_info(dump, "Dump: %s", fecha);
	int i;
	for(i=0; i<tabla_particiones->elements_count; i++){
		particion = list_get(tabla_particiones, i);
		char estado[] = "L";
		if(particion->ocupada == 1) estado[0] = 'X';
		char cola[9];
		switch(particion->codigo_operacion){
		case NEW_POKEMON:
			strcpy(cola, "NEW");
			break;
		case APPEARED_POKEMON:
			strcpy(cola, "APPEARED");
			break;
		case GET_POKEMON:
			strcpy(cola, "GET");
			break;
		case LOCALIZED_POKEMON:
			strcpy(cola, "LOCALIZED");
			break;
		case CATCH_POKEMON:
			strcpy(cola, "CATCH");
			break;
		case CAUGHT_POKEMON:
			strcpy(cola, "CAUGHT");
			break;
		default:
			strcpy(cola, "VACIA");
			break;
		}
		log_info(dump, "Particion %d: %p - %p.	[%s]\n Size: %db	LRU: %d		COLA: %s		ID_MENSAJE: %d",
				i, inicioMemoria + particion->posicion_inicial, inicioMemoria + particion->posicion_final, estado, particion->size, particion->id,
				cola, particion->id_mensaje);
		//%06p
	}
}

t_list * getMensajesAEnviar(op_code operacion, int32_t id_proceso){
	info_mensaje * mensaje;
	t_particion * mensajeCacheado;
	t_list* mensajesAEnviar=list_create();
	mensajesAEnviar->elements_count=0;
	op_code tipoMensajeABuscar;
	switch(operacion){
	case SUSCRIPCION_NEW:
		tipoMensajeABuscar =NEW_POKEMON;
		break;
	case SUSCRIPCION_APPEARED:
		tipoMensajeABuscar =APPEARED_POKEMON;
		break;
	case SUSCRIPCION_CATCH:
		tipoMensajeABuscar =CATCH_POKEMON;
		break;
	case SUSCRIPCION_CAUGHT:
		tipoMensajeABuscar =CAUGHT_POKEMON;
		break;
	case SUSCRIPCION_GET:
		tipoMensajeABuscar =GET_POKEMON;
		break;
	case SUSCRIPCION_LOCALIZED:
		tipoMensajeABuscar =LOCALIZED_POKEMON;
		break;
	default:
		break;
	}
	pthread_mutex_lock(&mutex_guardar_en_memoria);
	t_list* mensajesCacheados = getMensajesCacheadosDeOperacion(tipoMensajeABuscar); // mensajesCacheadoes es una lista de t_particion

	for(int i=0; i<mensajesCacheados->elements_count; i++){
		mensajeCacheado = list_get(mensajesCacheados, i);
		if(!recibioMensaje(id_proceso, mensajeCacheado->id_mensaje)){
			mensaje = obtenerMensaje(mensajeCacheado->id_mensaje);
			list_add(mensajesAEnviar, mensaje);
		}
	}
	pthread_mutex_unlock(&mutex_guardar_en_memoria);
	list_destroy(mensajesCacheados);
	return mensajesAEnviar;
}

bool recibioMensaje(int32_t id_proceso, int32_t id_mensaje){
	info_mensaje * mensaje;
	t_suscriptor * suscriptor;
	mensaje = obtenerMensaje(id_mensaje); // obtenemos el info_mensaje de list_mensajes

	for(int j=0; j<mensaje->suscriptoresQueRecibieron->elements_count; j++){
		suscriptor = list_get(mensaje->suscriptoresQueRecibieron, j);

		if(suscriptor->id == id_proceso){
			return true;
		}
	}
	return false;
}

bool mensajeCacheadoDeOperacion(t_particion * particion, op_code operacion){
	return particion->codigo_operacion == operacion;
}

t_list* getMensajesCacheadosDeOperacion(op_code operacion){

	bool _mensajeCacheadoDeOperacion(void* element){
		return mensajeCacheadoDeOperacion((t_particion*)element, operacion);
	}
	t_list* mensajesCacheados = list_filter(tabla_particiones, _mensajeCacheadoDeOperacion);

	return mensajesCacheados;
}

bool esElMensaje(info_mensaje* mensaje, int32_t id_mensaje){
	return mensaje->id_mensaje == id_mensaje;
}

info_mensaje * obtenerMensaje(int32_t id_mensaje){
	info_mensaje * mensaje = NULL;

	bool _esElMensaje(void* element){
		return esElMensaje((info_mensaje*)element, id_mensaje);
	}
	pthread_mutex_lock(&mutex_list_mensaje);
	t_list* mensajesConEseID = list_filter(list_mensajes, _esElMensaje);
	pthread_mutex_unlock(&mutex_list_mensaje);

	if(mensajesConEseID->elements_count == 1){
		mensaje = list_get(mensajesConEseID, 0);
		list_destroy(mensajesConEseID);
		return mensaje;
	}else {
		if(mensajesConEseID->elements_count > 1){
			printf("Mas de un mensaje con el mismo id. Cantidad: %d \n", mensajesConEseID->elements_count);
			list_destroy(mensajesConEseID);
			return NULL;
		}else printf("No estaba el mensaje en memoria cache (particiones)");
		list_destroy(mensajesConEseID);
		return NULL;
	}
}

bool esElSuscriptor(t_suscriptor * suscriptor, int32_t id_proceso){
	return suscriptor->id == id_proceso;
}

bool procesoSuscriptoACola(op_code operacion, int32_t id_proceso){
	bool _esElSuscriptor(void* element){
		return esElSuscriptor((t_suscriptor*)element, id_proceso);
	}

	t_list * suscriptor = list_filter(list_suscriptores, _esElSuscriptor);
	if(suscriptor->elements_count >1){
		list_destroy(suscriptor);
		printf("Estaba suscripto mas de una vez a la misma cola \n");
		return -1;
	}else {
		bool resultado = suscriptor->elements_count;
		list_destroy(suscriptor);
		return resultado;
	}
}

t_suscriptor * obtenerSuscriptor(int32_t id_proceso){
	t_suscriptor * suscriptor = NULL;
	bool _esElSuscriptor(void* element){
		return esElSuscriptor((t_suscriptor*)element, id_proceso);
	}

	t_list * suscriptores = list_filter(list_suscriptores, _esElSuscriptor);
	if(suscriptores->elements_count >1){
		printf("Estaba suscripto mas de una vez a la misma cola \n");
		return suscriptor;
	}else {
		suscriptor = list_get(suscriptores, 0);
		return suscriptor;
	}
}



