/*
 ============================================================================
 Name        : Broker.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "Broker.h"

uint32_t id_Mensaje = 0;

uint32_t main(void) {
	malloc(sizeof(bool)*6);

	logger = iniciar_logger();
	config = leer_config();

	char *IP_BROKER = config_get_string_value(config, "IP_BROKER");
	char *PUERTO_BROKER = config_get_string_value(config, "PUERTO_BROKER");

	log_info(logger,"Lei IP_BROKER %s ",IP_BROKER);
	log_info(logger,"Lei PUERTO_BROKER %s ",PUERTO_BROKER);

	//iniciar_servidor(IP_BROKER, PUERTO_BROKER);
	crear_socket_escucha(IP_BROKER, PUERTO_BROKER);
	return EXIT_SUCCESS;

}

void crearColasDeSuscriptores(){
	malloc(sizeof(t_queue) *6);
	t_queue * queue_Suscriptores_New = queue_create();
	t_queue * queue_Suscriptores_Appeared = queue_create();
	t_queue * queue_Suscriptores_Catch = queue_create();
	t_queue * queue_Suscriptores_Caught = queue_create();
	t_queue * queue_Suscriptores_Get = queue_create();
	t_queue * queue_Suscriptores_Localized = queue_create();
}

void suscribirProcesoACola(op_code operacion, uint32_t * PID){
	switch(operacion) {
	case 1: // NEW_POKEMON = 1
		queue_push(queue_Suscriptores_New, PID);
		break;
	case 2: // 	APPEARED_POKEMON=2
		queue_push(queue_Suscriptores_Appeared, PID);
		break;
	case 3: // 	CATCH_POKEMON=3
		queue_push(queue_Suscriptores_Catch, PID);
		break;
	case 4: // CAUGHT_POKEMON=4
		queue_push(queue_Suscriptores_Caught, PID);
		break;
	case 5: // GET_POKEMON=5
		queue_push(queue_Suscriptores_Get, PID);
		break;
	case 6: // LOCALIZED_POKEMON=6
		queue_push(queue_Suscriptores_Localized, PID);
		break;
	}
}

void crearColasDeMensajes(){
	malloc(sizeof(t_queue) *6);
	t_queue * queue_Mensajes_New = queue_create();
	t_queue * queue_Mensajes_Appeared = queue_create();
	t_queue * queue_Mensajes_Catch = queue_create();
	t_queue * queue_Mensajes_Caught = queue_create();
	t_queue * queue_Mensajes_Get = queue_create();
	t_queue * queue_Mensajes_Localized = queue_create();
}

void agregarMensaje(op_code operacion, info_Mensaje infoMensaje){
/*
 * CHEQUEAR EL SEGUNDO ARGUMENTO DEL QUEUE_PUSH EN CADA CASE;
 * SERIA LA FORMA CORRECTA DE GUARDAR EL STRUCT INFO_MENSAJE EN UNA COLA?
 */
	switch(operacion) {
	case 1: // NEW_POKEMON = 1
		queue_push(queue_Mensajes_New, &infoMensaje);
		break;
	case 2: // 	APPEARED_POKEMON=2
		queue_push(queue_Mensajes_Appeared, &infoMensaje);
		break;
	case 3: // 	CATCH_POKEMON=3
		queue_push(queue_Mensajes_Catch, &infoMensaje);
		break;
	case 4: // CAUGHT_POKEMON=4
		queue_push(queue_Mensajes_Caught, &infoMensaje);
		break;
	case 5: // GET_POKEMON=5
		queue_push(queue_Mensajes_Get, &infoMensaje);
		break;
	case 6: // LOCALIZED_POKEMON=6
		queue_push(queue_Mensajes_Localized, &infoMensaje);
		break;
	}
}

uint32_t asignarID(){
	id_Mensaje =+1;
	return id_Mensaje;
}

t_log* iniciar_logger(void){
	t_log* logger;
	logger = log_create("/home/utnso/workspace/tp-2020-1c-5rona/Broker/Broker.log", "Broker", 1, LOG_LEVEL_INFO);
	if(logger == NULL){
		printf("No pude iniciar el logger\n");
		exit(1);
	}
	log_info(logger, "Inicio log");
	return logger;
}

t_config * leer_config(void){
	t_config * config;
	config = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Broker/config/Broker.config");
	if(config== NULL){
		printf("No pude leer la config\n");
		exit(2);
	}
	return config;
}
