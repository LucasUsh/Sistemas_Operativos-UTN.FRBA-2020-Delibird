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
void crear_metadata_new (t_New* new, char* ruta);
off_t asignar_bloque_libre(void);
char get_open (char* ruta_metadata);
void set_open (char* ruta_metadata, char valor);
FILE* abrir_para (char* ruta, char* modo);
void actualizar_bitmap (void);
int32_t cantidad_de_bloques (char* vector);
void liberar_bloques (char** strings_bloques, int32_t cantidad, char* ruta_metadata, char* pokemon);
char* apuntar_a_posicion (char* archivo_cargado, t_posicion posicion);
char* agregar_cantidad(char* archivo_cargado, char* apuntador, int32_t cantidad_nueva);
char* agregar_nueva_posicion(char* archivo_cargado, t_posicion posicion_nueva, int32_t cantidad_nueva);
void volcar_archivo_cargado(char* archivo_cargado, char* ruta_metadata, char* pokemon);
void metadata_agregar_bloque (char* ruta_metadata, char* pokemon, char* bloque);
void metadata_actualizar_size (char* ruta_metadata, char* pokemon, int32_t size_nuevo);
void metadata_volcar (char* ruta_metadata, char* pokemon, char* archivo_cargado);

extern void salir (const char* mensaje);

// Funciones que requieren liberar memoria:
char* ruta_metadata_pokemon_teorica (t_pokemon pokemon);
char* ruta_carpeta_pokemon_teorica (t_pokemon pokemon);
FILE* abrir_para (char* ruta, char* modo);
char* copiar_stream(FILE* archivo_lectura, char* ruta_archivo);
char* copiar_linea(int32_t numero_de_linea, FILE* archivo_lectura, char* ruta_archivo);
char* traer_bloques(char** bloques, int32_t cantidad);
char* posicion_a_string (t_posicion posicion);
char* metadata_traer (char* ruta_metadata, char* pokemon, int32_t* tam_alojamiento);

#endif
