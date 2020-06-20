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


bool flag_Suscriptores_New, flag_Suscriptores_Appeared, flag_Suscriptores_Catch, flag_Suscriptores_Caught, flag_Suscriptores_Get, flag_Suscriptores_Localized= false;
bool flag_Mensajes_New, flag_Mensajes_Appeared, flag_Mensajes_Catch, flag_Mensajes_Caught, flag_Mensajes_Get, flag_Mensajes_Localized= false;
int32_t sizeMemoria;

double get_id(){
	//para obtener id usamos el timestamp

	struct timeval tv;
	gettimeofday(&tv, NULL);
	double id =((double)tv.tv_sec) * 1000 + (double)(tv.tv_usec) / 1000;
	//printf("el id es: %d\n", id);

	return id;
}

void escucharSuscripciones(char* IP_BROKER, char* PUERTO_BROKER){
	//iniciar_servidor(IP_BROKER, PUERTO_BROKER);
	printf("Estoy escuchando suscripciones\n");
	int32_t socketSuscripciones = crear_socket_escucha(IP_BROKER, PUERTO_BROKER);

	while(socketSuscripciones != -1){
		struct sockaddr_in dir_cliente;
		int32_t tam_direccion = sizeof(struct sockaddr_in);
		int32_t socketCliente = accept(socketSuscripciones, (void*) &dir_cliente, &tam_direccion);
		printf("Se conecto un cliente\n");

		int32_t operacion;
		if(recv(socketCliente, &operacion, sizeof(int32_t), MSG_WAITALL) == -1){
			operacion = -1;
		}
		switch (operacion) {
		case NEW_POKEMON:
			printf("Recibi un new_pokemon");
			break;
		case APPEARED_POKEMON:
			printf("Recibi un appeared_pokemon");
			break;
		case CATCH_POKEMON:
			printf("Recibi un catch_pokemon");
			break;
		case CAUGHT_POKEMON:
			printf("Recibi un caught_pokemon");
			break;
		case GET_POKEMON:
			printf("Recibi un get_pokemon");
			break;
		case LOCALIZED_POKEMON:
			printf("Recibi un localized_pokemon");
			break;
		case 0:
			exit(2);
		case -1:
			exit(2);
		default:
			return;
		}
		/*recv(socketCliente, &(buffer_size), sizeof(buffer_size), 0);
		recv(socketCliente, buffer, buffer_size, 0);*/
	}
}

int32_t main(void) {



	//Lau: creo que no hace falta
	//malloc(sizeof(bool)*12); // uno por cada cola de mensajes y suscriptores

	logger = iniciar_logger();
	config = leer_config();
	tabla_particiones = list_create();

	//get_id();

	sizeMemoria = atoi(config_get_string_value(config, "TAMANO_MEMORIA"));
	int inicioMemoria = (int)malloc(sizeMemoria);

	t_particion* particion_inicial = malloc(sizeof(t_particion));
	particion_inicial->posicion_inicial = inicioMemoria;
	particion_inicial->posicion_final = inicioMemoria + sizeMemoria;
	particion_inicial->size = sizeMemoria;

	list_add(tabla_particiones, particion_inicial);

	char *IP_BROKER = config_get_string_value(config, "IP_BROKER");
	char *PUERTO_BROKER = config_get_string_value(config, "PUERTO_BROKER");
	log_info(logger,"Lei IP_BROKER %s ",IP_BROKER);
	log_info(logger,"Lei PUERTO_BROKER %s ",PUERTO_BROKER);

	/*
	* Creamos un hilo que debe estar constantemente escuchando con IP_BROKER y PUERTO_BROKER.
	* El resto de los procesos se conectan a ese hilo, de a uno.
	*/

	// esto tendria que ser otro hilo
	while (1){
		escucharSuscripciones(IP_BROKER, PUERTO_BROKER);
	}



	/*
	* Se les asigna un nuevo socket para que envien o reciban mensajes, se agrega el proceso a la
	* lista de suscriptores a la que solicito unirse, Se verifica si hay mensajes sin ACK en la lista
	* a la que suscribio un proceso, en caso de que si haya se envia por el nuevo socket y se
	* los saca del socket de suscripciones para poder seguir atendiendo otras suscripciones.
	*
	* Luego, debera haber un hilo por cada proceso suscripto a una cola especficia.
	* Ejemplo: Team1 se suscribe a mensajes_Appeared, a mensajes_Get y a mensajes_Localized
	* entonces tiene que tener 3 hilos que manejaran los distintos mensajes que se envien o reciban
	* por cada socket.
	* Tenemos que implementar semaforos para asignar los ID de mensajes, agregar, enviar y recibir
	* mensajes, ya que habra concurrencia entre los distintos hilos
	*
	*/



	return EXIT_SUCCESS;

}

