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

	paquete->buffer->size = sizeof(new)*3; //por que *3??
	paquete->buffer->id_Mensaje = 0;
	paquete->buffer->stream = new;
	//memcpy(paquete->buffer->stream, new, paquete->buffer->size);

	free(new);
	free(posicion);
	free(p_pokemon);

	int32_t bytes_a_enviar;
	void *paqueteSerializado = serializar_paquete(paquete, &bytes_a_enviar);

	send(socket_cliente, paqueteSerializado, bytes_a_enviar, 0);

	free(paqueteSerializado);
	free(paquete->buffer);
	free(paquete);
}
