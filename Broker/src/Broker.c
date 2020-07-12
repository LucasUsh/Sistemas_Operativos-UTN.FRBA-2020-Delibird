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
#include "pruebas.h"


int32_t sizeMemoria, sizeMinParticion;
int32_t algMemoria;
int32_t frecuenciaCompactacion;
int32_t algReemplazo;
int32_t algParticionLibre;
int32_t id_mensaje_global = 0;
char *IP_BROKER;
char *PUERTO_BROKER;
char * LOG_FILE;

int32_t main(void) {

	logger = iniciar_logger();
	config = leer_config();


	sizeMemoria = atoi(config_get_string_value(config, "TAMANO_MEMORIA"));
	sizeMinParticion = atoi(config_get_string_value(config, "TAMANO_MINIMO_PARTICION"));
	algMemoria= atoi(config_get_string_value(config, "ALGORITMO_MEMORIA"));
	algReemplazo = atoi(config_get_string_value(config, "ALGORITMO_REEMPLAZO"));
	algParticionLibre = atoi(config_get_string_value(config, "ALGORITMO_PARTICION_LIBRE"));
	IP_BROKER = config_get_string_value(config, "IP_BROKER");
	PUERTO_BROKER = config_get_string_value(config, "PUERTO_BROKER");
	frecuenciaCompactacion = atoi(config_get_string_value(config, "FRECUENCIA_COMPACTACION"));
	LOG_FILE = config_get_string_value(config, "LOG_FILE");

	int inicioMemoria = (int)malloc(sizeMemoria); //f00X12345  f00X12345 + 2048
	t_particion* particionInicial = crearParticion(inicioMemoria, sizeMemoria, false, 0);
	tabla_particiones = list_create();
	list_add(tabla_particiones, particionInicial);

	//pruebaEncontrarBuddyTrasDosParticiones();


	log_info(logger,"Lei IP_BROKER %s ",IP_BROKER);
	log_info(logger,"Lei PUERTO_BROKER %s ",PUERTO_BROKER);

	inicializarListas();

	printf("Iniciando Broker \n");
	int32_t socketEscucha = crear_socket_escucha(IP_BROKER, PUERTO_BROKER);
	printf("Creado el socket de escucha\n");

	while(socketEscucha != -1){
		int32_t socket_cliente = (int32_t)recibir_cliente(socketEscucha);

		if(socket_cliente != -1){
			printf("Se conecto un cliente\n");

			int32_t tamanio_estructura = 0;
			int32_t id_mensaje=0;
			int32_t operacion=0;
			pthread_t hilo;
			if(recv(socket_cliente, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
				switch(operacion){

				case SUSCRIPCION_NEW || SUSCRIPCION_APPEARED || SUSCRIPCION_CATCH || SUSCRIPCION_CAUGHT || SUSCRIPCION_GET || SUSCRIPCION_LOCALIZED:
				/*if (pthread_create(&hilo, NULL, (void*)manejoMensajeSuscripcion, socket_cliente) == 0){
					printf("Creado el hilo que maneja la suscripcion");
				}else printf("Fallo al crear el hilo que maneja la suscripcion");*/
				//manejoMensajeSuscripcion(socket_cliente);
				break;
				case NEW_POKEMON:
					recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
					recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
					log_info(logger, "Nuevo mensaje en cola New \n");
					/*if (pthread_create(&hilo, NULL, (void*)manejoMensajeNew, &socket_cliente) == 0){
						printf("Creado el hilo que maneja el mensaje New");
					}else printf("Fallo al crear el hilo que maneja el mensaje New");*/
					recibirMensajeNew(socket_cliente);
					break;
				case APPEARED_POKEMON:
					recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
					recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
					log_info(logger, "Nuevo mensaje en cola Appeared \n");
					/*if (pthread_create(&hilo, NULL, (void*)manejoMensajeAppeared, (void*)socket_cliente) == 0){
						printf("Creado el hilo que maneja el mensaje Appeared");
					}else printf("Fallo al crear el hilo que maneja el mensaje Appeared");*/
					manejoMensajeAppeared(socket_cliente);
					printf("El id mensaje era: %d \n", id_mensaje);
					break;
				case GET_POKEMON:
					recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
					recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
					log_info(logger, "Nuevo mensaje en cola Get \n");
					/*if (pthread_create(&hilo, NULL, (void*)manejoMensajeGet, (void*)socket_cliente) == 0){
						printf("Creado el hilo que maneja el mensaje Get");
					}else printf("Fallo al crear el hilo que maneja el mensaje Get");*/
					manejoMensajeGet(socket_cliente);
					break;
				case LOCALIZED_POKEMON:
					recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
					recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
					log_info(logger, "Nuevo mensaje en cola Localized \n");
					/*if (pthread_create(&hilo, NULL, (void*)manejoMensajeLocalized, socket_cliente) == 0){
						printf("Creado el hilo que maneja el mensaje Localized");
					}else printf("Fallo al crear el hilo que maneja el mensaje Localized");*/
					manejoMensajeLocalized(socket_cliente);
					break;
				case CATCH_POKEMON:
					recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
					recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
					log_info(logger, "Nuevo mensaje en cola Catch \n");
					/*if (pthread_create(&hilo, NULL, (void*)manejoMensajeCatch, socket_cliente) == 0){
						printf("Creado el hilo que maneja el mensaje Catch");
					}else printf("Fallo al crear el hilo que maneja el mensaje Catch");*/
					manejoMensajeCatch(socket_cliente);
					break;
				case CAUGHT_POKEMON:
					recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
					recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
					log_info(logger, "Nuevo mensaje en cola Caught \n");
					/*if (pthread_create(&hilo, NULL, (void*)manejoMensajeCaught, socket_cliente) == 0){
						printf("Creado el hilo que maneja el mensaje Caught");
					}else printf("Fallo al crear el hilo que maneja el mensaje Caught");*/
					manejoMensajeCaught(socket_cliente);
					printf("El id mensaje era: %d \n", id_mensaje);
					break;
				}
			}else printf("Fallo al recibir codigo de operacion = -1\n");
			liberar_conexion(socket_cliente);
		}else printf("Fallo al recibir/aceptar al cliente\n");
	}
	if(socketEscucha == -1){
		printf("Fallo al crear socket de escucha = -1\n");
		//return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;

}

double get_id(){
	//para obtener id usamos el timestamp

	struct timeval tv;
	gettimeofday(&tv, NULL);
	double id =((double)tv.tv_sec) * 1000 + (double)(tv.tv_usec);
	//printf("el id es: %f\n", id);

	return id;
}

/*void iniciarColas(){

	pthread_t hilo_new;
	if (pthread_create(&hilo_new, NULL, (void*)escucharColaNew, NULL) == 0){
		printf("Creado el hilo que maneja la cola New\n");
	}else printf("Fallo al crear el hilo de cola New\n");

	escucharColaAppeared();

	pthread_t hilo_appeared;
	if (pthread_create(&hilo_appeared, NULL, (void*)escucharColaAppeared, NULL) == 0){
		printf("Creado el hilo que maneja la cola Appeared\n");
	}else printf("Fallo al crear el hilo de cola Appeared\n");

	pthread_t hilo_get;
	if (pthread_create(&hilo_get, NULL, (void*)escucharColaGet, NULL) == 0){
		printf("Creado el hilo que maneja la cola Get\n");
	}else printf("Fallo al crear el hilo de cola Get\n");

	pthread_t hilo_localized;
	if (pthread_create(&hilo_localized, NULL, (void*)escucharColaLocalized, NULL) == 0){
		printf("Creado el hilo que maneja la cola Localized\n");
	}else printf("Fallo al crear el hilo de cola Localized\n");

	pthread_t hilo_catch;
	if (pthread_create(&hilo_catch, NULL, (void*)escucharColaCatch, NULL) == 0){
		printf("Creado el hilo que maneja la cola Catch\n");
	}else printf("Fallo al crear el hilo de cola Catch\n");

	pthread_t hilo_caught;
	if (pthread_create(&hilo_caught, NULL, (void*)escucharColaCaught, NULL) == 0){
		printf("Creado el hilo que maneja la cola Caught\n");
	}else printf("Fallo al crear el hilo de cola Caught\n");

}*/

void manejoMensajeSuscripcion(int32_t socket_cliente){
	//pedir identificacion del proceso (handshake)
	informarId(socket_cliente); // falta incorporar semaforo
	//agregar en lista de colas a la que se suscribio: suscribirProceso(op_code operacion, int32_t * PID)
}

void recibirMensajeNew(int32_t socket_cliente){
	t_New* new = NULL;
	new = deserializar_paquete_new (&socket_cliente);
	printf("Llego un mensaje New Pokemon con los siguientes datos: %d  %s  %d  %d  %d \n", new->pokemon.size_Nombre, new->pokemon.nombre,
			new->cant, new->posicion.X, new->posicion.Y);
	//pedir identificacion del proceso (handshake)
	//informar id mensaje
	//asignar id mensaje al mensaje recibido
}

void manejoMensaje(info_mensaje* mensaje){
	//agregar mensaje en lista de mensaje
	administrarMensaje(algMemoria, mensaje, frecuenciaCompactacion, algReemplazo, algParticionLibre);
	//opcional: informar a todos los suscriptores (definir si esto se hace aca y se crea un hilo para esperar el ACK
	// o se hace en otro hilo)
}


void manejoMensajeAppeared(int32_t socket_cliente){
	t_Appeared* app = NULL;
	app = deserializar_paquete_appeared(&socket_cliente);
	printf("Llego un mensaje Appeared Pokemon con los siguientes datos: %d  %s  %d  %d\n", app->pokemon.size_Nombre, app->pokemon.nombre,
			app->posicion.X, app->posicion.Y);
	//pedir identificacion del proceso (handshake)
	//informar id mensaje
	//asignar id mensaje al mensaje recibido
	//agregar mensaje en lista Appeared
	//opcional: informar a todos los suscriptores (definir si esto se hace aca y se crea un hilo para esperar el ACK
	// o se hace en otro hilo)
}

void manejoMensajeGet(int32_t socket_cliente){
	t_Get* get = NULL;
	get = deserializar_paquete_get(&socket_cliente);
	printf("Llego un mensaje Get Pokemon con los siguientes datos: %d  %s\n", get->pokemon.size_Nombre, get->pokemon.nombre);
	//pedir identificacion del proceso (handshake)
	//informar id mensaje
	//asignar id mensaje al mensaje recibido
	//agregar mensaje en lista Get

	//opcional: informar a todos los suscriptores (definir si esto se hace aca y se crea un hilo para esperar el ACK
	// o se hace en otro hilo)
}

void manejoMensajeLocalized(int32_t socket_cliente){
	t_Localized* localized = NULL;
	localized = deserializar_paquete_localized(&socket_cliente);
	printf("Llego un mensaje Localized Pokemon con los siguientes datos: %d  %s  ", localized->pokemon.size_Nombre,
			localized->pokemon.nombre);
	int i;
	for(i=0; i<localized->listaPosiciones->elements_count; i++){
		t_posicion * posicion = list_get(localized->listaPosiciones, i);
		printf("%d  %d \n", posicion->X, posicion->Y);
	}
	//pedir identificacion del proceso (handshake)
	//informar id mensaje
	//asignar id mensaje al mensaje recibido
	//agregar mensaje en lista Localized

	//opcional: informar a todos los suscriptores (definir si esto se hace aca y se crea un hilo para esperar el ACK
	// o se hace en otro hilo)
}

void manejoMensajeCatch(int32_t socket_cliente){
	t_Catch* catch = NULL;
	catch = deserializar_paquete_catch(&socket_cliente);
	printf("Llego un mensaje Catch Pokemon con los siguientes datos: %d  %s  %d  %d \n", catch->pokemon.size_Nombre, catch->pokemon.nombre,
			catch->posicion.X, catch->posicion.Y);
	//pedir identificacion del proceso (handshake)
	//informar id mensaje
	//asignar id mensaje al mensaje recibido
	//agregar mensaje en lista Catch

	//opcional: informar a todos los suscriptores (definir si esto se hace aca y se crea un hilo para esperar el ACK
	// o se hace en otro hilo)
}

void manejoMensajeCaught(int32_t socket_cliente){
	t_Caught* caught = NULL;
	caught = deserializar_paquete_caught(&socket_cliente);
	printf("Llego un mensaje Caught Pokemon con los siguientes datos:  %d\n", caught->fueAtrapado);
	//pedir identificacion del proceso (handshake)
	//informar id mensaje
	//asignar id mensaje al mensaje recibido
	//agregar mensaje en lista Caught

	//opcional: informar a todos los suscriptores (definir si esto se hace aca y se crea un hilo para esperar el ACK
	// o se hace en otro hilo)
}

void informarId(int32_t socket_cliente){
	double id = get_id();

	t_paquete * paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = ID_MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));

	paquete->buffer->size = sizeof(double);
	paquete->buffer->id_Mensaje = id;
	paquete->buffer->stream = NULL;// o deberia ser stream = 0?

	int32_t bytes_a_enviar;
	void *paqueteSerializado = serializar_id (paquete, &bytes_a_enviar);
	send(socket_cliente, paqueteSerializado, bytes_a_enviar, 0);

	free(paqueteSerializado);
	free(paquete->buffer);
	free(paquete);
}

