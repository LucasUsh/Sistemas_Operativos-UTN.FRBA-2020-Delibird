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
	int size;
	void* stream; //en el stream debe ir la estructura administrativa: id_Mensaje + Mensaje
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char* mensaje, int socket_cliente);
char* recibir_mensaje(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void liberar_conexion(int socket_cliente);

void* serializar_paquete(t_paquete* paquete, int *bytes);

int crear_socket_escucha(char *ip_servidor, char* puerto_servidor);
int recibir_cliente(int);
void servir_cliente(int *socket);
void devolver_mensaje(void* payload, int size, int socket_cliente);

void* recibir_mensaje_servidor(int socket_cliente, int* size);

#endif /* SOCKETS_H_ */
