/*
 * socketsUnificado.h
 *
 *  Created on: 29 abr. 2020
 *      Author: utnso
 */

#ifndef SOCKETS_H_
#define SOCKETS_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netdb.h>
#include<sys/socket.h>
#include<string.h>
#include<pthread.h> // Servidor
#include<commons/log.h> //Cliente
#include<commons/collections/list.h> // Cliente
#include<signal.h> // Cliente
#include<unistd.h> // Cliente
#include<stdint.h>
#include "/home/utnso/workspace/tp-2020-1c-5rona/OurLibraries/UniversoPokemon/universo.h"

//#define IP_SERVIDOR "127.0.0.1"
//#define PUERTO_SERVIDOR "4444"


typedef enum
{
	SUSCRIPCION_NEW,
	SUSCRIPCION_APPEARED,
	SUSCRIPCION_CATCH,
	SUSCRIPCION_CAUGHT,
	SUSCRIPCION_GET,
	SUSCRIPCION_LOCALIZED,
	NEW_POKEMON,
	APPEARED_POKEMON,
	CATCH_POKEMON,
	CAUGHT_POKEMON,
	GET_POKEMON,
	LOCALIZED_POKEMON,
} op_code;

typedef struct
{
	int32_t size;
	int32_t id_Mensaje;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

int32_t crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char* mensaje, int32_t socket_cliente);
char* recibir_mensaje(int32_t socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void liberar_conexion(int32_t socket_cliente);

void* serializar_paquete(t_paquete* paquete, int32_t *bytes);
void* serializar_paquete_new (t_paquete*, int32_t*, t_New*);

int32_t crear_socket_escucha(char *ip_servidor, char* puerto_servidor);
int32_t recibir_cliente(int32_t);
void servir_cliente(int32_t *socket);
void devolver_mensaje(void* payload, int32_t size, int32_t socket_cliente);

void enviar_mensaje_con_opCode(op_code codOperacion, int32_t idMensaje, void* mensaje, int32_t socket_cliente);

#endif /* SOCKETS_H_ */
