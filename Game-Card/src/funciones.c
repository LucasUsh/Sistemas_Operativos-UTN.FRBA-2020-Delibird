#include "funciones.h"

void funcion_new_pokemon(t_New* new)
{
	printf("Funcion new %s \n",new->pokemon);

	config_GC = config_create("/home/utnso/workspace/tp-2020-1c-5rona/Game-Card/Game-Card.config");
	punto_de_montaje = config_get_string_value(config_GC,"PUNTO_MONTAJE_TALLGRASS");

	//TODO: Verificar si existe en fileSystem sino crearlo
	//TODO: Abrir el archivo...
	//TODO: enviar appeared_pokemon al broker
}