/*void recibir_Suscripciones(char* IP_BROKER, char* PUERTO_BROKER) {
	int32_t socket_servidor_GC = crear_socket_escucha(IP_BROKER, PUERTO_BROKER);
	int32_t socket_cliente_entrante;

	while(1) {
    	socket_cliente_entrante = recibir_cliente(int32_t socket_servidor);



    	pthread_create(&hilo_global_cliente_GC, NULL, (void*) responder_mensaje, &socket_cliente_entrante);
    	pthread_detach(hilo_global_cliente_GC);
    }
}

void responder_mensaje(int32_t* socket_cliente) {

	int32_t codigo_operacion;

	if(recv(*socket_cliente, &codigo_operacion, sizeof(int32_t), MSG_WAITALL) == -1)
			codigo_operacion = -1;

	//int32_t size;
	//void* msg;
	switch (codigo_operacion) {

		case 0:
			//msg = recibir_mensaje_servidor(socket_cliente, &size);
			//printf("Recibi el siguiente mensaje: %s", (char*) msg);
			//devolver_mensaje(msg, size, socket_cliente);
			//free(msg);
			break;

		case -1:
			printf ("Error al recibir paquete en serve_client. Hilo finalizado.");
			pthread_exit(NULL);
	}
}
*/



void suscribirProceso(op_code operacion, int32_t * PID){
	switch(operacion) {
	case SUSCRIPCION_NEW: // 0
		// Si no existe la cola, la creamos
		if (flag_Suscriptores_New == false){
			suscriptores_New = list_create();
			flag_Suscriptores_New= true;
		}

		list_add(suscriptores_New, PID);

		break;

	case SUSCRIPCION_APPEARED: // 	1
		// Si no existe la cola, la creamos
		if (flag_Suscriptores_Appeared == false){
			suscriptores_Appeared = list_create();
			flag_Suscriptores_Appeared= true;
		}

		list_add(suscriptores_Appeared, PID);

		break;

	case SUSCRIPCION_CATCH: // 	2
		// Si no existe la cola, la creamos
		if (flag_Suscriptores_Catch == false){
			suscriptores_Catch = list_create();
			flag_Suscriptores_Catch=true;
		}

		list_add(suscriptores_Catch, PID);

		break;

	case SUSCRIPCION_CAUGHT: // 3
		// Si no existe la cola, la creamos
		if (flag_Suscriptores_Caught == false){
			suscriptores_Caught = list_create();
			flag_Suscriptores_Caught= true;
		}

		list_add(suscriptores_Caught, PID);

		break;

	case SUSCRIPCION_GET: // 4
		// Si no existe la cola, la creamos
		if (flag_Suscriptores_Get == false){
			suscriptores_Get = list_create();
			flag_Suscriptores_Get= true;
		}

		list_add(suscriptores_Get, PID);

		break;

	case SUSCRIPCION_LOCALIZED: // 5
		// Si no existe la cola, la creamos
		if (flag_Suscriptores_Localized == false){
			suscriptores_Localized = list_create();
			flag_Suscriptores_Localized=true;
		}

		list_add(suscriptores_Localized, PID);

		break;
	default:
		return;
	}
}

