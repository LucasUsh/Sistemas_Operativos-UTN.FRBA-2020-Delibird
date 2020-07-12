#include "Game-Card.h"

int32_t main(void)
{
	instalar_filesystem ();

	debug = log_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Card/debug.log", "Game-Card", 1, LOG_LEVEL_DEBUG);
	pthread_t hilo_servidor_GC;
	if (pthread_create (&hilo_servidor_GC, NULL, (void *) &crear_servidor_GC, NULL) == 0)
		log_debug (debug, "Hilo servidor creado correctamente.");

	// TODO: asociarse globalmente a las colas NEW_POKEMON, CATCH y GET
	// Una vez suscripto tendríamos entonces 3 sockets modo cliente,
	// cuando llega un mensaje informar al Broker la recepción del mismo (ACK) y hacer lo que corresponda


	//pthread_t h1;
	//int32_t socket;
	//logger_GC = log_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Card/Game-Card.log", "Game-Card", 1, LOG_LEVEL_INFO);

    //pthread_create(&h1, NULL, (void*) &conexionBroker, &socket);

    //pthread_join(h1,NULL);
    //liberar_conexion(socket);
    //config_destroy(config_GC);
    //log_destroy(logger_GC);

	pthread_join(hilo_servidor_GC, NULL);

    return 0;
}

void instalar_filesystem (){
	//Información del archivo de configuración:
	config_GC = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Card/Game-Card.config");
	punto_de_montaje = config_get_string_value(config_GC,"PUNTO_MONTAJE_TALLGRASS");
	tam_bloque = config_get_string_value(config_GC,"BLOCK_SIZE");
	cant_bloques = config_get_string_value(config_GC,"BLOCKS");
	magic_number = config_get_string_value(config_GC,"MAGIC_NUMBER");
	config_destroy(config_GC);

	int32_t tam_punto_de_montaje = strlen (punto_de_montaje);

	//Creación de carpetas:

	if (mkdir (punto_de_montaje, S_IRWXU | S_IROTH) != 0) salir("Error al crear la carpeta TallGrass");

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
	char ruta_archivo_metadata[strlen(carpeta_Metadata) + strlen ("/Metadata.bin") + 1];
	strcat(strcpy(ruta_archivo_metadata, carpeta_Metadata), "/Metadata.bin");
	char ruta_archivo_bitmap[strlen(carpeta_Metadata) + strlen ("/Bitmap.bin") + 1];
	strcat(strcpy(ruta_archivo_bitmap, carpeta_Metadata), "/Bitmap.bin");

	FILE* file_auxiliar = fopen (ruta_archivo_metadata, "w+");
	fprintf (file_auxiliar, "BLOCK_SIZE=%s\n", tam_bloque);
	fprintf (file_auxiliar, "BLOCKS=%s\n", cant_bloques);
	fprintf (file_auxiliar, "BLOCK_SIZE=%s\n", magic_number);
	fclose(file_auxiliar);

	file_auxiliar = fopen (ruta_archivo_bitmap, "w+");
	int32_t tam_bitmap = ((int32_t) atoi (tam_bloque)) / 8;



	fclose(file_auxiliar);
}

void crear_servidor_GC() {
	t_config* config = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Card/Game-Card.config");
	char* ip_gamecard = config_get_string_value(config_GC,"IP_GAMECARD");
	char* puerto_gamecard = config_get_string_value(config_GC,"PUERTO_GAMECARD");
	config_destroy(config);

	int32_t socket_servidor_GC = crear_socket_escucha(ip_gamecard, puerto_gamecard);
	int32_t socket_cliente_entrante;

    while(1) {
    	socket_cliente_entrante = recibir_cliente(socket_servidor_GC);

    	if (pthread_create(&hilo_global_cliente_GC, NULL, (void*) responder_mensaje, &socket_cliente_entrante) == 0)
    		log_debug (debug, "Hilo para responder al cliente creado correctamente.");

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

	log_debug (debug, "Código de operación %d", codigo_operacion);

	switch (codigo_operacion) {

		case NEW_POKEMON:
			;
			t_New* new = NULL;
			new = deserializar_paquete_new (socket_cliente);


			break;

		case CATCH_POKEMON:
			;
			t_Catch* catch = NULL;
			catch = deserializar_paquete_catch (socket_cliente);

			log_debug(debug, "Nombre: %s", catch->pokemon.nombre);
			log_debug(debug, "Posicion: x %d, y %d", catch->posicion.X, catch->posicion.Y);

			break;

		case GET_POKEMON:
			;
			t_Get* get = NULL;
			get = deserializar_paquete_get (socket_cliente);

			log_debug(debug, "Nombre: %s", get->pokemon.nombre);

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
