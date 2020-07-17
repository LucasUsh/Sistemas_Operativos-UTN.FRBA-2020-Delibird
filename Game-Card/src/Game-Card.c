#include "Game-Card.h"

int32_t main(void)
{
	config_GC = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Card/Game-Card.config");

	instalar_filesystem ();

	/*logger_GC = log_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Card/debug.log", "Game-Card", 1, LOG_LEVEL_DEBUG);
	pthread_t hilo_servidor_GC;
	if (pthread_create (&hilo_servidor_GC, NULL, (void *) &crear_servidor_GC, NULL) == 0)
		log_debug (logger_GC, "Hilo servidor creado correctamente.");

	// TODO: asociarse globalmente a las colas NEW_POKEMON, CATCH y GET
	// Una vez suscripto tendríamos entonces 3 sockets modo cliente,
	// cuando llega un mensaje informar al Broker la recepción del mismo (ACK) y hacer lo que corresponda


	pthread_t hilo_conexion_broker;
	int32_t socket;

    pthread_create(&hilo_conexion_broker, NULL, (void*) &conexionBroker, &socket);



	pthread_join(hilo_servidor_GC, NULL);
	pthread_join(hilo_conexion_broker,NULL);

	free(mapa_de_bloques.bitarray);

    liberar_conexion(socket);
    config_destroy(config_GC);
    log_destroy(logger_GC);
*/
    return 0;
}

void instalar_filesystem (){
	//Información del archivo de configuración:

	punto_de_montaje = config_get_string_value(config_GC,"PUNTO_MONTAJE_TALLGRASS");
	tam_bloque = config_get_string_value(config_GC,"BLOCK_SIZE");
	cant_bloques = config_get_string_value(config_GC,"BLOCKS");
	int32_t cant_bloques_i = (int32_t) atoi (cant_bloques);
	magic_number = config_get_string_value(config_GC,"MAGIC_NUMBER");

	int32_t tam_punto_de_montaje = strlen (punto_de_montaje);

	char comando[tam_punto_de_montaje + strlen("rm -r ") + 1];
	strcat(strcpy(comando, "rm -r "), punto_de_montaje);

	//Creación de carpetas:

	if (mkdir (punto_de_montaje, S_IRWXU | S_IROTH) != 0) {
		if (errno == EEXIST) system(comando);
		errno = 0;
		if (mkdir (punto_de_montaje, S_IRWXU | S_IROTH) != 0) salir("Error al crear la carpeta TallGrass");
	}

	char carpeta_Metadata[tam_punto_de_montaje + strlen ("/Metadata") + 1];
	strcat(strcpy(carpeta_Metadata, punto_de_montaje), "/Metadata");
	char carpeta_Files[tam_punto_de_montaje + strlen ("/Files") + 1];
	strcat(strcpy(carpeta_Files, punto_de_montaje), "/Files");
	char carpeta_Blocks[tam_punto_de_montaje + strlen ("/Blocks") + 1];
	strcat(strcpy(carpeta_Blocks, punto_de_montaje), "/Blocks");

	if (mkdir (carpeta_Metadata, S_IRWXU | S_IROTH) != 0) salir("Error al crear la carpeta Metadata");
	if (mkdir (carpeta_Files, S_IRWXU | S_IROTH) != 0) salir("Error al crear la carpeta Files");
	if (mkdir (carpeta_Blocks, S_IRWXU | S_IROTH) != 0) salir("Error al crear la carpeta Blocks");

	//Creacion de archivos administrativos:
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

	if (cant_bloques_i % 8 == 0) mapa_de_bloques.size = (size_t) cant_bloques_i / 8;
	else mapa_de_bloques.size = (size_t) (cant_bloques_i / 8 + 1);
	mapa_de_bloques.mode = LSB_FIRST;
	mapa_de_bloques.bitarray = malloc (mapa_de_bloques.size);
	int32_t i;
	for (i=0; i < cant_bloques_i; i++) bitarray_clean_bit(&mapa_de_bloques, i);
	if (cant_bloques_i % 8 != 0) {
		int32_t bits_de_mas = cant_bloques_i % 8;
		for (; bits_de_mas > 0; bits_de_mas--, i++) bitarray_set_bit(&mapa_de_bloques, i);
	}
	file_auxiliar = fopen (ruta_archivo_Metadata_Bitmap, "w+");
	fwrite (mapa_de_bloques.bitarray, sizeof(char), mapa_de_bloques.size, file_auxiliar);
	fclose(file_auxiliar);

	file_auxiliar = fopen (ruta_archivo_Files_Metadata, "w+");
	fprintf (file_auxiliar, "DIRECTORY=Y");
	fclose(file_auxiliar);
}

