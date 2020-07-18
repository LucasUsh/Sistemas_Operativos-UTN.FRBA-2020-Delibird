#include "funciones.h"

void funcion_new_pokemon(t_New* new) {

	printf("Creando a %s...\n", new->pokemon.nombre);

	char* ruta_metadata = ruta_metadata_pokemon_teorica (new->pokemon);

	//TODO: Verificar si existe en fileSystem sino crearlo

	if (existe (ruta_metadata)) {
		FILE* archivo_pokemon = fopen (ruta_metadata, "r+");

	}
	else {

	}

	//TODO: Abrir el archivo...
	//TODO: enviar appeared_pokemon al broker
	free (ruta_metadata);
}

int32_t existe (char* ruta) {
	struct stat estado_archivo;
	return (stat (ruta, &estado_archivo) == 0);
}

/*¡¡¡Se debe liberar memoria luego de usarla!!!*/
char* ruta_metadata_pokemon_teorica (t_pokemon pokemon) {

	char* ruta = malloc (strlen(punto_de_montaje) + strlen("/Files//Metadata.bin") + pokemon.size_Nombre);

	return strcat (strcat (strcat (strcpy(ruta, punto_de_montaje), "/Files/"), pokemon.nombre), "/Metadata.bin");
}
