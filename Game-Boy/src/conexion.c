#include "conexion.h"

int conexionBroker()
{
	t_config* config = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Boy/Game-Boy.config");
	char* ip_broker = config_get_string_value(config,"IP_BROKER​");
	char* puerto_broker = config_get_string_value(config,"PUERTO_BROKER");

	int socket = crear_conexion(ip_broker,puerto_broker);
	return socket;
}

int conexionTeam()
{
	t_config* config = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Boy/Game-Boy.config");
	char* ip_team = config_get_string_value(config,"IP_TEAM");
	char* puerto_team = config_get_string_value(config,"PUERTO_TEAM​");

	int socket = crear_conexion(ip_team, puerto_team);
	return socket;
}
