/*
 * socketsUnificado.c
 *
 *  Created on: 29 abr. 2020
 *      Author: utnso
 */

#include "sockets.h"

/****************************************************************************/
/**********************************CONEXION**********************************/
/****************************************************************************/

int32_t crear_conexion(char *ip, char* puerto){
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int32_t socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1){
		freeaddrinfo(server_info);
		return 0;
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}

void enviar_mensaje(char* mensaje, int32_t socket_cliente){
	t_paquete * paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = 99999999;
	paquete->buffer = malloc(sizeof(t_buffer));

	paquete->buffer->size = strlen(mensaje)+1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size); //paquete->buffer->stream = mensaje;

	int32_t bytes_a_enviar;
	void * paqueteSerializado = serializar_paquete(paquete, &bytes_a_enviar);

	send(socket_cliente, paqueteSerializado, bytes_a_enviar, 0);

	free (paqueteSerializado);
	free (paquete->buffer->stream);
	free (paquete->buffer);
	free (paquete);

}

void liberar_conexion(int32_t socket_cliente){
	close(socket_cliente);
}

/////////////Server:

int32_t crear_socket_escucha(char *ip_servidor, char* puerto_servidor){
	int32_t socket_servidor;

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

    return socket_servidor;
}

int32_t recibir_cliente(int32_t socket_servidor){
	struct sockaddr_in dir_cliente;

	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	return accept(socket_servidor, (struct sockaddr*) &dir_cliente, &tam_direccion);
	//       ^ accept crea un nuevo socket para el cliente

}



/*********************************************************************************/
/**********************************SERIALIZACION**********************************/
/*********************************************************************************/


void* serializar_paquete(t_paquete* paquete, int32_t *bytes){

	*bytes = sizeof(paquete->codigo_operacion) + sizeof(paquete->buffer->size) + paquete->buffer->size;
	void *stream = malloc(*bytes);
	int32_t desplazamiento = 0;

	memcpy(stream + desplazamiento, &(paquete->codigo_operacion), sizeof(int32_t));
	desplazamiento+= sizeof(int32_t);
	memcpy(stream + desplazamiento, &(paquete->buffer->size), sizeof(int32_t));
	desplazamiento+= sizeof(int32_t);
	memcpy(stream + desplazamiento, paquete->buffer->stream, paquete->buffer->size);

	return stream;
}

void* serializar_paquete_new (t_paquete* paquete, int32_t* bytes, t_New* new){

	*bytes = sizeof(paquete->codigo_operacion) + sizeof(paquete->buffer->size) + sizeof(paquete->buffer->id_Mensaje) + paquete->buffer->size;
	void *stream = malloc(*bytes);
	int32_t desplazamiento = 0;

	memcpy(stream, &(paquete->codigo_operacion), sizeof(paquete->codigo_operacion));
	desplazamiento+= sizeof(paquete->codigo_operacion);

	memcpy(stream + desplazamiento, &(paquete->buffer->size), sizeof(paquete->buffer->size));
	desplazamiento+= sizeof(paquete->buffer->size);
	memcpy(stream + desplazamiento, &(paquete->buffer->id_Mensaje), sizeof(paquete->buffer->id_Mensaje));
	desplazamiento+= sizeof(paquete->buffer->id_Mensaje);

	memcpy(stream + desplazamiento, &(new->pokemon.size_Nombre), sizeof(new->pokemon.size_Nombre));
	desplazamiento+= sizeof(new->pokemon.size_Nombre);
	memcpy(stream + desplazamiento, new->pokemon.nombre, new->pokemon.size_Nombre);
	desplazamiento+= new->pokemon.size_Nombre;
	memcpy(stream + desplazamiento, &(new->posicion.X), sizeof(new->posicion.X));
	desplazamiento+= sizeof(new->posicion.X);
	memcpy(stream + desplazamiento, &(new->posicion.Y), sizeof(new->posicion.Y));
	desplazamiento+= sizeof(new->posicion.Y);
	memcpy(stream + desplazamiento, &(new->cant), sizeof(new->cant));

	return stream;
}

t_New* deserializar_paquete_new (int32_t* socket_cliente) {

	t_New* new = malloc (sizeof(t_New));

	recv (*socket_cliente, &(new->pokemon.size_Nombre), sizeof(new->pokemon.size_Nombre), MSG_WAITALL);

	new->pokemon.nombre = malloc (new->pokemon.size_Nombre);

	recv(*socket_cliente, new->pokemon.nombre, new->pokemon.size_Nombre, MSG_WAITALL);

	recv(*socket_cliente, &(new->posicion.X), sizeof(new->posicion.X), MSG_WAITALL);

	recv(*socket_cliente, &(new->posicion.Y), sizeof(new->posicion.Y), MSG_WAITALL);

	recv(*socket_cliente, &(new->cant), sizeof(new->cant), MSG_WAITALL);

	return new;
}

void* serializar_paquete_appeared (t_paquete* paquete, int32_t* bytes, t_Appeared* app){

	*bytes = sizeof(paquete->codigo_operacion) + sizeof(paquete->buffer->size) + sizeof(paquete->buffer->id_Mensaje) + paquete->buffer->size;
	void *stream = malloc(*bytes);
	int32_t desplazamiento = 0;

	memcpy(stream, &(paquete->codigo_operacion), sizeof(paquete->codigo_operacion));
	desplazamiento+= sizeof(paquete->codigo_operacion);

	memcpy(stream + desplazamiento, &(paquete->buffer->size), sizeof(paquete->buffer->size));
	desplazamiento+= sizeof(paquete->buffer->size);
	memcpy(stream + desplazamiento, &(paquete->buffer->id_Mensaje), sizeof(paquete->buffer->id_Mensaje));
	desplazamiento+= sizeof(paquete->buffer->id_Mensaje);

	memcpy(stream + desplazamiento, &(app->pokemon.size_Nombre), sizeof(app->pokemon.size_Nombre));
	desplazamiento+= sizeof(app->pokemon.size_Nombre);
	memcpy(stream + desplazamiento, app->pokemon.nombre, app->pokemon.size_Nombre);
	desplazamiento+= app->pokemon.size_Nombre;
	memcpy(stream + desplazamiento, &(app->posicion.X), sizeof(app->posicion.X));
	desplazamiento+= sizeof(app->posicion.X);

	return stream;
}

t_Appeared* deserializar_paquete_appeared (int32_t* socket_cliente) {

	t_Appeared* app = malloc (sizeof(t_Appeared));

	recv (*socket_cliente, &(app->pokemon.size_Nombre), sizeof(app->pokemon.size_Nombre), MSG_WAITALL);

	app->pokemon.nombre = malloc (app->pokemon.size_Nombre);

	recv(*socket_cliente, app->pokemon.nombre, app->pokemon.size_Nombre, MSG_WAITALL);

	recv(*socket_cliente, &(app->posicion.X), sizeof(app->posicion.X), MSG_WAITALL);

	recv(*socket_cliente, &(app->posicion.Y), sizeof(app->posicion.Y), MSG_WAITALL);

	return app;
}
































