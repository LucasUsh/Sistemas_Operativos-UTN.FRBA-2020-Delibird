/*
 * sockets.c
 *
 *  Created on: 23 abr. 2020
 *      Author: utnso
 */

#include "socketsCliente.h"

void* serializar_paquete(t_paquete* paquete, int* bytes)
{
	void * stream = malloc(bytes);
	int desplazamiento = 0;

	memcpy(stream + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(stream + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(stream + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	//desplazamiento+= paquete->buffer->size;

	return stream;
}


int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		printf("error");

	freeaddrinfo(server_info);

	return socket_cliente;
}

void enviar_mensaje(char* mensaje, int socket_cliente)
{
	t_paquete * paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));

	paquete->buffer->size = strlen(mensaje)+1;

	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size); //paquete->buffer->stream = mensaje;

	int* bytes = paquete->buffer->size + sizeof(int)+ sizeof(int); // sizeof(int) es por el codigo de operacion

	void * paqueteSerializado = serializar_paquete(paquete, &bytes);

	send(socket_cliente, paqueteSerializado, bytes, 0);

	free (paqueteSerializado);
	free (paquete->buffer->stream);
	free (paquete->buffer);
	free (paquete);

}

char* recibir_mensaje(int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(paquete));

	recv(socket_cliente, &(paquete->codigo_operacion), sizeof(int), 0);

	paquete->buffer = malloc(sizeof(t_buffer));
	recv(socket_cliente, &(paquete->buffer->size), sizeof(int), 0 );

	paquete->buffer->stream = malloc(paquete->buffer->size);

	recv(socket_cliente, &(paquete->buffer->stream), paquete->buffer->size, 0 );

	void * stream =  malloc(paquete->buffer->size);
	memcpy (stream, &(paquete->buffer->stream), paquete->buffer->size);

	return stream;

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}
