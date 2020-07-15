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
	double id_mensaje;
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
int32_t sizeMemoria, sizeMinParticion;
int32_t algMemoria;
int32_t frecuenciaCompactacion;
int32_t algReemplazo;
int32_t algParticionLibre;

char *IP_BROKER;
char *PUERTO_BROKER;
char * LOG_FILE;

t_log* iniciar_logger(void);
t_config* leer_config(void);

void suscribirProceso(op_code operacion, int32_t* PID);
void agregarMensaje(t_paquete* paquete);
double get_id();

int getMemoriaOcupada();
int getMemoriaDisponible();

void iniciarBroker();

void manejoMensajeSuscripcion(int32_t socket_cliente);
void manejoMensaje(info_mensaje* mensaje);
info_mensaje * recibirMensajeNew(int32_t socket_cliente);
info_mensaje * recibirMensajeAppeared(int32_t socket_cliente);
info_mensaje * recibirMensajeGet(int32_t socket_cliente);
info_mensaje * recibirMensajeLocalized(int32_t socket_cliente);
info_mensaje * recibirMensajeCatch(int32_t socket_cliente);
info_mensaje * recibirMensajeCaught(int32_t socket_cliente);


void informarId(int32_t socket_cliente);

int32_t getSizeMensajeNew(t_New msgNew);
int32_t getSizeMensajeAppeared(t_Appeared msgAppeared);
int32_t getSizeMensajeGet(t_Get msgGet);
int32_t getSizeMensajeLocalized(t_Localized msgLocalized);
int32_t getSizeMensajeCatch(t_Catch msgCatch);
int32_t getSizeMensajeCaught(t_Caught msgCaught);


t_list* suscriptores_New;
t_list* suscriptores_Appeared;
t_list* suscriptores_Catch;
t_list* suscriptores_Caught;
t_list* suscriptores_Get;
t_list* suscriptores_Localized;

t_list* list_mensajes;


t_list* mensajes_New;
t_list* mensajes_Appeared;
t_list* mensajes_Catch;
t_list* mensajes_Caught;
t_list* mensajes_Get;
t_list* mensajes_Localized;

#endif /* BROKER_H_ */
