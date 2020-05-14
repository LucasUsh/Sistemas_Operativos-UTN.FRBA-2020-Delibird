#include "Game-Card.h"

int main(void)
{
	pthread_t hilo_servidor_game_card;
	crear_servidor_game_card(&hilo_servidor_game_card);

	pthread_t h1;
	int socket;
	logger_game_card = log_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Card/Game-Card.log", "Game-Card", 1, LOG_LEVEL_INFO);

    pthread_create(&h1, NULL, conexionBroker, &socket);


    pthread_join(h1,NULL);
    liberar_conexion(socket);
    config_destroy(config_game_card);
    log_destroy(logger_game_card);
    return 0;
}

void crear_servidor_game_card(pthread_t* hilo_servidor_game_card) {

}

void conexionBroker(int *socket)
{
	char* ip_broker;
	char* puerto_broker;
	char* timer;
	config_game_card = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Card/Game-Card.config");
	ip_broker = config_get_string_value(config_game_card,"IP_BROKER");
	puerto_broker = config_get_string_value(config_game_card,"PUERTO_BROKER");
	timer = config_get_string_value(config_game_card,"TIEMPO_DE_REINTENTO_CONEXION");

	*socket = crear_conexion(ip_broker,puerto_broker);
	while(*socket == 0)
	{
		sleep(1);
		*socket = crear_conexion(ip_broker,puerto_broker);
	}
	printf("\n");
	log_info(logger_game_card,"Conectado al Broker");
}