void crear_servidor_GC() {

	char* ip_gamecard = config_get_string_value(config_GC,"IP_GAMECARD");
	char* puerto_gamecard = config_get_string_value(config_GC,"PUERTO_GAMECARD");

	int32_t socket_servidor_GC = crear_socket_escucha(ip_gamecard, puerto_gamecard);
	int32_t socket_cliente_entrante;

    while(1) {
    	socket_cliente_entrante = recibir_cliente(socket_servidor_GC);

    	if (pthread_create(&hilo_global_cliente_GC, NULL, (void*) responder_mensaje, &socket_cliente_entrante) == 0)
    		log_debug (logger_GC, "Hilo para responder al cliente creado correctamente.");

    	pthread_detach(hilo_global_cliente_GC); //lo desasocio aunque sigue su curso
    }

}

void responder_mensaje(int32_t* socket_cliente) {

	int32_t codigo_operacion = 0;
	int32_t tamanio_estructura = 0;
	int32_t id_mensaje = 0;

	if(recv(*socket_cliente, &codigo_operacion, sizeof(int32_t), MSG_WAITALL) == -1)
			codigo_operacion = -1;
	recv(*socket_cliente, &tamanio_estructura, sizeof(int32_t), MSG_WAITALL);
	recv(*socket_cliente, &id_mensaje, sizeof(int32_t), MSG_WAITALL);

	log_debug (logger_GC, "Código de operación %d", codigo_operacion);

	switch (codigo_operacion) {

		case NEW_POKEMON:
			;
			t_New* new = NULL;
			new = deserializar_paquete_new (socket_cliente);
			funcion_new_pokemon(new);

			break;

		case CATCH_POKEMON:
			;
			t_Catch* catch = NULL;
			catch = deserializar_paquete_catch (socket_cliente);

			log_debug(logger_GC, "Nombre: %s", catch->pokemon.nombre);
			log_debug(logger_GC, "Posicion: x %d, y %d", catch->posicion.X, catch->posicion.Y);

			break;

		case GET_POKEMON:
			;
			t_Get* get = NULL;
			get = deserializar_paquete_get (socket_cliente);

			log_debug(logger_GC, "Nombre: %s", get->pokemon.nombre);

			break;

		default:

			break;
	}
}

void conexionBroker(int32_t *socket)
{
	char* ip_broker;
	char* puerto_broker;

	config_GC = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Card/Game-Card.config");
	ip_broker = config_get_string_value(config_GC,"IP_BROKER");
	puerto_broker = config_get_string_value(config_GC,"PUERTO_BROKER");
	//char* timer = config_get_string_value(config_GC,"TIEMPO_DE_REINTENTO_CONEXION");

	*socket = crear_conexion(ip_broker,puerto_broker);
	while(*socket == 0)
	{
		sleep(1);
		*socket = crear_conexion(ip_broker,puerto_broker);
	}
	printf("\n");
	log_info(logger_GC,"Conectado al Broker");
}


void salir (const char* mensaje) {
	perror (mensaje);
	exit (EXIT_FAILURE);
}

/*
log_debug (debug, "***Estructura t_New recibida*** \n");
log_debug (debug, "Nombre: %s, tamanio: %d \n", new->pokemon.nombre, new->pokemon.size_Nombre);
log_debug (debug, "Posicion: (%d, %d) \n", new->posicion.X, new->posicion.Y);
log_debug (debug, "Cantidad: %d", new->cant);
log_debug (debug, "*******************************");
 */
