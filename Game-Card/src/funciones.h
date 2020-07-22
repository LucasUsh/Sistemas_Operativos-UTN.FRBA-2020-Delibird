#ifndef SRC_FUNCIONES_H_
#define SRC_FUNCIONES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/bitarray.h>
#include <commons/collections/dictionary.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include "/home/utnso/workspace/tp-2020-1c-5rona/OurLibraries/UniversoPokemon/universo.h"

//Punteros globales:
extern t_log* logger_GC;

extern char* punto_de_montaje;

extern t_dictionary* semaforos;

//Datos globales:

extern t_bitarray mapa_de_bloques;

extern sem_t diccionario;
extern sem_t bitmap;

extern int32_t tiempo_reintento_operacion;
extern int32_t tiempo_retardo_operacion;
extern int32_t tamanio_bloque;
extern int32_t cantidad_bloques;
extern int32_t tam_punto_de_montaje;


// Funciones que no requieren liberar memoria:
void funcion_new_pokemon(t_New* new);
void funcion_catch_pokemon(t_Catch* catch);
void funcion_get_pokemon(t_Get* get);

int32_t existe (char* nombre_pokemon);
off_t crear_metadata_new (t_New* new, char* ruta);
off_t asignar_bloque_libre(void);
char get_open (char* ruta_metadata);
void set_open (char* ruta_metadata, char valor);
FILE* abrir_para (char* ruta, char* modo);
void actualizar_bitmap (void);
char* traer_bloques(char** bloques, int32_t cantidad);

extern void salir (const char* mensaje);

// Funciones que requieren liberar memoria:
char* ruta_metadata_pokemon_teorica (t_pokemon pokemon);
char* ruta_carpeta_pokemon_teorica (t_pokemon pokemon);
FILE* abrir_para (char* ruta, char* modo);
char* copiar_stream(FILE* archivo_lectura, char* ruta_archivo);
char* copiar_linea(int numero_de_linea, FILE* archivo_lectura, char* ruta_archivo);

#endif
