/*
 * socketsUnificado.c
 *
 *  Created on: 29 abr. 2020
 *      Author: utnso
 */

#include "sockets.h"


int crear_conexion(char *ip, char* puerto){
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1){
		freeaddrinfo(server_info);
		return 0;
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}

void enviar_mensaje(char* mensaje, int socket_cliente){
	t_paquete * paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = 99999999;
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

char* recibir_mensaje(int socket_cliente){
	op_code operacion;
	int buffer_size = 0;

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

void liberar_conexion(int socket_cliente){
	close(socket_cliente);
}

void* serializar_paquete(t_paquete* paquete, int *bytes){

	*bytes = sizeof(paquete->codigo_operacion) + sizeof(paquete->buffer->size) + paquete->buffer->size;
	void *stream = malloc(*bytes);
	int desplazamiento = 0;

	memcpy(stream + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(stream + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(stream + desplazamiento, paquete->buffer->stream, paquete->buffer->size);

	return stream;
}

void iniciar_servidor(char *ip_servidor, char* puerto_servidor){
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip_servidor, puerto_servidor, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }

	listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

    while(1)
    	esperar_cliente(socket_servidor);
}

void esperar_cliente(int socket_servidor){
	struct sockaddr_in dir_cliente;

	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (struct sockaddr*) &dir_cliente, &tam_direccion);
	//     ^ accept crea un nuevo socket para el cliente

	pthread_create(&thread_socket_global, NULL, (void*)serve_client, &socket_cliente);
	pthread_detach(thread_socket_global); //lo desasocio aunque sigue su curso

}

void serve_client(int* socket){
	int codigo_operacion;

	if(recv(*socket, &codigo_operacion, sizeof(int), MSG_WAITALL) == -1)
		codigo_operacion = -1;

	process_request(codigo_operacion, *socket);
}

void process_request(int codigo_operacion, int socket_cliente) {
	int size;
	void* msg;
	switch (codigo_operacion) {

		case 99999999:
			msg = recibir_mensaje_servidor(socket_cliente, &size);
			printf("Recibi el siguiente mensaje: %s", (char*) msg);
			devolver_mensaje(msg, size, socket_cliente);
			free(msg);
			break;

		case -1:
			printf ("Error al recibir paquete en serve_client. Hilo finalizado.");
			pthread_exit(NULL);
	}
}

void* recibir_mensaje_servidor(int socket_cliente, int* size){
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void devolver_mensaje(void* payload, int size, int socket_cliente){
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = 99999999;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = size;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, payload, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, &bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}
