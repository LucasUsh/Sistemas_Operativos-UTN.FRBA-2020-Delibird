/*
 ============================================================================
 Name        : team.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "team.h"
#include "pokemon_utils.h"


int32_t cantidadEntrenadores = 0;
t_list* cola_ready;
t_list* mensajes_localized_a_planificar;
t_list* objetivo_global;




t_posicion avanzar(t_posicion posicion, int32_t posX, int32_t posY){
	int32_t nuevaPosicionX = posicion.X + posX;
	int32_t nuevaPosicionY = posicion.Y + posY;

	posicion.X = nuevaPosicionX;
	posicion.Y = nuevaPosicionY;

	return posicion;
}

void planificar_fifo(){

	printf("planificando FIFO...\n");
	while(cola_ready->elements_count > 0){
		printf("proximo entrenador..\n");

		// en fifo, el proximo entrenador es el que esté primero en la cola de ready
		t_entrenador* entrenador = list_remove(cola_ready, 0);
		entrenador->estado = EXEC;

		int32_t posicion_final_X = entrenador->posicion_destino.X - entrenador->posicion.X;
		int32_t posicion_final_Y = entrenador->posicion_destino.Y - entrenador->posicion.Y;

		printf("posicion vieja: x-> %d, y-> %d\n", entrenador->posicion.X, entrenador->posicion.Y);

		entrenador->posicion = avanzar(entrenador->posicion, posicion_final_X , posicion_final_Y);

		printf("posicion nueva: x-> %d, y-> %d\n", entrenador->posicion.X, entrenador->posicion.Y);

		//una vez que lo muevo llamo al broker y hago el catch

	}

	return;
}

void simular_fifo(t_list* entrenadores)
{
	printf("simulando fifo... \n");
	t_posicion posPok;

	posPok.X = 3;
	posPok.Y = 5;

	t_entrenador* entrenador_mas_cercano = get_entrenador_mas_cercano(entrenadores, posPok);

	entrenador_mas_cercano->estado = READY;
	entrenador_mas_cercano->posicion_destino = posPok;

	list_add(cola_ready, entrenador_mas_cercano);


	printf("elementos en ready: %d\n", cola_ready->elements_count);

	planificar_fifo();

	printf("elementos en ready: %d\n", cola_ready->elements_count);

}

void planificar_rr(){
	// en fifo, el proximo entrenador es el que esté primero en la cola de ready
	printf("planificando RR...\n");
	while(cola_ready->elements_count > 0){
	t_entrenador* entrenador = list_remove(cola_ready, 0);
	entrenador->estado = EXEC;

	int32_t posicion_final_X = entrenador->posicion_destino.X - entrenador->posicion.X;
	int32_t posicion_final_Y = entrenador->posicion_destino.Y - entrenador->posicion.Y;

	printf("posicion vieja: x-> %d, y-> %d\n", entrenador->posicion.X, entrenador->posicion.Y);

	entrenador->posicion = avanzar(entrenador->posicion, posicion_final_X , posicion_final_Y);

	printf("posicion nueva: x-> %d, y-> %d\n", entrenador->posicion.X, entrenador->posicion.Y);

	//una vez que lo muevo llamo al broker y hago el catch
	}

	return;
}

void planificar_sjfsd(){
	return;
}

void planificar_sjfcd(){
	return;
}

void planificar(t_algoritmo* algoritmo)
{

	switch(algoritmo->algoritmo_code){
	case FIFO:
		planificar_fifo();
		break;
	case RR:
		planificar_rr();
		break;
	case SJFSD:
		planificar_sjfsd();
		break;
	case SJFCD:
		planificar_sjfcd();
		break;
	default:
		return;
	}

}

void show_entrenadores(t_algoritmo* algoritmo, t_list* entrenadores, t_list* objetivo_global){
	printf("PLANIFICACION: \n	ALGORITMO: %s, QUANTUM: %d\n", algoritmo->algoritmo_string, algoritmo->quantum);
	    printf("********************\n");


	    void _mostrar_pokemon(void* elemento){
	    	return mostrar_pokemon((t_pokemon_team*)elemento, objetivo_global);
	    }


	    for(int32_t i = 0; i < entrenadores->elements_count; i++){
	    	t_entrenador* entrenador_actual = list_get(entrenadores, i);
	    	printf("|ENTRENADOR %d\n|----------------\n|POSICION: (%d,%d)\n",
	    			i + 1,
					entrenador_actual->posicion.X,
					entrenador_actual->posicion.Y);


	    	printf("|POKEMONES:\n");
	    	list_iterate(entrenador_actual->pokemones, _mostrar_pokemon);

	    	printf("|OBJETIVOS:\n");
	    	list_iterate(entrenador_actual->objetivo, _mostrar_pokemon);


			printf("********************\n");

		}

		printf("|OBJETIVO GLOBAL:\n");
		list_iterate(objetivo_global, _mostrar_pokemon);

		printf("********************\n");
}

void generar_y_enviar_get(t_list* objetivo_global){
	int i=0;
	for(i = 0; i < objetivo_global->elements_count; i++){
		t_pokemon_team* pokemon_actual = list_get(objetivo_global, i);

		t_Get mensaje_get;
		mensaje_get.pokemon.nombre = pokemon_actual->nombre;
		mensaje_get.pokemon.size_Nombre = string_length(pokemon_actual->nombre); // +1?

		printf("mandando el mensaje GET %s\n", mensaje_get.pokemon.nombre);
		//conectarse al broker y mandar el mensaje;
	}

	return;
}

t_Localized* generar_localized(char* pokemon, int cant_posiciones){
	t_list* listaPosiciones = list_create();
	t_Localized* mensaje = malloc(sizeof(t_Localized)); // reemplazar por funcion get size Localized de broker

	for(int i = 0; i < cant_posiciones; i++){
		t_posicion* posicion = malloc(sizeof(t_posicion));
		posicion->X = (rand() % (10)) + 1; // numero random entre 1 y 10
		posicion->Y = (rand() % (10)) + 1;

		list_add(listaPosiciones, posicion);
	}

	mensaje->listaPosiciones = listaPosiciones;
	mensaje->pokemon.nombre = pokemon;
	mensaje->pokemon.size_Nombre = string_length(pokemon); // creo que hay que sumar 1

	return mensaje;
}

t_list* simular_localized(int cant_mensajes){
	/*
	 * El objetivo de esta funcion es generar mensajes localized de forma aleatoria
	 * */
	srand(time(NULL));
	t_list* mensajes_localized = list_create();
	t_list* nombre_pokemones = get_nombres_pokemon();


	for(int i = 0; i < cant_mensajes; i++){
		//el nombre lo obtengo de forma aleatoria
		char* nombre_pokemon = get_nombre_aleatorio(nombre_pokemones);

		//con el nombre, genero tambien X cantidad de posiciones aleatorias
		t_Localized* mensaje_localized = generar_localized(nombre_pokemon, 1);

		list_add(mensajes_localized, mensaje_localized);
	}

	return mensajes_localized;

}

