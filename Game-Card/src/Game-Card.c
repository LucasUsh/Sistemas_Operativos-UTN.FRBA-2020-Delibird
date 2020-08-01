#include "Game-Card.h"

int32_t main(void)
{
	inicializaciones_globales();

	instalar_filesystem ();

	pthread_t hilo_servidor_GC;
	if (pthread_create (&hilo_servidor_GC, NULL, (void *) &crear_servidor_GC, NULL) == 0)
		log_info (logger_GC, "Hilo servidor creado correctamente.");

	pthread_t hilo_new;
	if (pthread_create (&hilo_new, NULL, (void*) &hilo_suscriptor, SUSCRIPCION_NEW) == 0)
		log_debug (logger_GC, "Hilo cola new creado correctamente.");

	pthread_t hilo_catch;
	if (pthread_create(&hilo_catch, NULL, (void*) &hilo_suscriptor, SUSCRIPCION_CATCH) == 0)
		log_debug (logger_GC, "Hilo cola catch creado correctamente.");

	pthread_t hilo_get;
	if (pthread_create(&hilo_get, NULL, (void*) &hilo_suscriptor, SUSCRIPCION_GET) == 0)
		log_debug (logger_GC, "Hilo cola get creado correctamente.");

	pthread_join(hilo_new,NULL);
	pthread_join(hilo_catch,NULL);
	pthread_join(hilo_get,NULL);
	pthread_join(hilo_servidor_GC, NULL);

	liberar_memoria();

    return 0;
}

void instalar_filesystem (){
	// Información del archivo de configuración:

	tiempo_reintento_op = config_get_string_value(config_GC,"TIEMPO_DE_REINTENTO_OPERACION");
	tiempo_retardo_op = config_get_string_value(config_GC,"TIEMPO_RETARDO_OPERACION");
	punto_de_montaje = config_get_string_value(config_GC,"PUNTO_MONTAJE_TALLGRASS");
	tam_bloque = config_get_string_value(config_GC,"BLOCK_SIZE");
	cant_bloques = config_get_string_value(config_GC,"BLOCKS");
	magic_number = config_get_string_value(config_GC,"MAGIC_NUMBER");

	tiempo_reintento_operacion = (int32_t) atoi (tiempo_reintento_op);
	tiempo_retardo_operacion = (int32_t) atoi (tiempo_retardo_op);
	tamanio_bloque = (int32_t) atoi (tam_bloque);
	cantidad_bloques = (int32_t) atoi (cant_bloques);
	tam_punto_de_montaje = strlen (punto_de_montaje);

	// Creación de carpetas:

	if (mkdir (punto_de_montaje, S_IRWXU | S_IRWXO) != 0) {
		if (errno == EEXIST) {
			log_info(logger_GC, "Utilizacion de file system previamente formateado.");
			recuperar_datos();
			log_info(logger_GC, "Datos administrativos recobrados.");
			return;
		}
		else salir ("Problema al intentar crear la carpeta TallGrass.");
	}

	fclose(logger_GC->file);
	logger_GC->file = fopen("/home/utnso/workspace/tp-2020-1c-5rona/Game-Card/Game-Card.log", "w+");
	if (logger_GC->file == NULL) salir ("Error al crear el logger durante la instalacion.");
	log_info(logger_GC, "Instalando el sistema de archivos TallGrass...");

	char carpeta_Files[tam_punto_de_montaje + strlen ("/Files") + 1];
	strcat(strcpy(carpeta_Files, punto_de_montaje), "/Files");
	char carpeta_Metadata[tam_punto_de_montaje + strlen ("/Metadata") + 1];
	strcat(strcpy(carpeta_Metadata, punto_de_montaje), "/Metadata");
	char carpeta_Blocks[tam_punto_de_montaje + strlen ("/Blocks") + 1];
	strcat(strcpy(carpeta_Blocks, punto_de_montaje), "/Blocks");

	if (mkdir (carpeta_Metadata, S_IRWXU | S_IROTH) != 0) salir("Error al crear la carpeta Metadata");
	if (mkdir (carpeta_Files, S_IRWXU | S_IROTH) != 0) salir("Error al crear la carpeta Files");
	if (mkdir (carpeta_Blocks, S_IRWXU | S_IROTH) != 0) salir("Error al crear la carpeta Blocks");

	// Creacion de archivos administrativos:
	char ruta_archivo_Metadata_Metadata[strlen(carpeta_Metadata) + strlen ("/Metadata.bin") + 1];
	strcat(strcpy(ruta_archivo_Metadata_Metadata, carpeta_Metadata), "/Metadata.bin");
	char ruta_archivo_Metadata_Bitmap[strlen(carpeta_Metadata) + strlen ("/Bitmap.bin") + 1];
	strcat(strcpy(ruta_archivo_Metadata_Bitmap, carpeta_Metadata), "/Bitmap.bin");
	char ruta_archivo_Files_Metadata [strlen(carpeta_Files) + strlen ("/Metadata.bin") + 1];
	strcat(strcpy(ruta_archivo_Files_Metadata, carpeta_Files), "/Metadata.bin");

	FILE* file_auxiliar = fopen (ruta_archivo_Metadata_Metadata, "w+");
	fprintf (file_auxiliar, "BLOCK_SIZE=%s\n", tam_bloque);
	fprintf (file_auxiliar, "BLOCKS=%s\n", cant_bloques);
	fprintf (file_auxiliar, "BLOCK_SIZE=%s\n", magic_number);
	fclose(file_auxiliar);

	/*Bitmap*/
	if (cantidad_bloques % 8 == 0) mapa_de_bloques.size = (size_t) cantidad_bloques / 8;
	else mapa_de_bloques.size = (size_t) (cantidad_bloques / 8 + 1);
	mapa_de_bloques.mode = LSB_FIRST;
	mapa_de_bloques.bitarray = malloc (mapa_de_bloques.size);
	int32_t i;
	for (i=0; i < cantidad_bloques; i++) bitarray_clean_bit(&mapa_de_bloques, i);
	if (cantidad_bloques % 8 != 0) {
		int32_t bits_de_mas = cantidad_bloques % 8;
		for (; bits_de_mas > 0; bits_de_mas--, i++) bitarray_set_bit(&mapa_de_bloques, i);
	}
	file_auxiliar = fopen (ruta_archivo_Metadata_Bitmap, "w+");
	fwrite (mapa_de_bloques.bitarray, sizeof(char), mapa_de_bloques.size, file_auxiliar);
	fclose(file_auxiliar);

	file_auxiliar = fopen (ruta_archivo_Files_Metadata, "w+");
	fprintf (file_auxiliar, "DIRECTORY=Y");
	fclose(file_auxiliar);

	// Inicializo semaforos:
	sem_init (&bitmap, 0, 1);

	log_info(logger_GC, "Listo.");
}

