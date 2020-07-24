#include "funciones.h"

void funcion_new_pokemon(t_New* new) {

	log_info(logger_GC, "Iniciando operacion New Pokemon para %s...\n", new->pokemon.nombre);

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

		char** linea_dividida = string_split(linea_con_bloques, "=");

		int32_t cantidad = cantidad_de_bloques (linea_dividida[1]);

		char** strings_bloques = string_get_string_as_array(linea_dividida[1]);

		char* archivo_cargado = traer_bloques(strings_bloques, cantidad);

		liberar_bloques (strings_bloques, cantidad);

		char* apuntador = apuntar_a_posicion(archivo_cargado, new->posicion);
		if (apuntador != NULL) agregar_cantidad(archivo_cargado, apuntador, new->cant);
		else agregar_nueva_posicion(archivo_cargado, new->posicion, new->cant);

		volcar_archivo_cargado(archivo_cargado, ruta_metadata, new->pokemon.nombre);

		sleep(tiempo_retardo_operacion);

		sem_wait (dictionary_get(semaforos, new->pokemon.nombre));
		set_open (ruta_metadata, 'N');
		sem_post (dictionary_get(semaforos, new->pokemon.nombre));

		free(linea_con_bloques);
		string_iterate_lines(linea_dividida, (void*) free);
		free(linea_dividida);
		string_iterate_lines(strings_bloques, (void*) free);
		free(strings_bloques);
		free(archivo_cargado);
	}
	else {
		sem_t* nuevo_semaforo = malloc(sizeof(sem_t));
		dictionary_put (semaforos, new->pokemon.nombre, nuevo_semaforo);
		sem_init (dictionary_get (semaforos, new->pokemon.nombre), 0, 1);
		crear_metadata_new (new, ruta_metadata);
		sem_post (&diccionario);

		char* posicion_nueva = posicion_a_string(new->posicion);
		char* cantidad_nueva = string_itoa(new->cant);
		char* archivo_nuevo = malloc (strlen(posicion_nueva) + strlen(cantidad_nueva) + 2); //Más 2 por el '=' y el '\0'
		strcat (strcat (strcpy(archivo_nuevo, posicion_nueva), "="), cantidad_nueva);
		volcar_archivo_cargado(archivo_nuevo, ruta_metadata, new->pokemon.nombre);

		sleep (tiempo_retardo_operacion);

		sem_wait(dictionary_get(semaforos, new->pokemon.nombre));
		set_open (ruta_metadata, 'N');
		sem_post(dictionary_get(semaforos, new->pokemon.nombre));

		free(posicion_nueva);
		free(cantidad_nueva);
		free(archivo_nuevo);
	}

	free (ruta_metadata);
}

void funcion_catch_pokemon(t_Catch* catch) {
	log_info(logger_GC, "Iniciando operacion Catch para%s...\n", catch->pokemon.nombre);

}

