#ifndef SRC_GAME_BOY_H_
#define SRC_GAME_BOY_H_

#include<stdio.h>
#include<commons/config.h>
#include<commons/string.h>
#include<commons/log.h>
#include<readline/readline.h>
#include "../../OurLibraries/Sockets/sockets.c"
#include "/home/utnso/workspace/tp-2020-1c-5rona/OurLibraries/UniversoPokemon/universo.c"

int conexionBroker();
int conexionTeam();
void enviar_new_pokemon(char* pokemon, char* x, char* y, char* cantidad, int socket_cliente);

#endif
