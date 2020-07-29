#include "Game-Boy.h"

t_log* logger;
t_config* config;

int32_t main(int32_t argc, char *argv[])
{
	logger = log_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Boy/Game-Boy.log", "Game-Boy", 1, LOG_LEVEL_INFO);
	config = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Boy/Game-Boy.config");

	log_info(logger,"Game Boy iniciando...");

	validar_Argc(logger,argc);
	int32_t socket;
	int32_t operacion=0;
	int32_t tamanio_estructura = 0;
	int32_t id_mensaje=0;

	sem_t* envio_GC;
	sem_t* envio_Broker;
	sem_t* envio_Team;

	char ruta_envio_GC[] = "/dev/shm/sem.envio_GC";
	char ruta_envio_Broker[] = "/dev/shm/sem.envio_Broker";
	char ruta_envio_Team[] = "/dev/shm/sem.envio_Team";

	if (!existe(ruta_envio_GC)) envio_GC = sem_open("/envio_GC", O_CREAT | O_EXCL, 0644, 1);
		else envio_GC = sem_open ("/envio_GC", 0);

	if (!existe(ruta_envio_Broker)) envio_Broker = sem_open("/envio_Broker", O_CREAT | O_EXCL, 0644, 1);
		else envio_Broker = sem_open ("/envio_Broker", 0);

	if (!existe(ruta_envio_Team)) envio_Team = sem_open("/envio_Team", O_CREAT | O_EXCL, 0644, 1);
		else envio_Team = sem_open ("/envio_Team", 0);


	// BROKER:

	if(string_contains(argv[1], "BROKER")){

		sem_wait(envio_Broker);
		socket = conexionBroker();
		sem_post(envio_Broker);

		if(socket == 0){
			log_info(logger,"Error al conectar al Broker");
			finalizar(logger, config, socket);
			return 0;
		}
		log_info(logger,"Conectado al Broker");

		sem_wait(envio_Broker);
		enviar_handshake(1, socket);
		sem_post(envio_Broker);

		if(recv(socket, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
			if(operacion == ACK){ // Confirmacion de que la identificacion (handshake) fue recibida
				recv(socket, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
				recv(socket, &id_mensaje, sizeof(int32_t), MSG_WAITALL); //recibo el paquete, aunque a Game Boy no le interesa ningun dato

				if(string_contains(argv[2], "NEW_POKEMON")){
					sem_wait(envio_Broker);
					log_info(logger,"Envio NEW POKEMON");
					enviar_new_pokemon(argv[3], argv[4], argv[5], argv[6], "0", socket);
					sem_post(envio_Broker);
					if(recv(socket, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
						if(operacion == ACK){
							recv(socket, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
							recv(socket, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
						}
					}
				}
				if(string_contains(argv[2], "APPEARED_POKEMON")){
					sem_wait(envio_Broker);
					log_info(logger,"Envio APPEARED POKEMON");
					enviar_appeared_pokemon(argv[3], argv[4], argv[5], "0", socket);
					sem_post(envio_Broker);
					if(recv(socket, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
						if(operacion == ACK){
							recv(socket, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
							recv(socket, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
						}
					}
				}
				if(string_contains(argv[2], "CATCH_POKEMON")){
					sem_wait(envio_Broker);
					log_info(logger,"Envio CATCH POKEMON");
					enviar_catch_pokemon(argv[3], argv[4], argv[5], "0", socket);
					sem_post(envio_Broker);
					if(recv(socket, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
						if(operacion == ACK){
							recv(socket, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
							recv(socket, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
						}
					}
				}
				if(string_contains(argv[2], "CAUGHT_POKEMON")){
					sem_wait(envio_Broker);
					log_info(logger,"Envio CAUGHT POKEMON");
					enviar_caught_pokemon(argv[3], argv[4], socket);
					sem_post(envio_Broker);
					if(recv(socket, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
						if(operacion == ACK){
							recv(socket, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
							recv(socket, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
						}
					}
				}
				if(string_contains(argv[2], "GET_POKEMON")){
					sem_wait(envio_Broker);
					log_info(logger,"Envio GET POKEMON");
					enviar_get_pokemon(argv[3], "0", socket);
					sem_post(envio_Broker);
					if(recv(socket, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
						if(operacion == ACK){
							recv(socket, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
							recv(socket, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
						}
					}
				}
			}
		}
		else printf("Fallo al recibir codigo de operacion = -1\n");
		liberar_conexion(socket);
	}

	// GAME-CARD:

	else if(string_contains(argv[1], "GAMECARD")) {

		sem_wait(envio_GC);
		socket = conexionGameCard();
		sem_post(envio_GC);

		if(socket == 0){
			log_info(logger,"Error al conectar con Game-Card");
			finalizar(logger, config, socket);
			return 0;
		}
		log_info(logger,"Conectado al Game Card");

		if(string_contains(argv[2], "NEW_POKEMON")){
			sem_wait(envio_GC);
			log_info(logger,"Envio new_pokemon");
			enviar_new_pokemon(argv[3], argv[4], argv[5], argv[6], argv[7], socket);
			sem_post(envio_GC);
			if(recv(socket, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
				if(operacion == ACK){
					recv(socket, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
					recv(socket, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
				}
			}
			log_info(logger,"OK");
		}

		if(string_contains(argv[2], "CATCH_POKEMON")){
			sem_wait(envio_GC);
			log_info(logger,"Envio Catch Pokemon");
			enviar_catch_pokemon(argv[3], argv[4], argv[5], argv[6], socket);
			sem_post(envio_GC);
			if(recv(socket, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
				if(operacion == ACK){
					recv(socket, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
					recv(socket, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
				}
			}
			log_info(logger,"OK");
		}

		if(string_contains(argv[2], "GET_POKEMON")){
			sem_wait(envio_GC);
			log_info(logger,"Envio Get Pokemon");
			enviar_get_pokemon(argv[3], argv[4], socket);
			sem_post(envio_GC);
			if(recv(socket, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
				if(operacion == ACK){
					recv(socket, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
					recv(socket, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
				}
			}
			log_info(logger,"OK");
		}
	}

	// TEAM:

	else if(string_contains(argv[1], "TEAM")){

		sem_wait(envio_Team);
		socket = conexionTeam();
		sem_post(envio_Team);

		if(socket == 0)
		{
			log_info(logger,"Error al conectar al Team");
			finalizar(logger, config, socket);
			return 0;
		}
		log_info(logger,"Conectado al Team");
		if(string_contains(argv[2], "APPEARED_POKEMON")){
			sem_wait(envio_Team);
			enviar_appeared_pokemon(argv[3], argv[4], argv[5], "0", socket);
			sem_post(envio_Team);
		}
	}

	else if(string_contains(argv[1], "SUSCRIPTOR")){

		sem_wait(envio_Broker);
		socket = conexionBroker();
		sem_post(envio_Broker);

		if(socket == 0){
			log_info(logger,"Error al conectar al Broker");
			finalizar(logger, config, socket);
			return 0;
		}

		enviar_handshake(1, socket);
		if(recv(socket, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
			if(operacion == ACK){
				recv(socket, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
				recv(socket, &id_mensaje, sizeof(int32_t), MSG_WAITALL);

				if(string_contains(argv[2], "NEW_POKEMON")){
					t_New* new = NULL;
					log_info(logger,"Suscribo a NEW POKEMON");

					sem_wait(envio_Broker);
					enviar_suscripcion(SUSCRIPCION_NEW, socket);
					sem_post(envio_Broker);

					if(recv(socket, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
						if(operacion == ACK){
							recv(socket, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
							recv(socket, &id_mensaje, sizeof(int32_t), MSG_WAITALL);

							while(1){
								//por un tiempo determinado
								if(recv(socket, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
									recv(socket, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
									recv(socket, &id_mensaje, sizeof(int32_t), MSG_WAITALL);

									new = deserializar_paquete_new (&socket);
									sem_wait(envio_Broker);
									enviar_ACK(0, socket);
									sem_post(envio_Broker);

									log_info (logger, "Paquete deserializado con los siguientes datos:");
									log_info (logger, "Pokemon: %s, tamanio cadena: %d", new->pokemon.nombre, new->pokemon.size_Nombre);
									log_info (logger, "Posicion: (%d, %d)", new->posicion.X, new->posicion.Y);
									log_info (logger, "Cantidad: %d", new->cant);
								}
							}
						}
					}
				}
			}
		}
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

int32_t existe (char* ruta) {
	struct stat estado_archivo;
	return (stat (ruta, &estado_archivo) == 0);
}



