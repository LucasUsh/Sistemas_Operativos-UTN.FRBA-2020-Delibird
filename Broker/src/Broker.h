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
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include "/home/utnso/workspace/tp-2020-1c-5rona/OurLibraries/UniversoPokemon/universo.c"
#include <commons/collections/queue.h>
#include "../../OurLibraries/Sockets/sockets.c"

typedef struct {
	uint32_t id_Mensaje;
	void * mensaje;
	uint32_t PID; // ID del proceso que mando el mensaje
	t_list * suscriptoresALosQueSeEnvio;
	t_list * suscriptoresQueRecibieron; // estos serian los que devolvieron el ACK
} info_Mensaje;

t_log* logger;
t_config* config;

t_log* iniciar_logger(void);
t_config* leer_config(void);

void crearColasDeSuscriptores();
void suscribirProcesoACola(op_code operacion, uint32_t * PID);
void crearColasDeMensajes();
void agregarMensaje(op_code operacion, info_Mensaje * infoMensaje);
uint32_t asignarID();

t_queue * queue_Suscriptores_New;
t_queue * queue_Mensajes_New;
t_queue * queue_Suscriptores_Appeared;
t_queue * queue_Mensajes_Appeared;
t_queue * queue_Suscriptores_Catch;
t_queue * queue_Mensajes_Catch;
t_queue * queue_Suscriptores_Caught;
t_queue * queue_Mensajes_Caught;
t_queue * queue_Suscriptores_Get;
t_queue * queue_Mensajes_Get;
t_queue * queue_Suscriptores_Localized;
t_queue * queue_Mensajes_Localized;

#endif /* BROKER_H_ */
