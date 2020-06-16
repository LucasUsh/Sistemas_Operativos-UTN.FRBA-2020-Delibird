#include "conexion.h"

int conexionBroker()
{
	t_config* config = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Boy/Game-Boy.config");
	char* ip_broker = config_get_string_value(config,"IP_BROKER​");
	char* puerto_broker = config_get_string_value(config,"PUERTO_BROKER");
	uint32_t socket = crear_conexion(ip_broker, puerto_broker);
	config_destroy(config);
	return socket;
}

int conexionTeam()
{
	t_config* config = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Boy/Game-Boy.config");
	char* ip_team = config_get_string_value(config,"IP_TEAM");
	char* puerto_team = config_get_string_value(config,"PUERTO_TEAM​");
	config_destroy(config);
	int socket = crear_conexion(ip_team, puerto_team);
	return socket;
}

int conexionGameCard(){
	t_log* logger = log_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Boy/Game-Boy.log", "Game-Boy", 1, LOG_LEVEL_INFO);
	t_config* config = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Boy/Game-Boy.config");
	char* ip_broker = config_get_string_value(config,"IP_GAMECARD");
	char* puerto_broker = config_get_string_value(config,"PUERTO_GAMECARD");

	log_info (logger, "Todo bien hasta crear_conexion");

	int socket = crear_conexion(ip_broker, puerto_broker);

	log_info (logger, "Sali de crear_conexion");
	log_destroy (logger);

	config_destroy(config);
	return socket;
}
