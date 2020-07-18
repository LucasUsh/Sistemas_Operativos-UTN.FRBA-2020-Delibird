#include "Game-Boy.h"

t_log* logger;
t_config* config;

int32_t main(int32_t argc, char *argv[])
{
	logger = log_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Boy/Game-Boy.log", "Game-Boy", 1, LOG_LEVEL_INFO);
	config = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Boy/Game-Boy.config");
	validar_Argc(logger,argc);
	int32_t socket;
	int32_t operacion=0;
	int32_t tamanio_estructura = 0;
	double id_mensaje=0;

	printf("GAME BOY iniciando ... \n");

	// BROKER:

	if(string_contains(argv[1], "BROKER")){
		socket = conexionBroker();
		if(socket == 0){
			log_info(logger,"Error al conectar al Broker");
			finalizar(logger, config, socket);
			return 0;
		}
		log_info(logger,"Conectado al Broker");

		enviar_handshake(1, socket);
		if(recv(socket, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
			if(operacion == ACK){ // Confirmacion de que la identificacion (handshake) fue recibida
				recv(socket, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
				recv(socket, &id_mensaje, sizeof(double), MSG_WAITALL); //recibo el paquete, aunque a Game Boy no le interesa ningun dato

				if(string_contains(argv[2], "NEW_POKEMON")){
					enviar_new_pokemon(argv[3], argv[4], argv[5], argv[6], "0", socket);
					if(recv(socket, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){// Esperamos confirmacion de recepcion del mensaje
						if(operacion == ACK){
							recv(socket, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
							recv(socket, &id_mensaje, sizeof(double), MSG_WAITALL); //recibo el paquete, aca llega el id_mensaje asignado por Broker
						}
					}
				}
				if(string_contains(argv[2], "APPEARED_POKEMON")){
					enviar_appeared_pokemon(argv[3], argv[4], argv[5], argv[6], socket);
				}
				if(string_contains(argv[2], "CATCH_POKEMON")){
					enviar_catch_pokemon(argv[3], argv[4], argv[5], "0", socket);
				}
				if(string_contains(argv[2], "CAUGHT_POKEMON")){
					enviar_caught_pokemon(argv[3], argv[4], socket);
				}
				if(string_contains(argv[2], "GET_POKEMON")){
					enviar_get_pokemon(argv[3], "0", socket);
				}
			}
		}
		else printf("Fallo al recibir codigo de operacion = -1\n");
		liberar_conexion(socket);
	}

	// GAME-CARD:

	else if(string_contains(argv[1], "GAMECARD")) {
		socket = conexionGameCard();
		if(socket == 0){
			log_info(logger,"Error al conectar con Game-Card");
			finalizar(logger, config, socket);
			return 0;
		}
		log_info(logger,"Conectado al Game Card");
		if(string_contains(argv[2], "NEW_POKEMON")){
			log_info(logger,"Envio new_pokemon");
			enviar_new_pokemon(argv[3], argv[4], argv[5], argv[6], argv[7], socket);
		}
		if(string_contains(argv[2], "CATCH_POKEMON")){
			log_info(logger,"Envio Catch Pokemon");
			enviar_catch_pokemon(argv[3], argv[4], argv[5], argv[6], socket);
		}
		if(string_contains(argv[2], "GET_POKEMON")){
			log_info(logger,"Envio Get Pokemon");
			enviar_get_pokemon(argv[3], argv[4], socket);
		}
	}

	// TEAM:

	else if(string_contains(argv[1], "TEAM")){

		socket = conexionTeam();

		if(socket == 0)
		{
			log_info(logger,"Error al conectar al Team");
			finalizar(logger, config, socket);
			return 0;
		}
		log_info(logger,"Conectado al Team");
		if(string_contains(argv[2], "APPEARED_POKEMON")){
			enviar_appeared_pokemon(argv[3], argv[4], argv[5], "0", socket);
		}
	}

	else if(string_contains(argv[1], "SUSCRIPTOR")){
		socket = conexionBroker();
		if(socket == 0){
			log_info(logger,"Error al conectar al Broker");
			finalizar(logger, config, socket);
			return 0;
		}
		//Obtener los mensajes de una cola
	}

	finalizar(logger, config, socket);
	return 0;
}

void validar_Argc(t_log* logger, int32_t arg)
{
	if(arg < 3)
	{
		log_error(logger,"Error de argumentos");
		log_destroy(logger);
		exit(1);
	}
}

void finalizar(t_log* logger, t_config* config, int32_t socket)
{
	liberar_conexion(socket);
	config_destroy(config);
	log_destroy(logger);
}



