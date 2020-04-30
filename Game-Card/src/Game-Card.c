#include "Game-Card.h"

t_log* logger;
t_config* config;

int main(void)
{
	pthread_t h1;
	int socket;
    logger = log_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Card/Game-Card.log", "Game-Card", 1, LOG_LEVEL_INFO);

    printf("GAME CARD iniciando ... \n");
    int r = pthread_create(&h1, NULL, conexionBroker, NULL);

    pthread_join(h1,NULL);
    liberar_conexion(socket);
    config_destroy(config);
    log_destroy(logger);
    return 0;
}

int conexionBroker(void *arg)
{
	char* ip_broker;
	char* puerto_broker;
	config = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Card/Game-Card.config");
	ip_broker = config_get_string_value(config,"IP_BROKER");
	puerto_broker = config_get_string_value(config,"PUERTO_BROKER");
	int socket = crear_conexion(ip_broker,puerto_broker);
	while(socket == 0)
	{
		socket = crear_conexion(ip_broker,puerto_broker);
		sleep(1);
	}
	printf("\n");
	log_info(logger,"Conectado al Broker");

	return socket;
}
