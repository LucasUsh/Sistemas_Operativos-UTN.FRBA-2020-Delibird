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

bool existe_New, existe_Appeared, existe_Catch, existe_Caught, existe_Get, existe_Localized = false;

int main(void) {
	malloc(sizeof(bool)*6);

	logger = iniciar_logger();
	config = leer_config();

	iniciar_servidor();
	return EXIT_SUCCESS;

}

void suscribirACola(op_code operacion, int * PID){
	switch(operacion) {
	case NEW_POKEMON: // NEW_POKEMON = 1
		while(existe_New == false){
			t_queue * queue_New = queue_create();
			existe_New = true;
		}
		//queue_push(queue_New_Pokemon, PID);
		break;
	case APPEARED_POKEMON: // 	APPEARED_POKEMON=2
		while(existe_Appeared == false){
			t_queue * queue_Appeared = queue_create();
			existe_Appeared = true;
		}
		//queue_push(queue_Appeared, PID);
		break;
	case CATCH_POKEMON: // 	CATCH_POKEMON=3
		while(existe_Catch == false){
			t_queue * queue_Catch = queue_create();
			existe_Catch = true;
		}
		//queue_push(queue_Catch, PID);
		break;
	case CAUGHT_POKEMON: // CAUGHT_POKEMON=4
		while(existe_Caught == false){
			t_queue * queue_Caught = queue_create();
			existe_Caught = true;
		}
		//queue_push(queue_Caught, PID);
		break;
	case GET_POKEMON: // GET_POKEMON=5
		while(existe_Get == false){
			t_queue * queue_Get = queue_create();
			existe_Get = true;
		}
		//queue_push(queue_Get, PID);
		break;
	case LOCALIZED_POKEMON: // LOCALIZED_POKEMON=6
		while(existe_Localized == false){
			t_queue * queue_Localized = queue_create();
			existe_Localized = true;
		}
		//queue_push(queue_Localized, PID);
		break;
	}
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
	config = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Broker/Broker.config");
	if(config== NULL){
		printf("No pude leer la config\n");
		exit(2);
	}
	return config;
}
