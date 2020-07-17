#include "funciones.h"

void funcion_new_pokemon(t_New* new)
{
	printf("Funcion new %s \n",new->pokemon.nombre);

	//TODO: Verificar si existe en fileSystem sino crearlo
	//TODO: Abrir el archivo...
	//TODO: enviar appeared_pokemon al broker
}

bool existe (char* nombre_pokemon) {

	char carpeta_archivo[strlen(punto_de_montaje) + strlen ("/Files/") + strlen (nombre_pokemon) + 1];
	strcat (strcat (strcpy (carpeta_archivo, punto_de_montaje), "/Files/"), nombre_pokemon);

	struct stat estado_archivo;
	int32_t estado = stat (carpeta_archivo, &estado_archivo);

	if (estado == -1 && errno == ENOENT) return true;
	else return false;
}
