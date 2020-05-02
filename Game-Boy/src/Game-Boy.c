#include "Game-Boy.h"

t_log* logger;
t_config* config;

int main(void)
{
	int socketBroker;
	int socketTeam;

	logger = log_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Boy/Game-Boy.log", "Game-Boy", 1, LOG_LEVEL_INFO);
	config = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Boy/Game-Boy.config");

	printf("GAME BOY iniciando ... \n");
	socketBroker = conexionBroker();
	socketTeam = conexionTeam();




	liberar_conexion(socketBroker);
	liberar_conexion(socketTeam);
	config_destroy(config);
	log_destroy(logger);
	return 0;
}

int conexionBroker()
{
	char* ip_broker = config_get_string_value(config,"IP_BROKER​");
	char* puerto_broker = config_get_string_value(config,"PUERTO_BROKER");

	int socket = crear_conexion(ip_broker,puerto_broker);
	if(socket != 0)
	{
		log_info(logger,"Conectado al Broker");
	}else
	{
		log_info(logger,"Error al conectar al Broker");
	}

	return socket;
}

int conexionTeam()
{
	char* ip_team = config_get_string_value(config,"IP_TEAM");
	char* puerto_team = config_get_string_value(config,"PUERTO_TEAM​");

	int socket = crear_conexion(ip_team, puerto_team);
	if(socket != 0)
	{
		log_info(logger,"Conectado al Team");
	}else
	{
		log_info(logger,"Error al conectar al Team");
	}
	return socket;
}
