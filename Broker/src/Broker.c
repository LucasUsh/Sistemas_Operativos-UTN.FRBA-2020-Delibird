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
bool flag_Suscriptores_New, flag_Suscriptores_Appeared, flag_Suscriptores_Catch, flag_Suscriptores_Caught, flag_Suscriptores_Get, flag_Suscriptores_Localized= false;
bool flag_Mensajes_New, flag_Mensajes_Appeared, flag_Mensajes_Catch, flag_Mensajes_Caught, flag_Mensajes_Get, flag_Mensajes_Localized= false;

uint32_t main(void) {
	malloc(sizeof(bool)*12); // uno por cada cola de mensajes y suscriptores

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

void suscribirProceso(op_code operacion, uint32_t * PID){
	switch(operacion) {
	case 1: // NEW_POKEMON = 1
		// Si no existe la cola, la creamos
		if (flag_Suscriptores_New == false){
			t_list * suscriptores_New = list_create();
			flag_Suscriptores_New= true;
		}else {}
		list_add(suscriptores_New, PID);
		break;

	case 2: // 	APPEARED_POKEMON=2
		// Si no existe la cola, la creamos
		if (flag_Suscriptores_Appeared == false){
			t_list * suscriptores_Appeared = list_create();
			flag_Suscriptores_Appeared= true;
		}else {}
		list_add(suscriptores_Appeared, PID);
		break;

	case 3: // 	CATCH_POKEMON=3
		// Si no existe la cola, la creamos
		if (flag_Suscriptores_Catch == false){
			t_list * suscriptores_Catch = list_create();
			flag_Suscriptores_Catch=true;
		}else {}
		list_add(suscriptores_Catch, PID);
		break;

	case 4: // CAUGHT_POKEMON=4
		// Si no existe la cola, la creamos
		if (flag_Suscriptores_Caught == false){
			t_list * suscriptores_Caught = list_create();
			flag_Suscriptores_Caught= true;
		}else {}
		list_add(suscriptores_Caught, PID);
		break;

	case 5: // GET_POKEMON=5
		// Si no existe la cola, la creamos
		if (flag_Suscriptores_Get == false){
			t_list * suscriptores_Get = list_create();
			flag_Suscriptores_Get= true;
		}else {}
		list_add(suscriptores_Get, PID);
		break;

	case 6: // LOCALIZED_POKEMON=6
		// Si no existe la cola, la creamos
		if (flag_Suscriptores_Localized == false){
			t_list * suscriptores_Localized = list_create();
			flag_Suscriptores_Localized=true;
		}else {}
		list_add(suscriptores_Localized, PID);
		break;
	}
}

void agregarMensaje(op_code operacion, info_Mensaje * infoMensaje){
	//void queue_push(t_queue *, void *element);

	switch(operacion) {
	case 1: // NEW_POKEMON = 1
		// Si no existe la cola, la creamos
		if(flag_Mensajes_New== false){
			t_list * mensajes_New = list_create();
			flag_Mensajes_New = true;
		} else{}
		list_add(mensajes_New, infoMensaje);
		break;

	case 2: // 	APPEARED_POKEMON=2
		// Si no existe la cola, la creamos
		if(flag_Mensajes_Appeared==false){
			t_list * mensajes_Appeared = list_create();
			flag_Mensajes_Appeared = true;
		} else{}
		list_add(mensajes_Appeared, infoMensaje);
		break;

	case 3: // 	CATCH_POKEMON=3
		// Si no existe la cola, la creamos
		if(flag_Mensajes_Catch==false){
			t_list * mensajes_Catch = list_create();
			flag_Mensajes_Catch = true;
		} else{}
		list_add(mensajes_Catch, infoMensaje);
		break;

	case 4: // CAUGHT_POKEMON=4
		// Si no existe la cola, la creamos
		if(flag_Mensajes_Caught==false){
			t_list * mensajes_Caught = list_create();
			flag_Mensajes_Caught = true;
		} else{}
		list_add(mensajes_Caught, infoMensaje);
		break;

	case 5: // GET_POKEMON=5
		// Si no existe la cola, la creamos
		if(flag_Mensajes_Get==false){
			t_list * mensajes_Get = list_create();
			flag_Mensajes_Get = true;
		} else{}
		list_add(mensajes_Get, infoMensaje);
		break;

	case 6: // LOCALIZED_POKEMON=6
		// Si no existe la cola, la creamos
		if(flag_Mensajes_Localized==false){
			t_list * mensajes_Localized = list_create();
			flag_Mensajes_Localized = true;
		} else{}
		list_add(mensajes_Localized, infoMensaje);
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
