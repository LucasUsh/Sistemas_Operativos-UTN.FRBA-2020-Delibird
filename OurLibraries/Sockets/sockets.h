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

//#define IP_SERVIDOR "127.0.0.1"
//#define PUERTO_SERVIDOR "4444"


typedef enum
{
	SUSCRIPCION_LOCALIZED,
	SUSCRIPCION_CAUGHT,
	SUSCRIPCION_APPEARED,
	NEW_POKEMON,
	APPEARED_POKEMON,
	CATCH_POKEMON,
	CAUGHT_POKEMON,
	GET_POKEMON,
	LOCALIZED_POKEMON,
} op_code;

typedef struct
{
	uint32_t size;
	uint32_t id_Mensaje;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

uint32_t crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char* mensaje, uint32_t socket_cliente);
char* recibir_mensaje(uint32_t socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void liberar_conexion(uint32_t socket_cliente);

void* serializar_paquete(t_paquete* paquete, uint32_t *bytes);

uint32_t crear_socket_escucha(char *ip_servidor, char* puerto_servidor);
uint32_t recibir_cliente(uint32_t);
void servir_cliente(uint32_t *socket);
void devolver_mensaje(void* payload, uint32_t size, uint32_t socket_cliente);

void* recibir_mensaje_servidor(uint32_t socket_cliente, uint32_t* size);
void enviar_mensaje_con_opCode(op_code codOperacion, uint32_t idMensaje, void* mensaje, uint32_t socket_cliente);

#endif /* SOCKETS_H_ */
