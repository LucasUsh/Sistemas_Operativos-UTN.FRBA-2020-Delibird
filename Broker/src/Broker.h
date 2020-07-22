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
#include<commons/process.h>
#include <commons/collections/list.h>
#include "../../OurLibraries/Sockets/sockets.h"
#include "../../OurLibraries/UniversoPokemon/universo.h"

typedef struct {
	op_code op_code;
	int32_t id_mensaje;
	int32_t id_mensaje_correlativo;
	int32_t process_id; // ID del proceso que mando el mensaje
	void * mensaje;
	int32_t sizeMsg;
	t_list * suscriptoresALosQueSeEnvio;
	t_list * suscriptoresQueRecibieron; // estos serian los que devolvieron el ACK
} info_mensaje;

typedef struct {
	op_code op_code;
	int32_t id;
	int32_t socket;
} t_suscriptor;

typedef enum {
	BS,
	PARTICIONES
} algoritmoMemoria;

typedef enum {
	FIFO,
	LRU
} algoritmoReemplazo;

typedef enum {
	FF,
	BF
} algoritmoParticionLibre;


t_log* logger;
t_config* config;
t_log* dump;
int32_t sizeMemoria, sizeMinParticion;
int32_t algMemoria;
int32_t frecuenciaCompactacion;
int32_t algReemplazo;
int32_t algParticionLibre;
int32_t inicioMemoria;
t_list* list_mensajes;
t_list* list_suscriptores;
char *IP_BROKER;
char *PUERTO_BROKER;
char * LOG_FILE;


int32_t get_id();
void manejoMensajeSuscripcion(int32_t socket_cliente, int32_t id_proceso, int32_t operacion);
void manejoMensaje(info_mensaje* mensaje);
info_mensaje * recibirMensajeNew(int32_t socket_cliente);
info_mensaje * recibirMensajeAppeared(int32_t socket_cliente);
info_mensaje * recibirMensajeGet(int32_t socket_cliente);
info_mensaje * recibirMensajeLocalized(int32_t socket_cliente);
info_mensaje * recibirMensajeCatch(int32_t socket_cliente);
info_mensaje * recibirMensajeCaught(int32_t socket_cliente);
void enviarMensaje(op_code operacion, info_mensaje * mensaje, int32_t socket_cliente);
bool esCorrelativo();
void enviarMensajeNew(t_New * new, int32_t id_mensaje, int32_t socket_cliente);

void iniciarBroker();
t_log* iniciar_logger(void);
t_config* leer_config(void);
t_log* iniciar_dump(void);

int getMemoriaOcupada();
int getMemoriaDisponible();
int32_t getSizeMensajeNew(t_New msgNew);
int32_t getSizeMensajeAppeared(t_Appeared msgAppeared);
int32_t getSizeMensajeGet(t_Get msgGet);
int32_t getSizeMensajeLocalized(t_Localized msgLocalized);
int32_t getSizeMensajeCatch(t_Catch msgCatch);
int32_t getSizeMensajeCaught(t_Caught msgCaught);

void rutina (int n);
void hacerDump();
t_list * getMensajesAEnviar(op_code operacion, int32_t id_proceso);
t_list* getMensajesCacheadosDeOperacion(op_code operacion);

info_mensaje * obtenerMensaje(int32_t id_mensaje);
bool procesoSuscriptoACola(op_code operacion, int32_t id_proceso);
t_suscriptor * obtenerSuscriptor(int32_t id_proceso);

#endif /* BROKER_H_ */
