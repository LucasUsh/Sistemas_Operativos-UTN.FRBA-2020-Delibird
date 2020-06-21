#include "mensajes.h"

void enviar_new_pokemon(char* pokemon, char* x, char* y, char* cantidad, int32_t socket_cliente)
{
	t_paquete * paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = NEW_POKEMON;
	paquete->buffer = malloc(sizeof(t_buffer));

	t_posicion * posicion = malloc(sizeof(t_posicion));
	posicion->X = (int32_t) atoi (x);
	posicion->Y = (int32_t) atoi (y);

	t_pokemon * p_pokemon = malloc(sizeof(t_pokemon));
	p_pokemon->size_Nombre = strlen(pokemon) +1;
	p_pokemon->nombre = pokemon;

	t_New * new = malloc(sizeof(t_New));
	new->cant = (int32_t) atoi (cantidad);
	new->posicion = *posicion;
	new->pokemon = *p_pokemon;

	paquete->buffer->size = tamanio_new (new);
	paquete->buffer->id_Mensaje = 0;
	paquete->buffer->stream = new;

//	free(posicion);
//	free(p_pokemon);
//	free(new);

	int32_t bytes_a_enviar;
	void *paqueteSerializado = serializar_paquete_new (paquete, &bytes_a_enviar, new);

	send(socket_cliente, paqueteSerializado, bytes_a_enviar, 0);

	free(posicion);
	free(p_pokemon);
	free(new);


	free(paqueteSerializado);
	free(paquete->buffer);
	free(paquete);
}

void enviar_appeared_pokemon(char* pokemon, char* x, char* y, char* id_mensaje, int32_t socket_cliente)
{
	printf("Appeared Pokemon \n");
}

void enviar_catch_pokemon(char* pokemon, char* x, char* y, int32_t socket_cliente)
{
	printf("Catch Pokemon \n");
}

void enviar_caught_pokemon(char* pokemon, char* id_mensaje, char* ok, int32_t socket_cliente)
{
	printf("Caught Pokemon \n");
}

void enviar_get_pokemon(char* pokemon, int32_t socket_cliente)
{

}

