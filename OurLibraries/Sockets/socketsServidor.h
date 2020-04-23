/*
 * socketsServidor.h
 *
 *  Created on: 23 abr. 2020
 *      Author: utnso
 */

#ifndef SOCKETSSERVIDOR_H_
#define SOCKETSSERVIDOR_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include<pthread.h>

#define IP "127.0.0.1"
#define PUERTO "4444"

typedef enum
{
	MENSAJE=1
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

pthread_t thread;

void* recibir_buffer(int*, int);

void iniciar_servidor(void);
void esperar_cliente(int);
void* recibir_mensaje_servidor(int socket_cliente, int* size);
int recibir_operacion(int);
void process_request(int cod_op, int cliente_fd);
void serve_client(int *socket);
void* serializar_paquete_servidor(t_paquete* paquete, int bytes);
void devolver_mensaje(void* payload, int size, int socket_cliente);


#endif /* SOCKETSSERVIDOR_H_ */
