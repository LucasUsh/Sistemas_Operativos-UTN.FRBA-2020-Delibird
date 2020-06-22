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
#include "/home/utnso/workspace/tp-2020-1c-5rona/OurLibraries/Sockets/sockets.c"
#include "/home/utnso/workspace/tp-2020-1c-5rona/OurLibraries/Sockets/sockets.h"
#include "/home/utnso/workspace/tp-2020-1c-5rona/OurLibraries/UniversoPokemon/universo.c"
#include "/home/utnso/workspace/tp-2020-1c-5rona/OurLibraries/UniversoPokemon/universo.h"

#define IP_GAME_CARD "127.0.0.3"
#define PUERTO_GAME_CARD "5001"

t_log* logger_GC = NULL;
t_log* debug = NULL;

t_config* config_GC;

pthread_t hilo_global_cliente_GC;

void instalar_filesystem();

void crear_servidor_GC();
void responder_mensaje(int32_t* socket_cliente);
void conexionBroker(int32_t *socket);

#endif
