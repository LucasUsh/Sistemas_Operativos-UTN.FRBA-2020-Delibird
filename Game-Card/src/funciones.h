#ifndef SRC_FUNCIONES_H_
#define SRC_FUNCIONES_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdbool.h>
#include "/home/utnso/workspace/tp-2020-1c-5rona/OurLibraries/UniversoPokemon/universo.h"

extern char* punto_de_montaje;
extern char* tam_bloque;
extern char* cant_bloques;
extern char* magic_number;

void funcion_new_pokemon(t_New* new);

bool existe (char* nombre_pokemon);

#endif
