#ifndef GAME_CARD_H_
#define GAME_CARD_H_

#include<commons/config.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/bitarray.h>
#include<stdio.h>
#include<stdlib.h>
#include<readline/readline.h>
#include<netdb.h>
#include<pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdbool.h>
#include "/home/utnso/workspace/tp-2020-1c-5rona/OurLibraries/Sockets/sockets.c"
#include "/home/utnso/workspace/tp-2020-1c-5rona/OurLibraries/Sockets/sockets.h"
#include "/home/utnso/workspace/tp-2020-1c-5rona/OurLibraries/UniversoPokemon/universo.c"
#include "/home/utnso/workspace/tp-2020-1c-5rona/OurLibraries/UniversoPokemon/universo.h"
#include "/home/utnso/workspace/tp-2020-1c-5rona/Game-Card/src/funciones.h"

//Punteros globales:
t_log* logger_GC = NULL;
t_log* debug = NULL;

t_config* config_GC = NULL;

char* punto_de_montaje = NULL;
char* tam_bloque = NULL;
char* cant_bloques = NULL;
char* magic_number = NULL;

//Datos globales:
pthread_t hilo_global_cliente_GC;

t_bitarray mapa_de_bloques;

//Prototipos de funciones:
void instalar_filesystem();
void crear_servidor_GC();
void responder_mensaje(int32_t* socket_cliente);
void conexionBroker(int32_t *socket);
void salir (const char* mensaje);

#endif
