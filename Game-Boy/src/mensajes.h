#ifndef SRC_MENSAJES_H_
#define SRC_MENSAJES_H_


#include<commons/config.h>
#include<stdint.h>
#include<commons/log.h>
#include "/home/utnso/workspace/tp-2020-1c-5rona/OurLibraries/UniversoPokemon/universo.h"
#include "../../OurLibraries/Sockets/sockets.h"


void enviar_new_pokemon(char* pokemon, char* x, char* y, char* cantidad, char* id_mensaje, int32_t socket_cliente);
void enviar_appeared_pokemon(char* pokemon, char* x, char* y, char* id_mensaje, int32_t socket_cliente);
void enviar_catch_pokemon(char* pokemon, char* x, char* y, char* id, int32_t socket_cliente);
void enviar_caught_pokemon(char* pokemon, char* id_mensaje, char* ok, int32_t socket_cliente);
void enviar_get_pokemon(char* pokemon, int32_t socket_cliente);


#endif