void recuperar_datos() {
	char carpeta_Files[tam_punto_de_montaje + strlen ("/Files") + 1];
	strcat(strcpy(carpeta_Files, punto_de_montaje), "/Files");
	char carpeta_Metadata[tam_punto_de_montaje + strlen ("/Metadata") + 1];
	strcat(strcpy(carpeta_Metadata, punto_de_montaje), "/Metadata");
	char ruta_archivo_Metadata_Bitmap[strlen(carpeta_Metadata) + strlen ("/Bitmap.bin") + 1];
	strcat(strcpy(ruta_archivo_Metadata_Bitmap, carpeta_Metadata), "/Bitmap.bin");

	// Recupero el mapa de bits del disco:
	if (cantidad_bloques % 8 == 0) mapa_de_bloques.size = (size_t) cantidad_bloques / 8;
	else mapa_de_bloques.size = (size_t) (cantidad_bloques / 8 + 1);
	mapa_de_bloques.mode = LSB_FIRST;
	mapa_de_bloques.bitarray = malloc (mapa_de_bloques.size);
	FILE* aux = fopen (ruta_archivo_Metadata_Bitmap, "r");
	if (aux == NULL) salir("Error al cargar el mapa de bits existente");
	fread (mapa_de_bloques.bitarray, sizeof(char), mapa_de_bloques.size, aux);
	fclose(aux);

	// Recupero semáforos:
	DIR *Files = opendir(carpeta_Files);
	if (Files == NULL) salir("No se pudo abrir la carpeta Files para recuperar semaforos");

	struct dirent *entrada;

	sem_t* nuevo_semaforo;

	while ((entrada = readdir (Files)) != NULL) {
	    if ((strcmp(entrada->d_name, ".")!=0) && (strcmp(entrada->d_name, "..")!=0) && (strcmp(entrada->d_name, "Metadata.bin")!=0)){
	    	nuevo_semaforo = malloc(sizeof(sem_t));
	    	dictionary_put (semaforos, entrada->d_name, nuevo_semaforo);
	    	sem_init (dictionary_get (semaforos, entrada->d_name), 0, 1);
	    }
	}

	closedir (Files);

	sem_init (&bitmap, 0, 1);
}

