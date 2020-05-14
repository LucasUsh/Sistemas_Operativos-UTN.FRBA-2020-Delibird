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

#define IP_GAME_CARD "127.0.0.3"
#define PUERTO_GAME_CARD "5001"


t_log* logger_game_card;
t_config* config_game_card;

crear_servidor_game_card(pthread_t*);
void conexionBroker(int *socket);

#endif
