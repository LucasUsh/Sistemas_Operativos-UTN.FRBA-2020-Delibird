#include "Game-Card.h"

int main(void)
{
	pthread_t hilo_servidor_GC;
	pthread_create (&hilo_servidor_GC, NULL, (void *) &crear_servidor_GC, NULL);

/*
	pthread_t h1;
	int socket;
	logger_GC = log_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Card/Game-Card.log", "Game-Card", 1, LOG_LEVEL_INFO);

    pthread_create(&h1, NULL, (void*) &conexionBroker, &socket);


    pthread_join(h1,NULL);
    liberar_conexion(socket);
    config_destroy(config_GC);
    log_destroy(logger_GC);
 */
    return 0;
}

void crear_servidor_GC() {
	int socket_servidor_GC = crear_socket_escucha(IP_GAME_CARD, PUERTO_GAME_CARD);
	int socket_cliente_entrante;

    while(1) {
    	// quizás para Broker no conviene sobrepisar tod0 el tiempo el valor de este socket:
    	socket_cliente_entrante = recibir_cliente(socket_servidor_GC);

    	pthread_create(&hilo_global_cliente_GC, NULL, (void*) responder_mensaje, &socket_cliente_entrante);
    	pthread_detach(hilo_global_cliente_GC); //lo desasocio aunque sigue su curso
    }
}

void responder_mensaje(int* socket_cliente) {

	int codigo_operacion;

	if(recv(*socket_cliente, &codigo_operacion, sizeof(int), MSG_WAITALL) == -1)
			codigo_operacion = -1;

	//int size;
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

void conexionBroker(int *socket)
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
