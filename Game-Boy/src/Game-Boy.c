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
			log_info(logger,"Error al conectar al Broker");
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
	}else{

		if(string_contains(argv[1], "TEAM")){
			socket = conexionTeam();
			if(socket == 0)
			{
				log_info(logger,"Error al conectar al Team");
				return 0;
			}
		}
	}



	liberar_conexion(socket);
	config_destroy(config);
	log_destroy(logger);
	return 0;
}


void enviar_new_pokemon(char* pokemon, char* x, char* y, char* cantidad, int socket_cliente)
{
	t_paquete * paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = 1; //NEW_POKEMON

	t_posicion * posicion = malloc(sizeof(t_posicion));
	posicion->X = (uint32_t)x;
	posicion->Y = (uint32_t)y;

	t_pokemon * p_pokemon = malloc(sizeof(t_pokemon));
	p_pokemon->size_Nombre = strlen(pokemon) +1;
	p_pokemon->nombre = pokemon;

	t_New * new = malloc(sizeof(t_New));
	new->cant = (uint32_t) cantidad;
	new->posicion = *posicion;
	new->pokemon = *p_pokemon;

	paquete->buffer->size = sizeof(paquete) + sizeof(posicion) + sizeof(p_pokemon) + sizeof(new) + 2*sizeof(double);
	memcpy(paquete->buffer->stream, new, paquete->buffer->size);

	int bytes_a_enviar = paquete ->buffer->size + sizeof(uint32_t);
	void * paqueteSerializado = serializar_paquete(paquete, &bytes_a_enviar);

	int estado = send(socket_cliente, paqueteSerializado, bytes_a_enviar, 0);

	printf("Envio pokemon \n");

	free(new);
	free(posicion);
	free(paquete);
}


