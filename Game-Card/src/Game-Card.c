#include "Game-Card.h"

int main(void)
{
	pthread_t hilo_servidor_GC;
	pthread_create (&hilo_servidor_GC, NULL, crear_servidor_GC, NULL);

	pthread_t h1;
	int socket;
	logger_GC = log_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Card/Game-Card.log", "Game-Card", 1, LOG_LEVEL_INFO);

    pthread_create(&h1, NULL, conexionBroker, &socket);


    pthread_join(h1,NULL);
    liberar_conexion(socket);
    config_destroy(config_GC);
    log_destroy(logger_GC);
    return 0;
}

void crear_servidor_GC() {
	int socket_servidor_GC;

	struct addrinfo protocolos, *info_servidor_GC, *aux;

	    memset(&protocolos, 0, sizeof(protocolos));
	    protocolos.ai_family = AF_UNSPEC;
	    protocolos.ai_socktype = SOCK_STREAM;
	    protocolos.ai_flags = AI_PASSIVE;

	    getaddrinfo(IP_GAME_CARD, PUERTO_GAME_CARD, &protocolos, &info_servidor_GC);

	    for (aux = info_servidor_GC; aux != NULL; aux = aux->ai_next)
	    {
	        if ((socket_servidor_GC = socket(aux->ai_family, aux->ai_socktype, aux->ai_protocol)) == -1)
	            continue;

	        if (bind(socket_servidor_GC, aux->ai_addr, aux->ai_addrlen) == -1) {
	            close(socket_servidor_GC);
	            continue;
	        }
	        break;
	    }

		listen(socket_servidor_GC, SOMAXCONN);

	    freeaddrinfo(info_servidor_GC);

	    while(1)
	    	esperar_cliente(socket_servidor_GC);
	}
}

void conexionBroker(int *socket)
{
	char* ip_broker;
	char* puerto_broker;
	char* timer;
	config_GC = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Card/Game-Card.config");
	ip_broker = config_get_string_value(config_GC,"IP_BROKER");
	puerto_broker = config_get_string_value(config_GC,"PUERTO_BROKER");
	timer = config_get_string_value(config_GC,"TIEMPO_DE_REINTENTO_CONEXION");

	*socket = crear_conexion(ip_broker,puerto_broker);
	while(*socket == 0)
	{
		sleep(1);
		*socket = crear_conexion(ip_broker,puerto_broker);
	}
	printf("\n");
	log_info(logger_GC,"Conectado al Broker");
}
