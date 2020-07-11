/*
 * Broker.h
 *
 *  Created on: 27 abr. 2020
 *      Author: utnso
 */

#ifndef BROKER_H_
#define BROKER_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include <sys/time.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include <commons/collections/list.h>
#include "../../OurLibraries/Sockets/sockets.h"
#include "../../OurLibraries/UniversoPokemon/universo.h"

typedef struct {
	op_code op_code;
	int32_t id_mensaje;
	int32_t process_id; // ID del proceso que mando el mensaje
	void * mensaje;
	int32_t sizeMsg;
	t_list * suscriptoresALosQueSeEnvio;
	t_list * suscriptoresQueRecibieron; // estos serian los que devolvieron el ACK
} info_mensaje;


typedef struct {
	bool ACK;
	double id;
} t_suscriptor;

t_log* logger;
t_config* config;

t_log* iniciar_logger(void);
t_config* leer_config(void);

void suscribirProceso(op_code operacion, int32_t* PID);
void agregarMensaje(t_paquete* paquete);
double get_id();

int getMemoriaOcupada();
int getMemoriaDisponible();

void iniciarColas();
void manejoMensajeSuscripcion(int32_t socket_cliente);
void recibirMensajeNew(int32_t socket_cliente);
void manejoMensajeNew(int32_t * socket_cliente);
void manejoMensajeAppeared(int32_t socket_cliente);
void manejoMensajeGet(int32_t socket_cliente);
void manejoMensajeLocalized(int32_t socket_cliente);
void manejoMensajeCatch(int32_t socket_cliente);
void manejoMensajeCaught(int32_t socket_cliente);
void informarId(int32_t socket_cliente);
void inicializarListas();


t_list* suscriptores_New;
t_list* suscriptores_Appeared;
t_list* suscriptores_Catch;
t_list* suscriptores_Caught;
t_list* suscriptores_Get;
t_list* suscriptores_Localized;
t_list* mensajes_New;
t_list* mensajes_Appeared;
t_list* mensajes_Catch;
t_list* mensajes_Caught;
t_list* mensajes_Get;
t_list* mensajes_Localized;

#endif /* BROKER_H_ */
