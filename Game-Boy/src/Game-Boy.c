#include "Game-Boy.h"

t_log* logger;
t_config* config;

int main(int argc, char *argv[])
{
	logger = log_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Boy/Game-Boy.log", "Game-Boy", 1, LOG_LEVEL_INFO);
	config = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Boy/Game-Boy.config");
	validar_Argc(logger,argc);
	int socket;

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
	}
	// Ejemplo./gameboy GAMECARD NEW_POKEMON Pikachu 2 5 10 9
	else if(string_contains(argv[1], "GAMECARD")) {
		log_info(logger,"If de GAMECARD");
		socket = conexionGameCard();
		if(socket == 0){
			log_info(logger,"Error al conectar con Game-Card");
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
	else {

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

void validar_Argc(t_log* logger, int arg)
{
	if(arg < 3)
	{
		log_error(logger,"Error de argumentos");
		log_destroy(logger);
		exit(1);
	}
}


void enviar_new_pokemon(char* pokemon, char* x, char* y, char* cantidad, int socket_cliente)
{
	log_info(logger,"Entro a enviar new_pokemon");
	t_paquete * paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = NEW_POKEMON; //NEW_POKEMON
	paquete->buffer = malloc(sizeof(t_buffer));

	t_posicion * posicion = malloc(sizeof(t_posicion));
	posicion->X = (int32_t) atoi (x);
	posicion->Y = (int32_t) atoi (y);

	t_pokemon * p_pokemon = malloc(sizeof(t_pokemon));
	p_pokemon->size_Nombre = strlen(pokemon) +1;
	p_pokemon->nombre = pokemon;

	t_New * new = malloc(sizeof(t_New));
	new->cant = (int32_t) atoi (cantidad);
	new->posicion = *posicion;
	new->pokemon = *p_pokemon;

	paquete->buffer->size = sizeof(new)*3;
	paquete->buffer->id_Mensaje =0;
	paquete->buffer->stream = new;
	//memcpy(paquete->buffer->stream, new, paquete->buffer->size);

	free(new);
	free(p_pokemon);
	free(posicion);

	int32_t bytes_a_enviar;
	void *paqueteSerializado = serializar_paquete(paquete, &bytes_a_enviar);

	send(socket_cliente, paqueteSerializado, bytes_a_enviar, 0);

	printf("Envio pokemon \n");

	free(paqueteSerializado);
	free(paquete);
}


