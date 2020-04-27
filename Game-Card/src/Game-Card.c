#include "Game-Card.h"

int main(void)
{
    char* ip_broker;
	char* puerto_broker;
    t_config* config;
    t_log* logger;

    logger = log_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Card/Game-Card.log", "Game-Card", 1, LOG_LEVEL_INFO);
    config = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Card/Game-Card.config");

    ip_broker = config_get_string_value(config,"IP_BROKER");
    log_info(logger, ip_broker);
    puerto_broker = config_get_string_value(config,"PUERTO_BROKER");
    log_info(logger, puerto_broker);

    printf("GAME CARD iniciando ... \n");
    printf("Conectando ... ");
    int socket = crear_conexion(ip_broker,puerto_broker);
    printf("Conectado al socket %d \n ",socket);


    liberar_conexion(socket);
    config_destroy(config);
    log_destroy(logger);
    return 0;
}
