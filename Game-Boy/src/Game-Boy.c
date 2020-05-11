#include "Game-Boy.h"

t_log* logger;
t_config* config;

int main(int argc, char *argv[])
{
	int socket;

	logger = log_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Boy/Game-Boy.log", "Game-Boy", 1, LOG_LEVEL_INFO);
	config = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Boy/Game-Boy.config");

	printf("GAME BOY iniciando ... \n");

	if(string_contains(argv[1], "BROKER")){
		socket = conexionBroker();
		if(socket == 0){
			return 0;
		}
		if(string_contains(argv[2], "NEW_POKEMON")){
			enviar_new_pokemon(argv[3], argv[4], argv[5], argv[6], socket);
		}
		if(string_contains(argv[2], "APPEARED_POKEMON")){
			printf("Appeared Pokemon \n");
		}
		if(string_contains(argv[2], "CATCH_POKEMON")){
			printf("Catch Pokemon \n");
		}
		if(string_contains(argv[2], "CAUGHT_POKEMON")){
			printf("Caught Pokemon \n");
		}
	}

	if(strcmp(argv[1], "TEAM")== 0){
		socket = conexionTeam();
	}




	liberar_conexion(socket);
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

void enviar_new_pokemon(char* pokemon, char* x, char* y, char* cantidad, int socket_cliente)
{
	t_paquete * paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = NEW_POKEMON;

	t_posicion * posicion;
	posicion->X = (int)x;
	posicion->Y = (int)y;
	t_pokemon * p_pokemon;
	p_pokemon->nombre = pokemon;

	t_New * new;
	new->cant = (int) cantidad;
	new->posicion = *posicion;
	new->pokemon = *p_pokemon;

	paquete ->buffer->size = sizeof(new);
	memcpy(paquete->buffer->stream, new, paquete->buffer->size);

	int bytes_a_enviar;
	void * paqueteSerializado = serializar_paquete(paquete, &bytes_a_enviar);

	send(socket_cliente, paqueteSerializado, bytes_a_enviar, 0);

	printf("Envio pokemon \n");
	free(paquete);
}


