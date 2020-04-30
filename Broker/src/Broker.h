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
#include "/home/utnso/workspace/tp-2020-1c-5rona/OurLibraries/Sockets/socketsUnificado.c"
#include <commons/collections/queue.h>


t_log* logger;
t_config* config;

t_log* iniciar_logger(void);
t_config* leer_config(void);

#endif /* BROKER_H_ */
