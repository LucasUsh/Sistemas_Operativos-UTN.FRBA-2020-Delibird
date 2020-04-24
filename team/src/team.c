/*
 ============================================================================
 Name        : team.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "team.h"
#include "/home/utnso/tp-2020-1c-5rona/OurLibraries/Sockets/socketsCliente.h"



int main(int argc, char** argv)
{
    printf("el entrenador que se va a cargar es el de la config: ", argc );

    printf("%s \n", argv[1]);

    char* cfg_name = string_new();
    string_append(&cfg_name, "../config/");
    string_append(&cfg_name, argv[1]);


    t_config* config = config_create(cfg_name);

	char* algoritmo = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	char* quantum = config_get_string_value(config, "QUANTUM");
	char* ip_broker = config_get_string_value(config, "IP_BROKER");
	char* puerto_broker = config_get_string_value(config, "PUERTO_BROKER");

	printf("algoritmo de planificacion: %s\n", algoritmo);
	printf("quantum: %s\n", quantum);
	printf("ip del broker: %s\n", ip_broker);
	printf("puerto del broker: %s\n", puerto_broker);

	return EXIT_SUCCESS;
}
