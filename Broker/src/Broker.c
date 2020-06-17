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


bool flag_Suscriptores_New, flag_Suscriptores_Appeared, flag_Suscriptores_Catch, flag_Suscriptores_Caught, flag_Suscriptores_Get, flag_Suscriptores_Localized= false;
bool flag_Mensajes_New, flag_Mensajes_Appeared, flag_Mensajes_Catch, flag_Mensajes_Caught, flag_Mensajes_Get, flag_Mensajes_Localized= false;

double get_id(){
	//para obtener id usamos el timestamp

	struct timeval tv;
	gettimeofday(&tv, NULL);
	double id =((double)tv.tv_sec) * 1000 + (double)(tv.tv_usec) / 1000;
	//printf("el id es: %d\n", id);

	return id;
}

int32_t main(void) {
	malloc(sizeof(bool)*12); // uno por cada cola de mensajes y suscriptores

	logger = iniciar_logger();
	config = leer_config();

	get_id();

	char *IP_BROKER = config_get_string_value(config, "IP_BROKER");
	char *PUERTO_BROKER = config_get_string_value(config, "PUERTO_BROKER");
	log_info(logger,"Lei IP_BROKER %s ",IP_BROKER);
	log_info(logger,"Lei PUERTO_BROKER %s ",PUERTO_BROKER);

	/*
	* Creamos un hilo que debe estar constantemente escuchando con IP_BROKER y PUERTO_BROKER.
	* El resto de los procesos se conectan a ese hilo, de a uno.
	*
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

	while (1){
	//iniciar_servidor(IP_BROKER, PUERTO_BROKER);
	printf("Estoy escuchando suscripciones\n");
	int32_t socketSuscripciones = crear_socket_escucha(IP_BROKER, PUERTO_BROKER);

	while(socketSuscripciones != -1){
		struct sockaddr_in dir_cliente;
		int32_t tam_direccion = sizeof(struct sockaddr_in);
		int32_t socketCliente = accept(socketSuscripciones, (void*) &dir_cliente, &tam_direccion);
		printf("Se conecto un cliente\n");

		int32_t operacion;
		if(recv(socketCliente, &operacion, sizeof(int32_t), MSG_WAITALL) == -1){
			operacion = -1;
		}
		switch (operacion) {
		case NEW_POKEMON:
			printf("Recibi un new_pokemon");
			break;
		case APPEARED_POKEMON:
			printf("Recibi un appeared_pokemon");
			break;
		case CATCH_POKEMON:
			printf("Recibi un catch_pokemon");
			break;
		case CAUGHT_POKEMON:
			printf("Recibi un caught_pokemon");
			break;
		case GET_POKEMON:
			printf("Recibi un get_pokemon");
			break;
		case LOCALIZED_POKEMON:
			printf("Recibi un localized_pokemon");
			break;
		case 0:
			exit(2);
		case -1:
			exit(2);
		}
		/*recv(socketCliente, &(buffer_size), sizeof(buffer_size), 0);
		recv(socketCliente, buffer, buffer_size, 0);*/
	}

	}
	return EXIT_SUCCESS;

}

/*void recibir_Suscripciones(char* IP_BROKER, char* PUERTO_BROKER) {
	int32_t socket_servidor_GC = crear_socket_escucha(IP_BROKER, PUERTO_BROKER);
	int32_t socket_cliente_entrante;

	while(1) {
    	socket_cliente_entrante = recibir_cliente(int32_t socket_servidor);



    	pthread_create(&hilo_global_cliente_GC, NULL, (void*) responder_mensaje, &socket_cliente_entrante);
    	pthread_detach(hilo_global_cliente_GC);
    }
}

void responder_mensaje(int32_t* socket_cliente) {

	int32_t codigo_operacion;

	if(recv(*socket_cliente, &codigo_operacion, sizeof(int32_t), MSG_WAITALL) == -1)
			codigo_operacion = -1;

	//int32_t size;
	//void* msg;
	switch (codigo_operacion) {

		case 0:
			//msg = recibir_mensaje_servidor(socket_cliente, &size);
			//printf("Recibi el siguiente mensaje: %s", (char*) msg);
			//devolver_mensaje(msg, size, socket_cliente);
			//free(msg);
			break;

		case -1:
			printf ("Error al recibir paquete en serve_client. Hilo finalizado.");
			pthread_exit(NULL);
	}
}
*/

