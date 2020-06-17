#ifndef GAME_CARD_H_
#define GAME_CARD_H_

#include<commons/config.h>
#include<commons/log.h>
#include<commons/string.h>
#include<stdio.h>
#include<stdlib.h>
#include<readline/readline.h>
#include<netdb.h>
#include<pthread.h>
#include "../../OurLibraries/Sockets/sockets.c"
#include "../../OurLibraries/Sockets/sockets.h"

#define IP_GAME_CARD "127.0.0.3"
#define PUERTO_GAME_CARD "5001"

t_log* logger_GC = NULL;
t_log* debug = NULL;

t_config* config_GC;

pthread_t hilo_global_cliente_GC;

void crear_servidor_GC();
void responder_mensaje(int* socket_cliente);
void conexionBroker(int32_t *socket);

#endif