void inicializarListas(){
	suscriptores_New = list_create();
	suscriptores_Appeared = list_create();
	suscriptores_Catch = list_create();
	suscriptores_Caught = list_create();
	suscriptores_Get = list_create();
	suscriptores_Localized = list_create();
}

void suscribirProceso(op_code operacion, int32_t * PID){
	switch(operacion) {
	case SUSCRIPCION_NEW: // 0
		list_add(suscriptores_New, PID);
		break;

	case SUSCRIPCION_APPEARED: // 	1
		list_add(suscriptores_Appeared, PID);
		break;

	case SUSCRIPCION_CATCH: // 	2
		list_add(suscriptores_Catch, PID);
		break;

	case SUSCRIPCION_CAUGHT: // 3
		list_add(suscriptores_Caught, PID);
		break;

	case SUSCRIPCION_GET: // 4
		list_add(suscriptores_Get, PID);
		break;

	case SUSCRIPCION_LOCALIZED: // 5
		list_add(suscriptores_Localized, PID);
		break;

	default: // Esto es para que no tire el warning de que no estamos teniendo en cuenta los otros tipos de mensaje
		return;
	}
}

bool esSuscripcion(op_code cod_operacion){
	return cod_operacion == SUSCRIPCION_NEW ||
			cod_operacion == SUSCRIPCION_APPEARED ||
			cod_operacion == SUSCRIPCION_CATCH ||
			cod_operacion == SUSCRIPCION_CAUGHT ||
			cod_operacion == SUSCRIPCION_GET ||
			cod_operacion == SUSCRIPCION_LOCALIZED;
}


void agregarMensaje(t_paquete* paquete){

	info_mensaje* mensaje = malloc(sizeof(info_mensaje));

	//recibo el mensaje y lo reconozco
	mensaje->id_mensaje = get_id();
	//mensaje->mensaje = deserializarPaquete(paquete);
	mensaje->op_code = paquete->codigo_operacion;

	//algoritmo_particion(algoritmo_particion, mensaje);

	return;
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

int32_t getsizeMensajeAppeared(t_Appeared msgAppeared){
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

int32_t getsizeMensajeCatch(t_Catch msgCatch){
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

int32_t getsizeMensajeCaught(t_Caught msgCaught){
	/*
	 * 0
	 * un uint_32 para saber si se puedo o no atrapar al pokemon
	*/

	return sizeof(typeof(sizeof(msgCaught.fueAtrapado)));
}