t_list* filtrar_localized_repetidos(t_list* mensajes_localized){

	//								**************************************
	// EN REALIDAD, PUEDEN LLEGAR REPETIDOS, HAY QUE COMPARAR SI SE PUEDE SACIAR O NO EL OBJETIVO GLOBAL
	//								**************************************

	t_list* mensajes_filtrados = list_create();
		for(int i = 0; i < mensajes_localized->elements_count; i++){
			t_Localized* mensaje = list_get(mensajes_localized, i);
			bool mensaje_encontrado = false;

			if(mensajes_filtrados->elements_count == 0){
				list_add(mensajes_filtrados, mensaje);
			} else {

				for(int j = 0; j < mensajes_filtrados->elements_count; j++){
					t_Localized* mensaje_filtrado = list_get(mensajes_filtrados, j);
					if(string_equals_ignore_case(mensaje_filtrado->pokemon.nombre, mensaje->pokemon.nombre)){
						mensaje_encontrado=true;
					}
				}

				if(!mensaje_encontrado){
					list_add(mensajes_filtrados, mensaje);
				}
			}
		}

		return mensajes_filtrados;
}

t_list* filtrar_localized_objetivo_global(t_list* mensajes_localized, t_list* objetivo_global){
	t_list* mensajes_filtrados = list_create();
	printf("%d\n", objetivo_global->elements_count);
	for(int i = 0; i < mensajes_localized->elements_count; i++){
		t_Localized* mensaje = list_get(mensajes_localized, i);

		for(int j = 0; j < objetivo_global->elements_count; j++){
			t_pokemon_team* objetivo_actual = list_get(objetivo_global, j);
			if(string_equals_ignore_case(mensaje->pokemon.nombre, objetivo_actual->nombre)){
				list_add(mensajes_filtrados, mensaje);
				break;
			}
		}
	}

	return mensajes_filtrados;
}