void funcion_get_pokemon(t_Get* get) {
	log_info(logger_GC, "Iniciando operacion Get para%s...\n", get->pokemon.nombre);

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

	FILE* ruta_metadata_new = abrir_para (ruta, "w+");

	fprintf (ruta_metadata_new, "DIRECTORY=N\n");
	fprintf (ruta_metadata_new, "SIZE=0\n");
	fprintf (ruta_metadata_new, "BLOCKS=[]\n");
	fprintf (ruta_metadata_new, "OPEN=Y\n");

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

void liberar_bloques (char** strings_bloques, int32_t cantidad){
	int32_t indice = 0;
	sem_wait(&bitmap);
	while(indice < cantidad) {
		bitarray_clean_bit(&mapa_de_bloques, atol(strings_bloques[indice]));
		indice++;
	}
	actualizar_bitmap();
	sem_post(&bitmap);
}

char* apuntar_a_posicion (char* archivo_cargado, t_posicion posicion) {
	char* dupla = posicion_a_string (posicion);
	char* puntero_substring = strstr(archivo_cargado, dupla);
	free(dupla);
	return puntero_substring;
}

void agregar_cantidad(char* archivo_cargado, char* apuntador, int32_t cantidad_nueva){
	int32_t i = 0;
	int32_t tam = 0;
	char* pos_cantidad_actual;

	while (apuntador[i] != '=' && apuntador[i] != '\0') i++;
	if(apuntador[i] == '\0') salir("Error en agregar_cantidad");

	i++;
	pos_cantidad_actual = apuntador + i;

	while (apuntador[i] != '\n') {tam++; i++;}

	char* str_cantidad_actual = malloc (tam+1);
	strncpy(str_cantidad_actual, pos_cantidad_actual, tam);
	str_cantidad_actual[tam] = '\0';

	int32_t cantidad_actual = (int32_t) atoi (str_cantidad_actual);
	int32_t cantidad_total = cantidad_actual + cantidad_nueva;

	char* str_cantidad_total = string_itoa(cantidad_total);
	int32_t tam_nuevo = strlen(str_cantidad_total);

	if (tam_nuevo == tam) strncpy(pos_cantidad_actual, str_cantidad_total, tam);

	else {
		int32_t diferencia = tam_nuevo - tam;

		archivo_cargado = realloc(archivo_cargado, strlen(archivo_cargado) + diferencia + 1);

		memmove(pos_cantidad_actual+tam_nuevo, pos_cantidad_actual+tam, strlen(pos_cantidad_actual) + tam + 1);

		memcpy(pos_cantidad_actual, str_cantidad_total, tam_nuevo);
	}

	free(str_cantidad_actual);
	free(str_cantidad_total);
}

void agregar_nueva_posicion(char* archivo_cargado, t_posicion posicion_nueva, int32_t cantidad_nueva) {

	char* str_posicion_nueva = posicion_a_string(posicion_nueva);

	char* str_cantidad_nueva = string_itoa(cantidad_nueva);

	char* nueva_linea = malloc(strlen(str_posicion_nueva) + strlen(str_cantidad_nueva) + 3); // +3 Para '=', '\n' y '\0'
	strcat (strcat (strcat (strcat(nueva_linea, str_posicion_nueva), "="), str_cantidad_nueva), "'\n'");

	int32_t ultima_posicion = strlen(archivo_cargado);
	archivo_cargado = realloc(archivo_cargado, ultima_posicion + strlen(nueva_linea) + 1);

	memcpy(archivo_cargado+ultima_posicion, nueva_linea, strlen(nueva_linea) + 1);

	free(str_posicion_nueva);
	free(nueva_linea);
	free(str_cantidad_nueva);
}

void volcar_archivo_cargado(char* archivo_cargado, char* ruta_metadata, char* pokemon) {
	int32_t espacio = tamanio_bloque-1;
	int32_t tam_total = strlen(archivo_cargado);

	int32_t bloques_necesarios;
	FILE* auxiliar = NULL;

	off_t bloque_libre;
	char* str_bloque_libre;
	char* ruta_dinamica;

	if (tam_total%espacio == 0) bloques_necesarios = tam_total/espacio;
	else bloques_necesarios = tam_total/espacio + 1;

	int32_t resta = tam_total;
	int32_t i;

	for (i = 0; i < bloques_necesarios; i++){
		bloque_libre = asignar_bloque_libre();
		str_bloque_libre = string_itoa (bloque_libre);

		metadata_agregar_bloque (ruta_metadata, pokemon, str_bloque_libre);

		ruta_dinamica = malloc(strlen (punto_de_montaje) + strlen("/Blocks/.bin") + strlen(str_bloque_libre) + 1);
		strcat (strcat (strcat (strcpy(ruta_dinamica, punto_de_montaje), "/Blocks/"), str_bloque_libre), ".bin");

		if (resta >= espacio){

			auxiliar = abrir_para (ruta_dinamica, "w+");
			fwrite (archivo_cargado+(i*espacio), sizeof(char), espacio, auxiliar);
			fclose (auxiliar);

			resta -= espacio;

			free(ruta_dinamica);
			free(str_bloque_libre);
		}
		else {
			auxiliar = abrir_para (ruta_dinamica, "w+");
			fwrite (archivo_cargado+(i*espacio), sizeof(char), resta, auxiliar);
			fclose (auxiliar);

			free(ruta_dinamica);
			free(str_bloque_libre);
		}
	}

	metadata_actualizar_size (ruta_metadata, pokemon, tam_total + bloques_necesarios);
}

void metadata_agregar_bloque (char* ruta_metadata, char* pokemon, char* bloque) {

	int32_t tam_archivo_cargado;

	char* archivo_cargado = metadata_traer(ruta_metadata, pokemon, &tam_archivo_cargado);

	int32_t i = 0;
	while (*(archivo_cargado+i) != ']') i++;

	if (*(archivo_cargado+i-1) == '[') {
		archivo_cargado = realloc(archivo_cargado, tam_archivo_cargado + strlen(bloque) + 1);
		memmove(archivo_cargado + i + strlen(bloque), archivo_cargado + i, strlen(archivo_cargado + i) + 1);
		memcpy(archivo_cargado + i, bloque, strlen(bloque));
	}
	else {
		char* aux = malloc(strlen(bloque)+2);
		strcat (strcpy(aux, ","), bloque);

		archivo_cargado = realloc(archivo_cargado, tam_archivo_cargado + strlen(aux) + 1);
		memmove(archivo_cargado + i + strlen(aux), archivo_cargado + i, strlen(archivo_cargado + i) + 1);
		memcpy(archivo_cargado + i, aux, strlen(aux));

		free(aux);
	}

	metadata_volcar (ruta_metadata, pokemon, archivo_cargado);

	free(archivo_cargado);
}

void metadata_actualizar_size (char* ruta_metadata, char* pokemon, int32_t size_nuevo) {

	int32_t tam_archivo_cargado;

	char* archivo_cargado = metadata_traer(ruta_metadata, pokemon, &tam_archivo_cargado);

	char* linea_size = strstr(archivo_cargado, "SIZE=");

	char* str_size_nuevo = string_itoa (size_nuevo);

	int32_t i = 5;
	while (linea_size[i] != '\n') i++;

	if(strlen(str_size_nuevo) == i) strcpy(linea_size + 5, str_size_nuevo);

	else {
		archivo_cargado = realloc(archivo_cargado, tam_archivo_cargado + strlen(str_size_nuevo) + 1);
		linea_size = strstr(archivo_cargado, "SIZE=");
		memmove(linea_size + 5 + strlen(str_size_nuevo), linea_size + i, strlen(linea_size + i) + 1);
		memcpy(linea_size + 5, str_size_nuevo, strlen(str_size_nuevo));
	}

	metadata_volcar (ruta_metadata, pokemon, archivo_cargado);

	free(archivo_cargado);
	free(str_size_nuevo);
}

void metadata_volcar (char* ruta_metadata, char* pokemon, char* archivo_cargado) {
	sem_wait (dictionary_get(semaforos, pokemon));
	FILE* metadata = abrir_para (ruta_metadata, "w+");
	fwrite(archivo_cargado, strlen(archivo_cargado) + 1, 1, metadata);
	fclose(metadata);
	sem_post (dictionary_get(semaforos, pokemon));
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

	char* stream = calloc(1, estado_archivo.st_size);

	fread(stream, estado_archivo.st_size, 1, archivo_lectura);

	return stream;
}

char* copiar_linea (int32_t numero_de_linea, FILE* archivo_lectura, char* ruta_archivo) {
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
		carpeta_dinamica = malloc(strlen (carpeta_Blocks) + strlen("/.bin") + strlen(bloques[i]) + 1);
		strcat (strcat (strcat (strcpy(carpeta_dinamica, punto_de_montaje), "/Blocks/"), bloques[i]), ".bin");
		stat(carpeta_dinamica, &estado_binario);

		tamanio_previo = tamanio_total;
		tamanio_total += estado_binario.st_size;

		binario = abrir_para (carpeta_dinamica, "r");
		auxiliar = copiar_stream(binario, carpeta_dinamica);
		fclose(binario);

		datos_copiados = realloc (datos_copiados, tamanio_total);

		memcpy(datos_copiados+tamanio_previo, auxiliar, estado_binario.st_size);

		free(carpeta_dinamica);
		free(auxiliar);
		i++;
	}
	datos_copiados = realloc (datos_copiados, tamanio_total+1);
	datos_copiados[tamanio_total] = '\0';
	return datos_copiados;
}

char* posicion_a_string (t_posicion posicion) {
	char* posicionX = string_itoa(posicion.X);
	char* posicionY = string_itoa(posicion.Y);

	char* dupla = malloc (strlen(posicionX) + strlen(posicionY) + 2); // Dos más para el '-' y el '\0'

	strcat(strcat(strcat(dupla, posicionX), "-"), posicionY);

	free(posicionX);
	free(posicionY);

	return dupla;
}

char* metadata_traer (char* ruta_metadata, char* pokemon, int32_t* tam_alojamiento) {
	struct stat estado_archivo;
	stat(ruta_metadata, &estado_archivo);

	*tam_alojamiento = estado_archivo.st_size;

	char* stream = calloc(1, *tam_alojamiento + 1);

	sem_wait (dictionary_get(semaforos, pokemon));

	FILE* metadata = abrir_para (ruta_metadata, "r");

	fread(stream, estado_archivo.st_size, 1, metadata);

	fclose(metadata);

	sem_post (dictionary_get(semaforos, pokemon));

	stream[*tam_alojamiento] = '\0';

	return stream;
}
























