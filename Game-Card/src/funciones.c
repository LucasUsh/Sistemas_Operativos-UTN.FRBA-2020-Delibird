#include "funciones.h"

void funcion_new_pokemon(t_New* new) {

	log_info(logger_GC, "Creando a %s...\n", new->pokemon.nombre);

	char* ruta_metadata = ruta_metadata_pokemon_teorica (new->pokemon);

	sem_wait (&diccionario);
	if (existe (ruta_metadata)) {
		sem_post (&diccionario);

		sem_wait (dictionary_get(semaforos, new->pokemon.nombre));
		while (get_open (ruta_metadata) == 'Y') {
			sem_post (dictionary_get(semaforos, new->pokemon.nombre));
			sleep (tiempo_reintento_operacion);
			sem_wait (dictionary_get(semaforos, new->pokemon.nombre));
		}
		set_open (ruta_metadata, 'Y');
		FILE* metadata = abrir_para (ruta_metadata, "r");
		char* linea_con_bloques = copiar_linea(2, metadata, ruta_metadata);
		fclose(metadata);
		sem_post (dictionary_get(semaforos, new->pokemon.nombre));
		/* ZONA EN DESARROLLO:
		char** linea_separada = string_split(linea_con_bloques, "=");
		int32_t cantidad = cantidad_de_bloques (linea_separada[1]);

		char** bloques_juntos = string_get_string_as_array(linea_separada[1]);

		char* archivo_cargado = traer_bloques(bloques_juntos, cantidad);



		sleep(tiempo_retardo_operacion);
*/
		sem_wait (dictionary_get(semaforos, new->pokemon.nombre));
		set_open (ruta_metadata, 'N');
		sem_post (dictionary_get(semaforos, new->pokemon.nombre));
/*
		free(linea_con_bloques);
		string_iterate_lines(linea_separada, (void*) free);
		free(linea_separada);
		string_iterate_lines(bloques_juntos, (void*) free);
		free(bloques_juntos);
		free(archivo_cargado);*/
	}
	else {
		sem_t* nuevo_semaforo = malloc(sizeof(sem_t));
		dictionary_put (semaforos, new->pokemon.nombre, nuevo_semaforo);
		sem_init (dictionary_get (semaforos, new->pokemon.nombre), 0, 1);
		off_t bloque = crear_metadata_new (new, ruta_metadata);
		sem_post (&diccionario);
		// Escribir bloque
		sleep (tiempo_retardo_operacion);
	}

	//TODO: enviar appeared_pokemon al broker

	free (ruta_metadata);
}

void funcion_catch_pokemon(t_Catch* catch) {
	printf("Catch %s...\n", catch->pokemon.nombre);
}

void funcion_get_pokemon(t_Get* get) {

}

/****************************************************************************/
/**********************************AUXILIARES********************************/
/****************************************************************************/

int32_t existe (char* ruta) {
	struct stat estado_archivo;
	return (stat (ruta, &estado_archivo) == 0);
}

off_t crear_metadata_new (t_New* new, char* ruta) {
	char* carpeta_metadata = ruta_carpeta_pokemon_teorica(new->pokemon);
	if (mkdir (carpeta_metadata, S_IRWXU | S_IROTH) != 0) salir("Error al crear directorio para metadata del nuevo pokemon.");

	off_t bloque = asignar_bloque_libre();

	FILE* ruta_metadata_new = abrir_para (ruta, "w+");

	fprintf (ruta_metadata_new, "DIRECTORY=N\n");
	fprintf (ruta_metadata_new, "SIZE=%d\n", tamanio_bloque);
	fprintf (ruta_metadata_new, "BLOCKS=[%ld]\n", bloque);
	fprintf (ruta_metadata_new, "OPEN=N\n");

	fclose(ruta_metadata_new);
	free(carpeta_metadata);

	return bloque;
}

off_t asignar_bloque_libre() {
	off_t indice = 0;
	sem_wait(&bitmap);
	while(bitarray_test_bit(&mapa_de_bloques, indice)) indice++;
	bitarray_set_bit(&mapa_de_bloques, indice);
	actualizar_bitmap();
	sem_post(&bitmap);
	return indice;
}

char get_open (char* ruta_metadata) {
	FILE* metadata = abrir_para (ruta_metadata, "r");

	char* stream = copiar_stream(metadata, ruta_metadata);

	fclose (metadata);

	char** lineas = string_split(stream, "\n");
	free(stream);

	if (string_contains(lineas[3], "OPEN=Y")) {
		string_iterate_lines(lineas, (void*) free);
		free(lineas);
		return 'Y';
	}
	string_iterate_lines(lineas, (void*) free);
	free(lineas);
	return 'N';
}

