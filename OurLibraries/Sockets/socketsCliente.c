/*
 * sockets.c
 *
 *  Created on: 23 abr. 2020
 *      Author: utnso
 */

#include "socketsCliente.h"

void* serializar_paquete(t_paquete* paquete, int* bytes){
	*bytes = sizeof(paquete->codigo_operacion) + sizeof(paquete->buffer->size) + paquete->buffer->size;
	void * stream = malloc(*bytes);
	int desplazamiento = 0;

	memcpy(stream + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(stream + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(stream + desplazamiento, paquete->buffer->stream, paquete->buffer->size);

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
	{
		printf("error");
		freeaddrinfo(server_info);
		return 0;
	}

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

	int bytes_a_enviar;
	void * paqueteSerializado = serializar_paquete(paquete, &bytes_a_enviar);

	send(socket_cliente, paqueteSerializado, bytes_a_enviar, 0);

	free (paqueteSerializado);
	free (paquete->buffer->stream);
	free (paquete->buffer);
	free (paquete);

}

char* recibir_mensaje(int socket_cliente)
{
	op_code operacion;
	int buffer_size;
	void * buffer = malloc(buffer_size);

	recv(socket_cliente, &(operacion), sizeof(operacion), 0);

	/*switch (operacion) {
	case MENSAJE:
		break;
	case 0:
		exit(2);
	}*/

	recv(socket_cliente, &(buffer_size), sizeof(buffer_size), 0);
	recv(socket_cliente, buffer, buffer_size, 0);
	return buffer;
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}