t_list* filtrar(t_list* mensajes_localized,t_list* objetivo_global){
	return filtrar_localized_objetivo_global(filtrar_localized_repetidos(mensajes_localized), objetivo_global);
}

void show_cola_ready(){
	for(int i = 0; i < cola_ready->elements_count; i++){
		t_entrenador* entrenador_actual = list_get(cola_ready, i);

		printf("posicion en la lista: %d\n", i);
		printf("posicion actual: X:%d, Y:%d\n", entrenador_actual->posicion.X, entrenador_actual->posicion.Y);
		printf("posicion destino: X:%d, Y:%d\n", entrenador_actual->posicion_destino.X, entrenador_actual->posicion_destino.Y);
	}
}

void hilo_generador_mensajes_localized(){
	while(1){
		//cada 30 segs agrego un mensaje localized
		t_list* mensajes_localized = simular_localized(1);

		//filtro los mensajes que me interesan
		list_add(mensajes_localized_a_planificar, list_get(filtrar(mensajes_localized, objetivo_global), 0));

		sleep(30);//espera de 30 segs
	}
}

int32_t main(int32_t argc, char** argv)
{
	cola_ready = list_create();
	objetivo_global = list_create();


    printf("el entrenador que se va a cargar es el de la config: %s\n", argv[1] );
    //char* config_name = argv[1];

    //t_config* entrenador_config = config_create(argv[1]);
    t_config* entrenador_config = config_create("/home/utnso/workspace/tp-2020-1c-5rona/team/config/entrenador1.config");


    int32_t cantidadEntrenadores = array_length(config_get_array_value(entrenador_config, "POKEMON_ENTRENADORES"));
    printf("En este team hay %d entrenadores\n", cantidadEntrenadores);

    t_algoritmo* algoritmo = get_algoritmo(entrenador_config);
    t_list* entrenadores = get_entrenadores(entrenador_config, cantidadEntrenadores);


    objetivo_global = get_objetivo_global(entrenadores);
    //show_entrenadores(algoritmo, entrenadores, objetivo_global);


    //hilo_generador_de_mensajes
    generar_y_enviar_get(objetivo_global);

    t_list* mensajes_localized_parseados = simular_localized(3);

    mensajes_localized_a_planificar = filtrar(mensajes_localized_parseados, objetivo_global);



    printf("***************************************** LLEGARON LOS SIGUIENTES MENSAJES LOCALIZED \n");

   for(int j = 0; j < mensajes_localized_parseados->elements_count ; j++){
	t_Localized* mensaje = list_get(mensajes_localized_parseados, j);
	printf("pokemon: %s\n", mensaje->pokemon.nombre);
	printf("cant de posiciones: %d\n", mensaje->listaPosiciones->elements_count);

	for(int m=0; m < mensaje->listaPosiciones->elements_count; m++){
		t_posicion* posicion = list_get(mensaje->listaPosiciones, m);
		printf("Pos %d:\n", m);
		printf("posX: %d\n", posicion->X);
		printf("posY: %d\n", posicion->Y);
	}

	printf("*****************************************\n");

   }


   printf("***************************************** SE FILTRARON LOS SIGUIENTES MENSAJES LOCALIZED \n");

   for(int j = 0; j < mensajes_localized_a_planificar->elements_count ; j++){
	t_Localized* mensaje_filtrado = list_get(mensajes_localized_a_planificar, j);
	printf("pokemon: %s\n", mensaje_filtrado->pokemon.nombre);
	printf("cant de posiciones: %d\n", mensaje_filtrado->listaPosiciones->elements_count);

	for(int m=0; m < mensaje_filtrado->listaPosiciones->elements_count; m++){
		t_posicion* posicion = list_get(mensaje_filtrado->listaPosiciones, m);
		printf("Pos %d:\n", m);
		printf("posX: %d\n", posicion->X);
		printf("posY: %d\n", posicion->Y);
	}

	printf("*****************************************\n");

   }

   for(int n = 0; n < mensajes_localized_a_planificar->elements_count; n++){
	 t_Localized* mensaje = list_get(mensajes_localized_a_planificar, n);
	 t_posicion* posicion = list_get(mensaje->listaPosiciones, 0);

 	 t_entrenador* entrenador_mas_cercano = get_entrenador_mas_cercano(entrenadores, *posicion);

     entrenador_mas_cercano->estado = READY;
 	 entrenador_mas_cercano->posicion_destino = *posicion;

	 list_add(cola_ready, entrenador_mas_cercano);

   }

   //show_cola_ready();


   planificar(algoritmo);

   printf("End");

   return EXIT_SUCCESS;
}