void set_open (char* ruta_metadata, char valor) {

	FILE* metadata = abrir_para (ruta_metadata, "r+");

	char* stream = copiar_stream(metadata, ruta_metadata);

	fclose (metadata);

	metadata = abrir_para (ruta_metadata, "w");

	char** lineas = string_split(stream, "\n");
	free(stream);

	lineas[3][5] = valor;

	int i = 0;
	while(lineas[i] != NULL) {
		fputs(lineas[i], metadata);
		fputc('\n', metadata);
		i++;
	}

	fclose (metadata);
	string_iterate_lines(lineas, (void*) free);
	free(lineas);
}

void actualizar_bitmap () {
	char ruta_archivo_Metadata_Bitmap[tam_punto_de_montaje + strlen ("/Metadata/Bitmap.bin") + 1];
	strcat(strcpy(ruta_archivo_Metadata_Bitmap, punto_de_montaje), "/Metadata/Bitmap.bin");
	FILE* bitmap = fopen (ruta_archivo_Metadata_Bitmap, "w+");
	fwrite (mapa_de_bloques.bitarray, sizeof(char), mapa_de_bloques.size, bitmap);
	fclose(bitmap);
}

int32_t cantidad_de_bloques (char* vector){
	int32_t i = 1, comas = 0;
	while (vector[i] != ']') {
		if (vector[i] == ',') comas++;
		i++;
	}
	if (i == 1 || i == 2) return (i-1);
	return comas+1;
}

/*¡¡¡Se debe liberar memoria luego de usarlas!!!*/
char* ruta_carpeta_pokemon_teorica (t_pokemon pokemon) {

	char* ruta = malloc (tam_punto_de_montaje + strlen ("/Files/") + pokemon.size_Nombre);

	return strcat (strcat (strcpy(ruta, punto_de_montaje), "/Files/"), pokemon.nombre);
}

char* ruta_metadata_pokemon_teorica (t_pokemon pokemon) {

	char* ruta = malloc (tam_punto_de_montaje + strlen("/Files//Metadata.bin") + pokemon.size_Nombre);

	return strcat (strcat (strcat (strcpy(ruta, punto_de_montaje), "/Files/"), pokemon.nombre), "/Metadata.bin");
}

FILE* abrir_para (char* ruta, char* modo) {
	FILE* archivo = fopen(ruta, modo);
	if (archivo == NULL) salir("Fallo de fopen");
	return archivo;
}

char* copiar_stream(FILE* archivo_lectura, char* ruta_metadata) {

	struct stat estado_archivo;
	stat(ruta_metadata, &estado_archivo);

	char* stream = calloc(1, estado_archivo.st_size); //SAQUE EL -1 TENER EN CUENTA

	fread(stream, estado_archivo.st_size, 1, archivo_lectura);

	return stream;
}

char* copiar_linea (int numero_de_linea, FILE* archivo_lectura, char* ruta_archivo) {
	char* stream = copiar_stream (archivo_lectura, ruta_archivo);
	char** lineas = string_split(stream, "\n");
	free(stream);

	char* linea_pedida = string_substring_from(lineas[numero_de_linea], 0);

	string_iterate_lines(lineas, (void*) free);
	free(lineas);
	return linea_pedida;
}

char* traer_bloques(char** bloques, int32_t cantidad) {

	if (cantidad == 0) return NULL;

	char carpeta_Blocks[tam_punto_de_montaje + strlen ("/Blocks") + 1];
	strcat(strcpy(carpeta_Blocks, punto_de_montaje), "/Blocks");

	int32_t i = 0;
	struct stat estado_binario;
	char* carpeta_dinamica = NULL;
	char* datos_copiados = NULL;
	char* auxiliar = NULL;
	FILE* binario = NULL;
	int32_t tamanio_total = 0, tamanio_previo = 0;

	while (i < cantidad) {
		carpeta_dinamica = malloc(strlen (carpeta_Blocks) + strlen("/.bin") + strlen(bloques[i]));
		strcat (strcat (strcat (strcpy(carpeta_dinamica, punto_de_montaje), "/Blocks/"), bloques[i]), ".bin");
		stat(carpeta_dinamica, &estado_binario);

		tamanio_previo = tamanio_total;
		tamanio_total += estado_binario.st_size;

		binario = abrir_para (carpeta_dinamica, "r");
		auxiliar = copiar_stream(binario, carpeta_dinamica);
		fclose(binario);

		datos_copiados = realloc (datos_copiados, tamanio_total);

		memcpy(datos_copiados[tamanio_previo], auxiliar, estado_binario.st_size);

		free(carpeta_dinamica);
		free(auxiliar);
		i++;
	}
	return datos_copiados;
}











