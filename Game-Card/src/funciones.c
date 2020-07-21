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
		sem_post (dictionary_get(semaforos, new->pokemon.nombre));

		//Hago los cambios en el archivo real

		sem_wait (dictionary_get(semaforos, new->pokemon.nombre));
		set_open (ruta_metadata, 'N');
		sem_post (dictionary_get(semaforos, new->pokemon.nombre));


	}
	else {
		sem_t* nuevo_semaforo = malloc(sizeof(sem_t));
		dictionary_put (semaforos, new->pokemon.nombre, nuevo_semaforo);
		sem_init (dictionary_get (semaforos, new->pokemon.nombre), 0, 1);
		crear_metadata_new (new, ruta_metadata);
		sleep (tiempo_retardo_operacion);
		sem_post (&diccionario);
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

void crear_metadata_new (t_New* new, char* ruta) {
	char* carpeta_metadata = ruta_carpeta_pokemon_teorica(new->pokemon);
	if (mkdir (carpeta_metadata, S_IRWXU | S_IROTH) != 0) salir("Error al crear directorio para metadata del nuevo pokemon.");

	FILE* ruta_metadata_new = fopen (ruta, "w+");

	fprintf (ruta_metadata_new, "DIRECTORY=N\n");
	fprintf (ruta_metadata_new, "SIZE=%d\n", tamanio_bloque);
	fprintf (ruta_metadata_new, "BLOCKS=[%ld]\n", asignar_bloque_libre());
	fprintf (ruta_metadata_new, "OPEN=N\n");

	fclose(ruta_metadata_new);
	free(carpeta_metadata);
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
	FILE* archivo = abrir_para (ruta_metadata, "r");

	struct stat estado_archivo;
	stat(ruta_metadata, &estado_archivo);

	char* stream = calloc(1, estado_archivo.st_size + 1);
	fread(stream, estado_archivo.st_size, 1, archivo);
	fclose (archivo);

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

	char* stream = copiar_stream(ruta_metadata);

	FILE* archivo = abrir_para (ruta_metadata, "w");

	char** lineas = string_split(stream, "\n");
	free(stream);

	lineas[3][5] = valor;

	int i = 0;
	while(lineas[i] != NULL) {
		fputs(lineas[i], archivo);
		fputc('\n', archivo);
		i++;
	}

	fclose (archivo);
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

char* copiar_stream(char* ruta_metadata) {
	FILE* archivo = abrir_para (ruta_metadata, "r");

	struct stat estado_archivo;
	stat(ruta_metadata, &estado_archivo);

	char* stream = calloc(1, estado_archivo.st_size + 1);

	fread(stream, estado_archivo.st_size, 1, archivo);
	fclose(archivo);

	return stream;
}
















