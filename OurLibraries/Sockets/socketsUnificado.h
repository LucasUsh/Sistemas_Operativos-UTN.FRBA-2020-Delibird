/*
 * socketsUnificado.h
 *
 *  Created on: 29 abr. 2020
 *      Author: utnso
 */

#ifndef SOCKETSUNIFICADO_H_
#define SOCKETSUNIFICADO_H_

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

#define IP_SERVIDOR "127.0.0.1"
#define PUERTO_SERVIDOR "4444"

typedef enum
{
	NEW_POKEMON=1,
	APPEARED_POKEMON=2,
	CATCH_POKEMON=3,
	CAUGHT_POKEMON=4,
	GET_POKEMON=5,
	LOCALIZED_POKEMON=6,
	MENSAJE = 99
}op_code;

typedef struct
{
	int id_Mensaje;
	int id_Correlacional;
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

typedef struct {
	int size_Nombre;
	char * nombre;
} t_pokemon;

typedef struct {
	// codigo_operacion= 1;
	t_pokemon pokemon;
	int posX;
	int posY;
	int cant;
} t_New;

typedef struct {
	// codigo_operacion= 2;
	t_pokemon pokemon;
	int cantUbicaciones;
	t_list * listaPosiciones;
} t_Localized;

typedef struct {
	// codigo_operacion= 3;
	t_pokemon pokemon;
} t_Get;

typedef struct {
	// codigo_operacion= 4;
	t_pokemon pokemon;
	int posX;
	int posY;
} t_Appeared;

typedef struct {
	// codigo_operacion= 5;
	t_pokemon pokemon;
	int posX;
	int posy;
} t_Catch;

typedef struct {
	// codigo_operacion= 6;
	int fueAtrapado; // 1 cuando fue atrapado, 0 cuando no fue atrapado
} t_Caught;

pthread_t thread;

int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char* mensaje, int socket_cliente);
char* recibir_mensaje(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void liberar_conexion(int socket_cliente);

void* serializar_paquete(t_paquete* paquete, int* bytes);

void* recibir_buffer(int*, int);
void iniciar_servidor(void);
void esperar_cliente(int);
void* recibir_mensaje_servidor(int socket_cliente, int* size);
int recibir_operacion(int);
void process_request(int cod_op, int cliente_fd);
void serve_client(int *socket);
void devolver_mensaje(void* payload, int size, int socket_cliente);


#endif /* SOCKETSUNIFICADO_H_ */