void suscribirProceso(op_code operacion, int32_t * PID){
	switch(operacion) {
	case SUSCRIPCION_NEW: // NEW_POKEMON = 1
		// Si no existe la cola, la creamos
		if (flag_Suscriptores_New == false){
			suscriptores_New = list_create();
			flag_Suscriptores_New= true;
		}

		list_add(suscriptores_New, PID);

		break;

	case SUSCRIPCION_APPEARED: // 	APPEARED_POKEMON=2
		// Si no existe la cola, la creamos
		if (flag_Suscriptores_Appeared == false){
			suscriptores_Appeared = list_create();
			flag_Suscriptores_Appeared= true;
		}

		list_add(suscriptores_Appeared, PID);

		break;

	case SUSCRIPCION_CATCH: // 	CATCH_POKEMON=3
		// Si no existe la cola, la creamos
		if (flag_Suscriptores_Catch == false){
			suscriptores_Catch = list_create();
			flag_Suscriptores_Catch=true;
		}

		list_add(suscriptores_Catch, PID);

		break;

	case SUSCRIPCION_CAUGHT: // CAUGHT_POKEMON=4
		// Si no existe la cola, la creamos
		if (flag_Suscriptores_Caught == false){
			suscriptores_Caught = list_create();
			flag_Suscriptores_Caught= true;
		}

		list_add(suscriptores_Caught, PID);

		break;

	case SUSCRIPCION_GET: // GET_POKEMON=5
		// Si no existe la cola, la creamos
		if (flag_Suscriptores_Get == false){
			suscriptores_Get = list_create();
			flag_Suscriptores_Get= true;
		}

		list_add(suscriptores_Get, PID);

		break;

	case SUSCRIPCION_LOCALIZED: // LOCALIZED_POKEMON=6
		// Si no existe la cola, la creamos
		if (flag_Suscriptores_Localized == false){
			suscriptores_Localized = list_create();
			flag_Suscriptores_Localized=true;
		}

		list_add(suscriptores_Localized, PID);

		break;
	default:
		return;
	}
}

void agregarMensaje(op_code operacion, info_Mensaje * infoMensaje){
	//void queue_push(t_queue *, void *element);

	switch(operacion) {
	case NEW_POKEMON: // NEW_POKEMON = 1
		// Si no existe la cola, la creamos
		if(flag_Mensajes_New== false){
			mensajes_New = list_create();
			flag_Mensajes_New = true;
		}
		list_add(mensajes_New, infoMensaje);
		break;

	case APPEARED_POKEMON: // 	APPEARED_POKEMON=2
		// Si no existe la cola, la creamos
		if(flag_Mensajes_Appeared==false){
			mensajes_Appeared = list_create();
			flag_Mensajes_Appeared = true;
		}
		list_add(mensajes_Appeared, infoMensaje);
		break;

	case CATCH_POKEMON: // 	CATCH_POKEMON=3
		// Si no existe la cola, la creamos
		if(flag_Mensajes_Catch==false){
			mensajes_Catch = list_create();
			flag_Mensajes_Catch = true;
		}
		list_add(mensajes_Catch, infoMensaje);
		break;

	case CAUGHT_POKEMON: // CAUGHT_POKEMON=4
		// Si no existe la cola, la creamos
		if(flag_Mensajes_Caught==false){
			mensajes_Caught = list_create();
			flag_Mensajes_Caught = true;
		}
		list_add(mensajes_Caught, infoMensaje);
		break;

	case GET_POKEMON: // GET_POKEMON=5
		// Si no existe la cola, la creamos
		if(flag_Mensajes_Get==false){
			mensajes_Get = list_create();
			flag_Mensajes_Get = true;
		}
		list_add(mensajes_Get, infoMensaje);
		break;

	case LOCALIZED_POKEMON: // LOCALIZED_POKEMON=6
		// Si no existe la cola, la creamos
		if(flag_Mensajes_Localized==false){
			mensajes_Localized = list_create();
			flag_Mensajes_Localized = true;
		}
		list_add(mensajes_Localized, infoMensaje);
		break;

	default:
		return;

	}

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
