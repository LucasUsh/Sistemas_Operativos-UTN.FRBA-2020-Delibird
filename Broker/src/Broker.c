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
int32_t id_mensaje_global = 0;
char *IP_BROKER;
char *PUERTO_BROKER;

int32_t main(void) {

	logger = iniciar_logger();
	config = leer_config();
	tabla_particiones = list_create();

	sizeMemoria = atoi(config_get_string_value(config, "TAMANO_MEMORIA"));
	sizeMinParticion = atoi(config_get_string_value(config, "TAMANO_MINIMO_PARTICION"));

	int inicioMemoria = (int)malloc(sizeMemoria); //f00X12345  f00X12345 + 2048
	t_particion* particionInicial = crearParticion(inicioMemoria, sizeMemoria, false, 0);
	list_add(tabla_particiones, particionInicial);

	//pruebaEncontrarBuddyTrasDosParticiones();

	IP_BROKER = config_get_string_value(config, "IP_BROKER");
	PUERTO_BROKER = config_get_string_value(config, "PUERTO_BROKER");
	log_info(logger,"Lei IP_BROKER %s ",IP_BROKER);
	log_info(logger,"Lei PUERTO_BROKER %s ",PUERTO_BROKER);

	inicializarListas();
	while (1){
		//escucharColaNew();
		//escucharColaAppeared();
		//escucharColaGet();
		//escucharColaCatch();
		//escucharColaCaught(); <--- todas recibiendo bien. Falta chequear escucharColaLocalized que envia solo Game Card

		iniciarColas();
		//escucharSuscripciones(IP_BROKER, PUERTO_BROKER);
	}



	/*
	* Se les asigna un nuevo socket para que envien o reciban mensajes, se agrega el proceso a la
	* lista de suscriptores a la que solicito unirse, Se verifica si hay mensajes sin ACK en la lista
	* a la que suscribio un proceso, en caso de que si haya se envia por el nuevo socket y se
	* los saca del socket de suscripciones para poder seguir atendiendo otras suscripciones.
	*
	* Luego, debera haber un hilo por cada proceso suscripto a una cola especficia.
	* Ejemplo: Team1 se suscribe a mensajes_Appeared, a mensajes_Get y a mensajes_Localized
	* entonces tiene que tener 3 hilos que manejaran los distintos mensajes que se envien o reciban
	* por cada socket.
	* Tenemos que implementar semaforos para asignar los ID de mensajes, agregar, enviar y recibir
	* mensajes, ya que habra concurrencia entre los distintos hilos
	*
	*/



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

void iniciarColas(){

	pthread_t hilo_new;
	if (pthread_create(&hilo_new, NULL, (void*)escucharColaNew, NULL) == 0){
		printf("Creado el hilo que maneja la cola New");
		pthread_detach(hilo_new); //lo desasocio aunque sigue su curso
	}else printf("Fallo al crear el hilo de cola New");

	pthread_t hilo_appeared;
	if (pthread_create(&hilo_appeared, NULL, (void*)escucharColaAppeared, NULL) == 0){
		printf("Creado el hilo que maneja la cola Appeared");
		pthread_detach(hilo_appeared);
	}else printf("Fallo al crear el hilo de cola Appeared");

	pthread_t hilo_get;
	if (pthread_create(&hilo_get, NULL, (void*)escucharColaGet, NULL) == 0){
		printf("Creado el hilo que maneja la cola Get");
		pthread_detach(hilo_get);
	}else printf("Fallo al crear el hilo de cola Get");

	pthread_t hilo_localized;
	if (pthread_create(&hilo_localized, NULL, (void*)escucharColaLocalized, NULL) == 0){
		printf("Creado el hilo que maneja la cola Localized");
		pthread_detach(hilo_localized);
	}else printf("Fallo al crear el hilo de cola Localized");

	pthread_t hilo_catch;
	if (pthread_create(&hilo_catch, NULL, (void*)escucharColaCatch, NULL) == 0){
		printf("Creado el hilo que maneja la cola Catch");
		pthread_detach(hilo_catch);
	}else printf("Fallo al crear el hilo de cola Catch");

	pthread_t hilo_caught;
	if (pthread_create(&hilo_caught, NULL, (void*)escucharColaCaught, NULL) == 0){
		printf("Creado el hilo que maneja la cola Caught");
		pthread_detach(hilo_caught);
	}else printf("Fallo al crear el hilo de cola Caught");

}

void escucharColaNew(){
	printf("Iniciando Cola de mensajes New\n");
	int32_t socketEscucha = crear_socket_escucha(IP_BROKER, PUERTO_BROKER);

	while(socketEscucha != -1){
		printf("Cola de mensajes New esuchando\n");
		int32_t socket_cliente = (int32_t)recibir_cliente(socketEscucha);

		if(socket_cliente != -1){
			printf("Nueva conexion en cola de mensajes New\n");
			int32_t tamanio_estructura = 0;
			int32_t id_mensaje;
			int32_t operacion;
			if(recv(socket_cliente, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
				if(operacion==NEW_POKEMON){
					recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
					recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
					printf("Nuevo mensaje en cola New \n");
					log_info(logger, "Nuevo mensaje en cola New \n");

					t_New* new = NULL;
					new = deserializar_paquete_new (&socket_cliente);
					printf("Llego un mensaje New Pokemon con los siguientes datos: %d  %s  %d  %d  %d \n", new->pokemon.size_Nombre, new->pokemon.nombre,
							new->cant, new->posicion.X, new->posicion.Y);
					//pedir identificacion del proceso (handshake)
					//informar id mensaje
					//asignar id mensaje al mensaje recibido
					//agregar mensaje en lista New

					//opcional: informar a todos los suscriptores (definir si esto se hace aca y se crea un hilo para esperar el ACK
					// o se hace en otro hilo)
				}else printf("El codigo de operacion no era New \n");
			}else {
				printf("Fallo al recibir codigo de operacion en cola New = -1\n");
				}
			liberar_conexion(socket_cliente);
			}else {
				printf("Fallo al recibir/aceptar al cliente\n");
			}
	}
	if(socketEscucha == -1){
		printf("Fallo al crear socket de escucha de cola New = -1\n");
	}
}

void escucharColaAppeared(){
	printf("Iniciando Cola de mensajes Appeared\n");
	int32_t socketEscucha = crear_socket_escucha(IP_BROKER, PUERTO_BROKER);

	while(socketEscucha != -1){
		printf("Cola de mensajes Appeared esuchando\n");
		int32_t socket_cliente = (int32_t)recibir_cliente(socketEscucha);

		if(socket_cliente != -1){
			printf("Nueva conexion en cola de mensajes Appeared\n");
			int32_t tamanio_estructura = 0;
			int32_t id_mensaje;
			int32_t operacion;
			if(recv(socket_cliente, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
				if(operacion==APPEARED_POKEMON){
					recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
					recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
					printf("Nuevo mensaje en cola Appeared \n");
					log_info(logger, "Nuevo mensaje en cola Appeared \n");

					t_Appeared* app = NULL;
					app = deserializar_paquete_appeared(&socket_cliente);
					printf("Llego un mensaje Appeared Pokemon con los siguientes datos: %d  %s  %d  %d  %d\n", app->pokemon.size_Nombre, app->pokemon.nombre,
							app->posicion.X, app->posicion.Y, id_mensaje);

					//pedir identificacion del proceso (handshake)
					//informar id mensaje
					//asignar id mensaje al mensaje recibido
					//agregar mensaje en lista Appeared

					//opcional: informar a todos los suscriptores (definir si esto se hace aca y se crea un hilo para esperar el ACK
					// o se hace en otro hilo)

				}else printf("El codigo de operacion no era Appeared \n");
			}else {
				printf("Fallo al recibir codigo de operacion en cola Appeared = -1\n");
				}
			liberar_conexion(socket_cliente);
		}else {
			printf("Fallo al recibir/aceptar al cliente\n");
		}
	}
	if(socketEscucha == -1){
		printf("Fallo al crear socket de escucha de cola Appeared = -1\n");
	}
}

void escucharColaGet(){
	printf("Iniciando Cola de mensajes Get\n");
	int32_t socketEscucha = crear_socket_escucha(IP_BROKER, PUERTO_BROKER);

	while(socketEscucha != -1){
		printf("Cola de mensajes Get esuchando\n");
		int32_t socket_cliente = (int32_t)recibir_cliente(socketEscucha);

		if(socket_cliente != -1){
			printf("Nueva conexion en cola de mensajes Get\n");
			int32_t tamanio_estructura = 0;
			int32_t id_mensaje;
			int32_t operacion;
			if(recv(socket_cliente, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
				if(operacion==GET_POKEMON){
					recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
					recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
					printf("Nuevo mensaje en cola Get \n");
					log_info(logger, "Nuevo mensaje en cola Get \n");

					t_Get* get = NULL;
					get = deserializar_paquete_get(&socket_cliente);
					printf("Llego un mensaje Get Pokemon con los siguientes datos: %d  %s\n", get->pokemon.size_Nombre, get->pokemon.nombre);
					//pedir identificacion del proceso (handshake)
					//informar id mensaje
					//asignar id mensaje al mensaje recibido
					//agregar mensaje en lista Get

					//opcional: informar a todos los suscriptores (definir si esto se hace aca y se crea un hilo para esperar el ACK
					// o se hace en otro hilo)

				}else printf("El codigo de operacion no era Get \n");
			}else {
				printf("Fallo al recibir codigo de operacion en cola Get = -1\n");
				}
			liberar_conexion(socket_cliente);
		}else {
			printf("Fallo al recibir/aceptar al cliente\n");
		}
	}
	if(socketEscucha == -1){
		printf("Fallo al crear socket de escucha de cola Get = -1\n");
	}
}

void escucharColaLocalized(){
	printf("Iniciando Cola de mensajes Localized\n");
	int32_t socketEscucha = crear_socket_escucha(IP_BROKER, PUERTO_BROKER);

	while(socketEscucha != -1){
		printf("Cola de mensajes Localized esuchando\n");
		int32_t socket_cliente = (int32_t)recibir_cliente(socketEscucha);

		if(socket_cliente != -1){
			printf("Nueva conexion en cola de mensajes Localized\n");
			int32_t tamanio_estructura = 0;
			int32_t id_mensaje;
			int32_t operacion;
			if(recv(socket_cliente, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
				if(operacion==LOCALIZED_POKEMON){
					recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
					recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
					printf("Nuevo mensaje en cola Localized \n");
					log_info(logger, "Nuevo mensaje en cola Localized \n");

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

				}else printf("El codigo de operacion no era Localized \n");
			}else {
				printf("Fallo al recibir codigo de operacion en cola Localized = -1\n");
				}
			liberar_conexion(socket_cliente);
		}else {
			printf("Fallo al recibir/aceptar al cliente\n");
		}
	}
	if(socketEscucha == -1){
		printf("Fallo al crear socket de escucha de cola Localized = -1\n");
	}
}

void escucharColaCatch(){
	printf("Iniciando Cola de mensajes Catch\n");
	int32_t socketEscucha = crear_socket_escucha(IP_BROKER, PUERTO_BROKER);

	while(socketEscucha != -1){
		printf("Cola de mensajes Catch esuchando\n");
		int32_t socket_cliente = (int32_t)recibir_cliente(socketEscucha);

		if(socket_cliente != -1){
			printf("Nueva conexion en cola de mensajes Catch\n");
			int32_t tamanio_estructura = 0;
			int32_t id_mensaje;
			int32_t operacion;
			if(recv(socket_cliente, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
				if(operacion==CATCH_POKEMON){
					recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
					recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
					printf("Nuevo mensaje en cola Catch \n");
					log_info(logger, "Nuevo mensaje en cola Catch \n");

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

				}else printf("El codigo de operacion no era Catch \n");
			}else {
				printf("Fallo al recibir codigo de operacion en cola Catch = -1\n");
				}
			liberar_conexion(socket_cliente);
		}else {
			printf("Fallo al recibir/aceptar al cliente\n");
		}
	}
	if(socketEscucha == -1){
		printf("Fallo al crear socket de escucha de cola Catch = -1\n");
	}
}

void escucharColaCaught(){
	printf("Iniciando Cola de mensajes Caught\n");
	int32_t socketEscucha = crear_socket_escucha(IP_BROKER, PUERTO_BROKER);

	while(socketEscucha != -1){
		printf("Cola de mensajes Caught esuchando\n");
		int32_t socket_cliente = (int32_t)recibir_cliente(socketEscucha);

		if(socket_cliente != -1){
			printf("Nueva conexion en cola de mensajes Caught\n");
			int32_t tamanio_estructura = 0;
			int32_t id_mensaje;
			int32_t operacion;
			if(recv(socket_cliente, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
				if(operacion==CAUGHT_POKEMON){
					recv(socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
					recv(socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
					printf("Nuevo mensaje en cola Caught \n");
					log_info(logger, "Nuevo mensaje en cola Caught \n");

					t_Caught* caught = NULL;
					caught = deserializar_paquete_caught(&socket_cliente);
					printf("Llego un mensaje Caught Pokemon con los siguientes datos: %d  %d\n",id_mensaje, caught->fueAtrapado);
					//pedir identificacion del proceso (handshake)
					//informar id mensaje
					//asignar id mensaje al mensaje recibido
					//agregar mensaje en lista Caught

					//opcional: informar a todos los suscriptores (definir si esto se hace aca y se crea un hilo para esperar el ACK
					// o se hace en otro hilo)

				}else printf("El codigo de operacion no era Caught \n");
			}else {
				printf("Fallo al recibir codigo de operacion en cola Caught = -1\n");
				}
			liberar_conexion(socket_cliente);
		}else {
			printf("Fallo al recibir/aceptar al cliente\n");
		}
	}
	if(socketEscucha == -1){
		printf("Fallo al crear socket de escucha de cola Caught = -1\n");
	}
}

void escucharSuscripciones(){
	printf("Iniciando socket de suscripciones \n");
	int32_t socketEscucha = crear_socket_escucha(IP_BROKER, PUERTO_BROKER);
	printf("Escuchando suscripciones\n");

	while(socketEscucha != -1){
		int32_t socket_cliente = (int32_t)recibir_cliente(socketEscucha);

		if(socket_cliente != -1){
			printf("Se conecto un cliente\n");

			int32_t operacion;
			if(recv(socket_cliente, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
				if(operacion== SUSCRIPCION_NEW || SUSCRIPCION_APPEARED || SUSCRIPCION_CATCH || SUSCRIPCION_CAUGHT || SUSCRIPCION_GET || SUSCRIPCION_LOCALIZED){

					//pedir identificacion del proceso (handshake)
					informarId(socket_cliente); // falta incorporar semaforo
					//agregar en lista de colas a la que se suscribio: suscribirProceso(op_code operacion, int32_t * PID)
					//informar socket de la lista a la que se suscribio para que envie los mensajes alli

				}else printf("El codigo de operacion no era una suscripcion \n");
			}else printf("Fallo al recibir codigo de operacion en escucharSuscripcion = -1\n");
			liberar_conexion(socket_cliente);
		}else printf("Fallo al recibir/aceptar al cliente\n");
	}
	if(socketEscucha == -1){
		printf("Fallo al crear socket de escucha de suscripciones = -1\n");
		//return EXIT_FAILURE;
	}
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