void agregarMensaje(op_code operacion, info_Mensaje * infoMensaje){
	//void queue_push(t_queue *, void *element);

	switch(operacion) {
	case NEW_POKEMON: // 6
		// Si no existe la cola, la creamos
		if(flag_Mensajes_New== false){
			mensajes_New = list_create();
			flag_Mensajes_New = true;
		}
		list_add(mensajes_New, infoMensaje);
		break;

	case APPEARED_POKEMON: // 7
		// Si no existe la cola, la creamos
		if(flag_Mensajes_Appeared==false){
			mensajes_Appeared = list_create();
			flag_Mensajes_Appeared = true;
		}
		list_add(mensajes_Appeared, infoMensaje);
		break;

	case CATCH_POKEMON: // 	8
		// Si no existe la cola, la creamos
		if(flag_Mensajes_Catch==false){
			mensajes_Catch = list_create();
			flag_Mensajes_Catch = true;
		}
		list_add(mensajes_Catch, infoMensaje);
		break;

	case CAUGHT_POKEMON: // 9
		// Si no existe la cola, la creamos
		if(flag_Mensajes_Caught==false){
			mensajes_Caught = list_create();
			flag_Mensajes_Caught = true;
		}
		list_add(mensajes_Caught, infoMensaje);
		break;

	case GET_POKEMON: // 10
		// Si no existe la cola, la creamos
		if(flag_Mensajes_Get==false){
			mensajes_Get = list_create();
			flag_Mensajes_Get = true;
		}
		list_add(mensajes_Get, infoMensaje);
		break;

	case LOCALIZED_POKEMON: // 11
		// Si no existe la cola, la creamos
		if(flag_Mensajes_Localized==false){
			mensajes_Localized = list_create();
			flag_Mensajes_Localized = true;
		}
		list_add(mensajes_Localized, infoMensaje);
		break;

	default:
		return;

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
	config = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Broker/config/Broker.config");
	if(config== NULL){
		printf("No pude leer la config\n");
		exit(2);
	}
	return config;
}



t_particion generarParticionDinamicamente(int32_t sizeMensaje, t_config* config){
	// Se genera una partición del temanio del Mensaje
	t_particion particionNueva;
	int32_t sizeMinParticion = atoi(config_get_string_value(config, "TAMANO_MINIMO_PARTICION"));

	if(sizeMensaje < sizeMinParticion){
		particionNueva.size = sizeMinParticion;
	} else {
		particionNueva.size = sizeMensaje;
	}

	particionNueva.ocupada = true;

	//Agregar el info_mensaje?


	return particionNueva;
}

int getMemoriaOcupada(){
	int memoriaOcupada = 0;

	int i = 0;
	for(i = 0; i < tabla_particiones->elements_count; i++){
		t_particion* particion_actual = list_get(tabla_particiones, i);
		memoriaOcupada += particion_actual->size;
	}

	return memoriaOcupada;
}

int getMemoriaDisponible(){
	return sizeMemoria - getMemoriaOcupada();
}

int32_t getSizePokemon(t_pokemon pokemon){
	return sizeof(typeof(pokemon.size_Nombre)) + sizeof(typeof(char)) * pokemon.size_Nombre;
}

int32_t getSizeMensajeNew(t_New msgNew){
	/*
	 * ‘Pikachu’ 5 10 2
	 * largo del nombre del pokémon
	 * el nombre del pokemon
	 * posición X
 	 * posicion Y
	 * cantidad
	*/
	int32_t sizeMsg = 0;
	sizeMsg +=getSizePokemon(msgNew.pokemon) +
			sizeof(typeof(t_posicion)) +
			sizeof(typeof(msgNew.cant));

	return sizeMsg;
}

int32_t getSizeMensajeLocalized(t_Localized msgLocalized){
	/*
	 * ‘Pikachu’ 3 4 5 1 5 9 3
	 * el largo del nombre del pokémon,
	 * el nombre del pokemon,
	 * la cantidad de posiciones donde se encuentra
	 * y un par de int_32 para cada posición donde se encuentre. (2 * int_32 * cant_posiciones)
	*/

	int32_t sizeMsg = 0;
	sizeMsg += getSizePokemon(msgLocalized.pokemon) +
			sizeof(typeof(msgLocalized.listaPosiciones->elements_count)) +
			sizeof(typeof(t_posicion)) * msgLocalized.listaPosiciones->elements_count;

	return sizeMsg;
}

int32_t getSizeMensajeGet(t_Get msgGet){
	/*
	 * ‘Pikachu’
	 * el largo del nombre del pokémon,
	 * el nombre del pokemon,
	*/

	int32_t sizeMsg = 0;
	sizeMsg += getSizePokemon(msgGet.pokemon);

	return sizeMsg;
}

int32_t getsizeMensajeAppeared(t_Appeared msgAppeared){
	/*
	 * ‘Pikachu’ 1 5
	 * el largo del nombre del pokémon,
	 * el nombre del pokemon,
	 * Pos X
	 * Pos Y
	*/

	int32_t sizeMsg = 0;
	sizeMsg += getSizePokemon(msgAppeared.pokemon)+
				  sizeof(typeof(t_posicion));

	return sizeMsg;
}

int32_t getsizeMensajeCatch(t_Catch msgCatch){
	/*
	 * ‘Pikachu’ 1 5
	 * el largo del nombre del pokémon,
	 * el nombre del pokemon,
	 * Pos X
	 * Pos Y
	*/

	int32_t sizeMsg = 0;
	sizeMsg += getSizePokemon(msgCatch.pokemon)+
				  sizeof(typeof(t_posicion));

	return sizeMsg;
}

int32_t getsizeMensajeCaught(t_Caught msgCaught){
	/*
	 * 0
	 * un uint_32 para saber si se puedo o no atrapar al pokemon
	*/

	return sizeof(typeof(sizeof(msgCaught.fueAtrapado)));
}


bool particionCandidata(t_particion* particion, int32_t sizeMensaje){
	return !particion->ocupada && sizeMensaje < particion->size;
}

t_particion* getParticionFirstFit(int32_t sizeMensaje){

	bool _particionCandidata(void* element){
		return particionCandidata((t_particion*)element, sizeMensaje);
	}

	t_list* particionesCandidatas = list_filter(tabla_particiones, _particionCandidata);

	return list_get(particionesCandidatas, 0);
}

t_particion* getParticionBestFit(int32_t sizeMensaje){
	int i;
	t_particion* mejorParticion = malloc(sizeof(t_particion));

	bool _particionCandidata(void* element){
		return particionCandidata((t_particion*)element, sizeMensaje);
	}

	t_list* particionesCandidatas = list_filter(tabla_particiones, _particionCandidata);

	mejorParticion = list_get(particionesCandidatas, 0); //agarro la primera

	//recorro a partir de la segunda
	for(i = 1; i < particionesCandidatas->elements_count; i++){
		t_particion* particionActual = list_get(particionesCandidatas, i);

		if(particionActual->size - sizeMensaje == 0){
			return particionActual;
		} else if(particionActual->size < mejorParticion->size){
			mejorParticion = particionActual;
		}
	}

	return mejorParticion;

}

t_particion crearParticion(int32_t inicio, int32_t fin, int32_t size, bool ocupada){
	t_particion newParticion;
	newParticion.ocupada = false;
	newParticion.posicion_final = fin;
	newParticion.posicion_inicial = inicio;
	newParticion.size = size;

	return newParticion;
}


void pruebaLista(){

	/*
	 * el objetivo de esta prueba es para que quede claro como removiendo un item de una lista
	 * el puntero elements_count se actualiza al instante
	 *
	 * Este caso nos viene bien para sacar un partición de la lista y dividirla en otras 2
	 *
	 * Por ejemplo: Entra un mensaje que ocupa 8 bytes.
	 * Por medio del algoritmo First fit se inserta en una de 10 bytes
	 * Esto va a generar una partición nueva de 8 bytes para el dato y una de 2 bytes restantes
	 * Estos 2 bytes podrían terminar siendo fragmentación interna por ejemplo si el tamaño min de particion es de 3 bytes.
	*/
	printf("PRUEBA LISTAS -> \n");
	t_list* lsprueba = list_create();
	list_add(lsprueba, 125);//0
	list_add(lsprueba, 198);//1
	list_add(lsprueba, 3553);//2
	list_add(lsprueba, 4128);//3

	int item2 = list_get(lsprueba, 2);
	printf("item en la pos 2: %d\n", item2);

	int i;
	for(i=0;i < lsprueba->elements_count;i++){
		printf("item en pos %d: %d\n",i, list_get(lsprueba, i));
	}


	item2 = list_remove(lsprueba, 2);
	printf("saque el elemento: %d\n", item2);

	for(i=0;i < lsprueba->elements_count;i++){
		printf("item en pos %d: %d\n",i, list_get(lsprueba, i));
	}

	list_add_in_index(lsprueba, 2, 3000);
	list_add_in_index(lsprueba, 3, 553);

	printf("agregue el elemento: 3000 en la pos 2\n");
	printf("agregue el elemento: 553 en la pos 3\n");

	for(i=0;i < lsprueba->elements_count;i++){
		printf("item en pos %d: %d\n",i, list_get(lsprueba, i));
	}

}

