#include "Game-Card.h"

int32_t main(void)
{
	instalar_filesystem ();

	debug = log_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Card/debug.log", "Game-Card", 1, LOG_LEVEL_DEBUG);
	pthread_t hilo_servidor_GC;
	if (pthread_create (&hilo_servidor_GC, NULL, (void *) &crear_servidor_GC, NULL) == 0)
		log_debug (debug, "Hilo servidor creado correctamente.");

	// TODO: asociarse globalmente a las colas NEW_POKEMON, CATCH y GET
	// Una vez suscripto tendríamos entonces 3 sockets modo cliente,
	// cuando llega un mensaje informar al Broker la recepción del mismo (ACK) y hacer lo que corresponda

/*
	pthread_t h1;
	int32_t socket;
	logger_GC = log_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Card/Game-Card.log", "Game-Card", 1, LOG_LEVEL_INFO);

    pthread_create(&h1, NULL, (void*) &conexionBroker, &socket);


    pthread_join(h1,NULL);
    liberar_conexion(socket);
    config_destroy(config_GC);
    log_destroy(logger_GC);
 */
	pthread_join(hilo_servidor_GC, NULL);
    return 0;
}

void instalar_filesystem (){
	t_config* config_GC = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Card/Game-Card.config");
	char* punto_de_montaje = config_get_string_value(config_GC,"PUNTO_MONTAJE_TALLGRASS");
	config_destroy(config_GC);


}

void crear_servidor_GC() {

	int32_t socket_servidor_GC = crear_socket_escucha(IP_GAME_CARD, PUERTO_GAME_CARD);
	int32_t socket_cliente_entrante;

    while(1) {
    	socket_cliente_entrante = recibir_cliente(socket_servidor_GC);

    	if (pthread_create(&hilo_global_cliente_GC, NULL, (void*) responder_mensaje, &socket_cliente_entrante) == 0)
    		log_debug (debug, "Hilo para responder al cliente creado correctamente.");

    	pthread_detach(hilo_global_cliente_GC); //lo desasocio aunque sigue su curso
    }
}

void responder_mensaje(int* socket_cliente) {

	int32_t codigo_operacion;

	if(recv(*socket_cliente, &codigo_operacion, sizeof(int), MSG_WAITALL) == -1)
			codigo_operacion = -1;

	log_debug (debug, "Código de operación %d", codigo_operacion);

	switch (codigo_operacion) {

		case NEW_POKEMON:
			//msg = recibir_mensaje_servidor(socket_cliente, &size);
			//printf("Recibi el siguiente mensaje: %s", (char*) msg);
			//devolver_mensaje(msg, size, socket_cliente);
			//free(msg);
			break;

		case CATCH_POKEMON:

			break;

		case GET_POKEMON:

			break;

		default:

			break;
	}
}

void conexionBroker(int32_t *socket)
{
	char* ip_broker;
	char* puerto_broker;

	config_GC = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Card/Game-Card.config");
	ip_broker = config_get_string_value(config_GC,"IP_BROKER");
	puerto_broker = config_get_string_value(config_GC,"PUERTO_BROKER");
	//char* timer = config_get_string_value(config_GC,"TIEMPO_DE_REINTENTO_CONEXION");

	*socket = crear_conexion(ip_broker,puerto_broker);
	while(*socket == 0)
	{
		sleep(1);
		*socket = crear_conexion(ip_broker,puerto_broker);
	}
	printf("\n");
	log_info(logger_GC,"Conectado al Broker");
}
