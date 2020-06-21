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
#include <commons/collections/list.h>
#include "/home/utnso/workspace/tp-2020-1c-5rona/OurLibraries/Sockets/sockets.c"
#include<pthread.h>
#include <sys/time.h>

typedef struct {
	int32_t id_Mensaje;
	void * mensaje;
	int32_t PID; // ID del proceso que mando el mensaje
	t_list * suscriptoresALosQueSeEnvio;
	t_list * suscriptoresQueRecibieron; // estos serian los que devolvieron el ACK
} info_Mensaje;

typedef struct {
	bool ACK;
	double id;
} t_suscriptor;

typedef struct {
	int posicion_inicial;
	int posicion_final;
	int size;
	bool ocupada;
	int numero;
} t_particion;


t_log* logger;
t_config* config;

t_log* iniciar_logger(void);
t_config* leer_config(void);

void suscribirProceso(op_code operacion, int32_t* PID);
void agregarMensaje(op_code operacion, info_Mensaje * infoMensaje);
double get_id();
int getMemoriaOcupada();
int getMemoriaDisponible();

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
t_list* tabla_particiones;

#endif /* BROKER_H_ */
