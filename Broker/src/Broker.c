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
//#include "pruebas.h"
#include "Particiones.h"
#include "../../OurLibraries/Sockets/mensajes.h"

int32_t id_mensaje_global = 0;
pthread_mutex_t mutex_id_mensaje;

pthread_mutex_t mutex_guardar_en_memoria;

int32_t main(void) {
	iniciarBroker();
	pthread_mutex_init(&mutex_guardar_en_memoria, NULL);

	signal(SIGUSR1, rutina);

	int32_t socketEscucha = crear_socket_escucha(IP_BROKER, PUERTO_BROKER);
	log_info(logger, "Creado socket de escucha \n");

	while(socketEscucha != -1){
		int32_t socket_cliente = (int32_t)recibir_cliente(socketEscucha);

		if(socket_cliente != -1){
			log_info(logger, "Se conecto un proceso \n");

			int32_t tamanio_estructura = 0;
			int32_t id_mensaje=0;
			int32_t operacion=0;
			pthread_t hilo;
			int32_t id_proceso =0;
			info_mensaje * mensaje;

			//HANDSHAKE
			if(recv(socket_cliente, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
				if(operacion == HANDSHAKE){
					recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
					recv(socket_cliente, &id_proceso, sizeof(int32_t), MSG_WAITALL);
					//ACK DEL HANDSHAKE
					enviar_ACK(0, socket_cliente);
					//ESPERA EL MENSAJE
					if(recv(socket_cliente, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
						switch(operacion){
						//case SUSCRIPCION_NEW:
							/*printf("new suscriptor \n");
							liberar_conexion(socket_cliente);
							break;*/
						case SUSCRIPCION_APPEARED:
						case SUSCRIPCION_GET:
						case SUSCRIPCION_LOCALIZED:
						case SUSCRIPCION_CATCH:
						case SUSCRIPCION_CAUGHT:
						case SUSCRIPCION_NEW:
							if(procesoSuscriptoACola(operacion, id_proceso)){ // si es un proceso que ya se suscribio
								recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
								recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
								recibirSuscripcionYaExistente(socket_cliente, id_proceso, operacion);
							} else{
								recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
								recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
								recibirSuscripcionNueva(socket_cliente, id_proceso, operacion);
								}
						break;
						case NEW_POKEMON:
							log_info(logger, "Llego un mensaje NEW_POKEMON \n");
							recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
							recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
							mensaje = recibirMensajeNew(socket_cliente);
							if(esCorrelativo(id_mensaje)){
								mensaje->id_mensaje_correlativo = id_mensaje;
							}
							mensaje->process_id=id_proceso;
							//CONFIRMAR RECEPCION DEL MENSAJE
							enviar_ACK(mensaje->id_mensaje, socket_cliente);
							if (pthread_create(&hilo, NULL, (void*)manejoMensaje, mensaje) == 0){
								printf("Creado el hilo que maneja el mensaje New\n");
							}else printf("Fallo al crear el hilo que maneja el mensaje New\n");
							pthread_detach(hilo);
							//info_mensaje * mensaje;
							//mensaje = obtenerMensaje(particion->codigo_operacion, particion->id_mensaje);
							break;
						case APPEARED_POKEMON:
							log_info(logger, "Llego un mensaje APPEARED_POKEMON \n");
							recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
							recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
							mensaje = recibirMensajeAppeared(socket_cliente);
							if(esCorrelativo(id_mensaje)){
								mensaje->id_mensaje_correlativo = id_mensaje;
							}
							mensaje->process_id=id_proceso;
							enviar_ACK(mensaje->id_mensaje, socket_cliente);
							if (pthread_create(&hilo, NULL, (void*)manejoMensaje, mensaje) == 0){
								printf("Creado el hilo que maneja el mensaje Appeared\n");
							}else printf("Fallo al crear el hilo que maneja el mensaje Appeared\n");
							pthread_detach(hilo);
							//enviar suscriptores
							break;
						case GET_POKEMON:
							log_info(logger, "Llego un mensaje GET_POKEMON \n");
							recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
							recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
							mensaje = recibirMensajeGet(socket_cliente);
							if(esCorrelativo(id_mensaje)){
								mensaje->id_mensaje_correlativo = id_mensaje;
							}
							mensaje->process_id=id_proceso;
							enviar_ACK(mensaje->id_mensaje, socket_cliente);
							if (pthread_create(&hilo, NULL, (void*)manejoMensaje, mensaje) == 0){
								printf("Creado el hilo que maneja el mensaje Get\n");
							}else printf("Fallo al crear el hilo que maneja el mensaje Get\n");
							pthread_detach(hilo);
							break;
						case LOCALIZED_POKEMON:
							log_info(logger, "Llego un mensaje LOCALIZED_POKEMON \n");
							recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
							recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
							mensaje = recibirMensajeLocalized(socket_cliente);
							if(esCorrelativo(id_mensaje)){
								mensaje->id_mensaje_correlativo = id_mensaje;
							}
							mensaje->process_id=id_proceso;
							enviar_ACK(mensaje->id_mensaje, socket_cliente);
							if (pthread_create(&hilo, NULL, (void*)manejoMensaje, mensaje) == 0){
								printf("Creado el hilo que maneja el mensaje Localized\n");
							}else printf("Fallo al crear el hilo que maneja el mensaje Localized\n");
							pthread_detach(hilo);
							break;
						case CATCH_POKEMON:
							log_info(logger, "Llego un mensaje CATCH_POKEMON \n");
							recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
							recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
							mensaje = recibirMensajeCatch(socket_cliente);
							if(esCorrelativo(id_mensaje)){
								mensaje->id_mensaje_correlativo = id_mensaje;
							}
							mensaje->process_id=id_proceso;
							enviar_ACK(mensaje->id_mensaje, socket_cliente);
							if (pthread_create(&hilo, NULL, (void*)manejoMensaje, mensaje) == 0){
								printf("Creado el hilo que maneja el mensaje Catch\n");
							}else printf("Fallo al crear el hilo que maneja el mensaje Catch\n");
							pthread_detach(hilo);
							break;
						case CAUGHT_POKEMON:
							log_info(logger, "Llego un mensaje CAUGHT_POKEMON \n");
							recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
							recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
							mensaje = recibirMensajeCaught(socket_cliente);
							if(esCorrelativo(id_mensaje)){
								mensaje->id_mensaje_correlativo = id_mensaje;
							}
							mensaje->process_id=id_proceso;
							enviar_ACK(mensaje->id_mensaje, socket_cliente);
							if (pthread_create(&hilo, NULL, (void*)manejoMensaje, mensaje) == 0){
								printf("Creado el hilo que maneja el mensaje Caught\n");
							}else printf("Fallo al crear el hilo que maneja el mensaje Caught\n");
							pthread_detach(hilo);
							break;
							}
						} else printf("Fallo al recibir codigo de operacion = -1\n");
				}else printf("El proceso no se identifico \n");
			}
			else printf("Fallo al recibir codigo de operacion = -1\n");
			liberar_conexion(socket_cliente);
		}else {
			printf("Fallo al recibir/aceptar al cliente\n");
			liberar_conexion(socket_cliente);
			sleep(2);
		}
	}
	if(socketEscucha == -1){
		printf("Fallo al crear socket de escucha = -1\n");
		sleep(2);
		//return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;

}

int32_t get_id(){
	pthread_mutex_lock(&mutex_id_mensaje);
	++id_mensaje_global;
	pthread_mutex_unlock(&mutex_id_mensaje);
	return id_mensaje_global;
}

void recibirSuscripcionNueva(int32_t socket_cliente, int32_t id_proceso, int32_t operacion){
	t_suscriptor * suscriptor;
	info_mensaje * mensaje;
	t_list * mensajesAEnviar = NULL;
	int32_t tamanio_estructura = 0;

	//suscripcion = deserializar_paquete_suscripcion(&socket_cliente);

	suscriptor = malloc(sizeof(t_suscriptor));
	suscriptor->id = id_proceso;
	suscriptor->op_code = operacion;
	list_add(list_suscriptores, suscriptor);

	mensajesAEnviar = getMensajesAEnviar(operacion, id_proceso);
	enviar_ACK(mensajesAEnviar->elements_count, socket_cliente);
	//Aca el suscriptor va a saber por el ID la cantidad de mensajes que Broker le va a enviar
	for(int i=0; i<mensajesAEnviar->elements_count; i++){
			mensaje = list_get(mensajesAEnviar, i);
			enviarMensaje(operacion, mensaje, socket_cliente);
			mensaje = obtenerMensaje(mensaje->id_mensaje);
			list_add(mensaje->suscriptoresALosQueSeEnvio, suscriptor);
			//esperar ACK:
			if(recv(socket_cliente, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
				if(operacion == ACK){
					recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
					recv(socket_cliente, &id_proceso, sizeof(int32_t), MSG_WAITALL);
					mensaje = obtenerMensaje(mensaje->id_mensaje);
					list_add(mensaje->suscriptoresQueRecibieron, suscriptor);
					//APPEARED
					// ENVIO APPEARED A TEAM
					//
				} else printf("Luego de enviar el mensaje devolvieron una operacion que no era ACK\n");
			} else printf("Fallo al recibir codigo de operacion = -1\n");
			//agregar suscriptor en info_mensaje.suscriptoresALosQueSeEnvio
		}
}

void recibirSuscripcionYaExistente(int32_t socket_cliente, int32_t id_proceso, int32_t operacion){
	t_suscriptor * suscriptor;
	info_mensaje * mensaje;
	t_list * mensajesAEnviar = NULL;
	int32_t tamanio_estructura = 0;

	//actualizar IP y PUERTO del id_proceso que ya estaba suscripto

	mensajesAEnviar = getMensajesAEnviar(operacion, id_proceso);
	enviar_ACK(mensajesAEnviar->elements_count, socket_cliente); //Aca el suscriptor va a saber por el ID la cantidad de mensajes que Broker le va a enviar
	for(int i = 0; i<mensajesAEnviar->elements_count; i++){
		mensaje = list_get(mensajesAEnviar, i);
		enviarMensaje(operacion, mensaje, socket_cliente);
		mensaje = obtenerMensaje(mensaje->id_mensaje);
		suscriptor = obtenerSuscriptor(id_proceso);
		list_add(mensaje->suscriptoresALosQueSeEnvio, suscriptor);
		//esperar ACK:
		if(recv(socket_cliente, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
			if(operacion == ACK){
				recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
				recv(socket_cliente, &id_proceso, sizeof(int32_t), MSG_WAITALL);
				list_add(mensaje->suscriptoresQueRecibieron, suscriptor);
				//agregar suscriptor en suscriptoresQueRecibieron
			} else printf("Luego de enviar el mensaje devolvieron una operacion que no era ACK\n");
		} else printf("Fallo al recibir codigo de operacion = -1\n");
	} list_destroy(mensajesAEnviar);

}

void manejoMensaje(info_mensaje* mensaje){
	pthread_mutex_lock(&mutex_guardar_en_memoria);
	switch(algMemoria){
	case BS:
		algoritmoBuddySystem(mensaje, algReemplazo);
		break;
	case PARTICIONES:
		algoritmoParticionDinamica(mensaje, frecuenciaCompactacion, algReemplazo, algParticionLibre);
		break;
		}
	pthread_mutex_unlock(&mutex_guardar_en_memoria);
	//pruebaMostrarEstadoMemoria();
	//opcional: informar a todos los suscriptores (definir si esto se hace aca y se crea un hilo para esperar el ACK
	// o se hace en otro hilo)
}

info_mensaje * recibirMensajeNew(int32_t socket_cliente){
	t_New* new = NULL;
	new = deserializar_paquete_new (&socket_cliente);
	printf("Llego un mensaje New Pokemon con los siguientes datos: %d  %s  %d  %d  %d \n", new->pokemon.size_Nombre, new->pokemon.nombre,
			new->cant, new->posicion.X, new->posicion.Y);
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
	list_add(list_mensajes, mensajeNew);
	return mensajeNew;
}


info_mensaje * recibirMensajeAppeared(int32_t socket_cliente){
	t_Appeared* app = NULL;
	app = deserializar_paquete_appeared(&socket_cliente);
	printf("Llego un mensaje Appeared Pokemon con los siguientes datos: %d  %s  %d  %d\n", app->pokemon.size_Nombre, app->pokemon.nombre,
			app->posicion.X, app->posicion.Y);
	info_mensaje * mensajeAppeared = malloc(sizeof(info_mensaje));
	mensajeAppeared->op_code = APPEARED_POKEMON;
	mensajeAppeared->id_mensaje = get_id();
	mensajeAppeared->id_mensaje_correlativo = 0;
	mensajeAppeared->process_id = 1;
	mensajeAppeared->mensaje = app;
	mensajeAppeared->sizeMsg = getSizeMensajeAppeared(*app);
	list_add(list_mensajes, mensajeAppeared);
	return mensajeAppeared;
}

info_mensaje * recibirMensajeGet(int32_t socket_cliente){
	t_Get* get = NULL;
	get = deserializar_paquete_get(&socket_cliente);
	printf("Llego un mensaje Get Pokemon con los siguientes datos: %d  %s\n", get->pokemon.size_Nombre, get->pokemon.nombre);
	info_mensaje * mensajeGet = malloc(sizeof(info_mensaje));
	mensajeGet->op_code = GET_POKEMON;
	mensajeGet->id_mensaje = get_id();
	mensajeGet->id_mensaje_correlativo = 0;
	mensajeGet->process_id = 1;
	mensajeGet->mensaje = get;
	mensajeGet->sizeMsg = getSizeMensajeGet(*get);
	list_add(list_mensajes, mensajeGet);
	return mensajeGet;
}

info_mensaje * recibirMensajeLocalized(int32_t socket_cliente){
	t_Localized* localized = NULL;
	localized = deserializar_paquete_localized(&socket_cliente);
	printf("Llego un mensaje Localized Pokemon con los siguientes datos: %d  %s  ", localized->pokemon.size_Nombre,
			localized->pokemon.nombre);
	int i;
	for(i=0; i<localized->listaPosiciones->elements_count; i++){
		t_posicion * posicion = list_get(localized->listaPosiciones, i);
		printf("%d  %d \n", posicion->X, posicion->Y);
	}
	info_mensaje * mensajeLocalized = malloc(sizeof(info_mensaje));
	mensajeLocalized->op_code = LOCALIZED_POKEMON;
	mensajeLocalized->id_mensaje = get_id();
	mensajeLocalized->id_mensaje_correlativo = 0;
	mensajeLocalized->process_id = 1;
	mensajeLocalized->mensaje = localized;
	mensajeLocalized->sizeMsg = getSizeMensajeLocalized(*localized);
	list_add(list_mensajes, mensajeLocalized);
	return mensajeLocalized;
}

info_mensaje * recibirMensajeCatch(int32_t socket_cliente){
	t_Catch* catch = NULL;
	catch = deserializar_paquete_catch(&socket_cliente);
	printf("Llego un mensaje Catch Pokemon con los siguientes datos: %d  %s  %d  %d \n", catch->pokemon.size_Nombre, catch->pokemon.nombre,
			catch->posicion.X, catch->posicion.Y);
	info_mensaje * mensajeCatch = malloc(sizeof(info_mensaje));
	mensajeCatch->op_code = CATCH_POKEMON;
	mensajeCatch->id_mensaje = get_id();
	mensajeCatch->id_mensaje_correlativo = 0;
	mensajeCatch->process_id = 1;
	mensajeCatch->mensaje = catch;
	mensajeCatch->sizeMsg = getSizeMensajeCatch(*catch);
	list_add(list_mensajes, mensajeCatch);
	return mensajeCatch;
}

info_mensaje * recibirMensajeCaught(int32_t socket_cliente){
	t_Caught* caught = NULL;
	caught = deserializar_paquete_caught(&socket_cliente);
	printf("Llego un mensaje Caught Pokemon con los siguientes datos:  %d\n", caught->fueAtrapado);
	info_mensaje * mensajeCaught = malloc(sizeof(info_mensaje));
	mensajeCaught->op_code = CAUGHT_POKEMON;
	mensajeCaught->id_mensaje = get_id();
	mensajeCaught->id_mensaje_correlativo = 0;
	mensajeCaught->process_id = 1;
	mensajeCaught->mensaje = caught;
	mensajeCaught->sizeMsg = getSizeMensajeCaught(*caught);
	list_add(list_mensajes, mensajeCaught);
	return mensajeCaught;
}

void enviarMensaje(op_code operacion, info_mensaje * mensaje, int32_t socket_cliente){
	int32_t id_mensaje;
	t_New * new;

	if(esCorrelativo(mensaje->id_mensaje_correlativo)){
		id_mensaje = mensaje->id_mensaje_correlativo;
	}else id_mensaje = mensaje->id_mensaje;

	switch(operacion){
	case SUSCRIPCION_NEW:
		new = mensaje->mensaje;
		//enviar_new_pokemon(char* pokemon, char* x, char* y, char* cantidad, char* id_mensaje, int32_t socket_cliente)
		//char* string_itoa(int number)
		enviar_new_pokemon(new->pokemon.nombre,string_itoa(new->posicion.X),string_itoa(new->posicion.Y),string_itoa(new->cant),string_itoa(id_mensaje), socket_cliente);
		break;
	case SUSCRIPCION_APPEARED:
		//enviar mensaje appeared
		break;
	case SUSCRIPCION_GET:
		//enviar mensaje get
		break;
	case SUSCRIPCION_LOCALIZED:
		//enviar mensaje localized
		break;
	case SUSCRIPCION_CATCH:
		//enviar mensaje catch
		break;
	case SUSCRIPCION_CAUGHT:
		//enviar mensaje caught
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

void enviarMensajeNew(t_New * new, int32_t id_mensaje, int32_t socket_cliente){
	t_paquete * paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = NEW_POKEMON;
	paquete->buffer = malloc(sizeof(t_buffer));

	paquete->buffer->size = getSizeMensajeNew(*new);
	paquete->buffer->id_Mensaje = id_mensaje;
	paquete->buffer->stream = new;

	int32_t bytes_a_enviar;
	void *paqueteSerializado = serializar_paquete_new (paquete, &bytes_a_enviar, new);

	send(socket_cliente, paqueteSerializado, bytes_a_enviar, 0);

	free(paqueteSerializado);
	free(paquete->buffer);
	free(paquete);

}

void iniciarBroker(){
	logger = iniciar_logger();
	config = leer_config();
	dump = iniciar_dump();

	log_info(logger, "Iniciando Broker \n");
	unsigned int miPid= process_getpid();
	log_info(logger, "El PID de Broker es %d \n", miPid);

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
	//algMemoria = atoi(config_get_string_value(config, "ALGORITMO_MEMORIA"));
	//algReemplazo = atoi(config_get_string_value(config, "ALGORITMO_REEMPLAZO"));
	//algParticionLibre = atoi(config_get_string_value(config, "ALGORITMO_PARTICION_LIBRE"));

	IP_BROKER = config_get_string_value(config, "IP_BROKER");
	PUERTO_BROKER = config_get_string_value(config, "PUERTO_BROKER");
	frecuenciaCompactacion = atoi(config_get_string_value(config, "FRECUENCIA_COMPACTACION"));
	LOG_FILE = config_get_string_value(config, "LOG_FILE");

	//Inicializar memoria
	inicioMemoria = (int32_t)malloc(sizeMemoria); //f00X12345  f00X12345 + 2048
	t_particion* particionInicial = crearParticion(inicioMemoria, sizeMemoria, false);
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
	config = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Broker/config/Broker.config");
	if(config== NULL){
		printf("No pude leer la config\n");
		exit(2);
	}
	return config;
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
	return sizeof(typeof(pokemon.size_Nombre)) + sizeof(typeof(char)) * pokemon.size_Nombre;
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
	sizeMsg += getSizePokemon(msgLocalized.pokemon) +
			sizeof(typeof(msgLocalized.listaPosiciones->elements_count)) +
			sizeof(typeof(t_posicion)) * msgLocalized.listaPosiciones->elements_count;

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
	if(n == SIGUSR1){
		pthread_mutex_lock(&mutex_guardar_en_memoria);
		hacerDump();
		pthread_mutex_unlock(&mutex_guardar_en_memoria);
	}else printf("Signal no reconocida. \n");
}

void hacerDump(){
	t_particion *particion;
    time_t tiempo = time(0);
    struct tm *tlocal = localtime(&tiempo);
    char fecha[128];
    strftime(fecha,128,"%d/%m/%y %H:%M:%S",tlocal);

	log_info(dump, "Dump: %s \n", fecha);
	int i;
	for(i=0; i<tabla_particiones->elements_count; i++){
		particion = list_get(tabla_particiones, i);
		log_info(dump, "Particion %d: %p - %p.	[%d]	Size: %db	LRU: %f		COLA: %d		ID_MENSAJE: %f \n",
				i, particion->posicion_inicial, particion->posicion_final, particion->ocupada, particion->size, particion->id,
				particion->codigo_operacion, particion->id_mensaje);
	}
}

t_list * getMensajesAEnviar(op_code operacion, int32_t id_proceso){
	info_mensaje * mensaje;
	t_particion * mensajeCacheado;
	t_suscriptor * suscriptor;
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
	t_list* mensajesCacheados = getMensajesCacheadosDeOperacion(tipoMensajeABuscar); // mensajesCacheadoes es una lista de t_particion

	for(int i=0; i<mensajesCacheados->elements_count; i++){
		mensajeCacheado = list_get(mensajesCacheados, i);
		mensaje = obtenerMensaje(mensajeCacheado->id_mensaje); // obtenemos el info_mensaje de list_mensajes

		for(int j=0; j<mensaje->suscriptoresQueRecibieron->elements_count; j++){
			suscriptor = list_get(mensaje->suscriptoresQueRecibieron, j);
			if(suscriptor->id == id_proceso){
				list_remove(mensajesCacheados, i);
			}
		}
	}
	//Obtenemos los info_mensaje de los mensajes a enviar
	for(int i=0; i<mensajesCacheados->elements_count; i++){
		mensaje = obtenerMensaje(mensajeCacheado->id_mensaje);
		list_add(mensajesAEnviar, mensaje);
	}
	list_destroy(mensajesCacheados);
	return mensajesAEnviar;
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

	t_list* mensajesConEseID = list_filter(list_mensajes, _esElMensaje);
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

int obtenerPosicionSuscriptor(t_suscriptor * suscriptor){
	int i;
	for(i=0; i < list_suscriptores->elements_count; i++){
		t_suscriptor * suscriptorAMirar = list_get(list_suscriptores, i);
		if(suscriptor->id == suscriptorAMirar->id){
			return i;
		}
	}
	printf("No encontre el suscriptor");
	return -1;
}

bool otraFuncionMagica(info_mensaje mensaje, int32_t id_proceso){

	return true;
}