void crear_servidor_GC() {

	char* ip_gamecard = config_get_string_value(config_GC,"IP_GAMECARD");
	char* puerto_gamecard = config_get_string_value(config_GC,"PUERTO_GAMECARD");
	int32_t operacion=0;
	int32_t tamanio_estructura = 0;
	int32_t id_mensaje=0;

	int32_t socket_servidor_GC = crear_socket_escucha(ip_gamecard, puerto_gamecard);
	int32_t socket_cliente_entrante;


    while(1) {
    	socket_cliente_entrante = recibir_cliente(socket_servidor_GC);

    	recv(socket_cliente_entrante, &operacion, sizeof(int32_t), MSG_WAITALL);
		recv(socket_cliente_entrante, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
		recv(socket_cliente_entrante, &id_mensaje, sizeof(int32_t), MSG_WAITALL);

		responder_mensaje(socket_cliente_entrante, operacion);
    }
}


void responder_mensaje(int32_t socket_cliente, op_code codigo_operacion) {
	pthread_t hilo;

	switch (codigo_operacion) {

		case NEW_POKEMON:
			;
			t_New* new = NULL;
			new = deserializar_paquete_new (&socket_cliente);
			enviar_ACK(0, socket_cliente);
			log_debug (logger_GC, "Pokemon: %s, Posicion: (%d, %d), Cantidad: %d", new->pokemon.nombre, new->posicion.X, new->posicion.Y, new->cant);


			if (pthread_create(&hilo, NULL, (void*)funcion_new_pokemon, new) == 0) {
				log_info (logger_GC, "Hilo para responder NEW_POKEMON creado correctamente.");
			}
			pthread_detach(hilo);

			break;

		case CATCH_POKEMON:
			;
			t_Catch* catch = NULL;
			catch = deserializar_paquete_catch (&socket_cliente);
			enviar_ACK(0, socket_cliente);

			log_debug(logger_GC, "Nombre: %s, Posicion: (%d, %d)", catch->pokemon.nombre, catch->posicion.X, catch->posicion.Y);

			if (pthread_create(&hilo, NULL, (void*)funcion_catch_pokemon, catch) == 0) {
				log_info (logger_GC, "Hilo para responder CATCH_POKEMON creado correctamente.");
			}
			pthread_detach(hilo);

			break;

		case GET_POKEMON:
			;
			t_Get* get = NULL;
			get = deserializar_paquete_get (&socket_cliente);
			enviar_ACK(0, socket_cliente);

			log_debug(logger_GC, "Nombre: %s", get->pokemon.nombre);

			break;

		default:

			break;
	}
}

void hilo_suscriptor(op_code code){
	int32_t operacion=0;
	int32_t tamanio_estructura = 0;
	int32_t id_mensaje=0;
	int32_t socket_broker_new = crear_conexion(ip_broker,puerto_broker);
	bool fin = false;

	while(1){
		if(socket_broker_new != 0){
			enviar_handshake(2, socket_broker_new);
			if(recv(socket_broker_new, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
				if(operacion == ACK){
					recv(socket_broker_new, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
					recv(socket_broker_new, &id_mensaje, sizeof(int32_t), MSG_WAITALL);


					enviar_suscripcion(code, socket_broker_new);
					if(recv(socket_broker_new, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
						if(operacion == ACK){
							recv(socket_broker_new, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
							recv(socket_broker_new, &id_mensaje, sizeof(int32_t), MSG_WAITALL);

							while(fin == false){
								if(recv(socket_broker_new, &operacion, sizeof(int32_t), MSG_WAITALL) != -1){
									recv(socket_broker_new, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
									recv(socket_broker_new, &id_mensaje, sizeof(int32_t), MSG_WAITALL);
									log_debug(logger_GC, "ID de Mensaje: %d", id_mensaje);
									responder_mensaje(socket_broker_new, operacion);

									} else {
										log_debug(logger_GC, "Se cayo la conexion");
										fin = true;
										}
							}
					}else printf("Fallo la suscripcion, respondieron algo que no era un ACK\n");
				}else printf("Se cayo la conexion\n");
			}else printf("Fallo el ACK del handshake\n");
		}else{
			sleep(tiempo_reintento_conexion);
			socket_broker_new = crear_conexion(ip_broker,puerto_broker);
			}
		}
	}
}

void salir (const char* mensaje) {
	log_error(logger_GC, mensaje);
	perror (mensaje);
	exit (EXIT_FAILURE);
}

void inicializaciones_globales() {
	logger_GC = log_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Card/Game-Card.log", "Game-Card", true, LOG_LEVEL_DEBUG);
	config_GC = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Card/Game-Card.config");
	sem_init (&diccionario, 0, 1);
	semaforos = dictionary_create();
	tiempo_reintento_con = config_get_string_value(config_GC,"TIEMPO_DE_REINTENTO_CONEXION");
	tiempo_reintento_conexion = (int32_t) atoi (tiempo_reintento_con);
	ip_broker = config_get_string_value(config_GC,"IP_BROKER");
	puerto_broker = config_get_string_value(config_GC,"PUERTO_BROKER");
//	id_proceso = atoi(config_get_string_value(config_GC,"ID_PROCESO"));
	id_proceso = 2;
}

void liberar_memoria() {
	free(mapa_de_bloques.bitarray);
	bitarray_destroy(&mapa_de_bloques);

    config_destroy(config_GC);
    log_destroy(logger_GC);

    sem_destroy(&bitmap);
    sem_destroy(&diccionario);

    dictionary_destroy_and_destroy_elements(semaforos, (void*) sem_destroy);
}

