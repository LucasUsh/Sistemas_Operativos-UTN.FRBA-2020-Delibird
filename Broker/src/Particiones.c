/*
 * Particiones.c
 *
 *  Created on: 22 jun. 2020
 *      Author: utnso
 */

#include<commons/string.h>
#include "Particiones.h"

t_particion* crearParticion(int inicio, int fin, bool ocupada){
	t_particion* newParticion = malloc(sizeof(t_particion));
	newParticion->ocupada = false;
	newParticion->posicion_final = fin;
	newParticion->posicion_inicial = inicio;
	newParticion->size = fin - inicio;

	return newParticion;
}

t_particion generarParticionDinamicamente(int32_t sizeMensaje, t_config* config){
	// Se genera una partición del tamanio del Mensaje
	t_particion particionNueva;
	int32_t sizeMinParticion = atoi(config_get_string_value(config, "TAMANO_MINIMO_PARTICION"));

	if(sizeMensaje < sizeMinParticion){
		particionNueva.size = sizeMinParticion;
	} else {
		particionNueva.size = sizeMensaje;
	}

	particionNueva.ocupada = true;

	//Agregar el info_mensaje?


	return particionNueva;
}

bool particionCandidata(t_particion* particion, int32_t sizeMensaje){
	return !particion->ocupada && sizeMensaje <= particion->size;
}

t_particion* getParticionFirstFit(int32_t sizeMensaje){

	bool _particionCandidata(void* element){
		return particionCandidata((t_particion*)element, sizeMensaje);
	}

	t_list* particionesCandidatas = list_filter(tabla_particiones, _particionCandidata);

	return list_get(particionesCandidatas, 0);
}

t_particion* getParticionBestFit(int32_t sizeMensaje){
	int i;
	t_particion* mejorParticion = malloc(sizeof(t_particion));

	bool _particionCandidata(void* element){
		return particionCandidata((t_particion*)element, sizeMensaje);
	}

	t_list* particionesCandidatas = list_filter(tabla_particiones, _particionCandidata);

	mejorParticion = list_get(particionesCandidatas, 0); //agarro la primera

	if(mejorParticion->size - sizeMensaje != 0){ // si la primera que tomamos no es BestFit hacemos toda la logica
		//recorro a partir de la segunda
		for(i = 1; i < particionesCandidatas->elements_count; i++){
			t_particion* particionActual = list_get(particionesCandidatas, i);

			if(particionActual->size - sizeMensaje == 0){
				return particionActual;
			} else if(particionActual->size < mejorParticion->size){
				mejorParticion = particionActual;
			}
		}
	}

	return mejorParticion;

}



void dividirParticionDinamica(int indiceParticion, t_particion* particionOriginal, int32_t sizeMsg){

	/*
	 *Ej: Particion mide 20 bytes y el tamaño del mensaje es de 14
	 *Se reemplaza la de 20 por una de 14 ocupada y otra de 6 libre
	 *particion al principio:
	 *|						|
	 *1500					1520
	 *
	 *particioes despues:
	 *|						|
	 *1500		  1514		1520
	*/

	/*
	 *
	 * [............................] MP
	 *
	 * 0 -> 15 particion 1
	 * 16 -> 50 particion 2
	*/

	t_particion* primeraParticion = crearParticion(particionOriginal->posicion_inicial,
			particionOriginal->posicion_inicial + sizeMsg, false);

	int inicioSegundaParticion = primeraParticion->posicion_final + 1;

	int sizeSegundaParticion = particionOriginal->size - primeraParticion->size;

	t_particion* segundaParticion = crearParticion(inicioSegundaParticion,
			inicioSegundaParticion + sizeSegundaParticion, false);

	/*
	 *lo que habría que hacer es obtener el index donde se encuentra la particion y agregar las otras 2.
	 * supongo que tenemos que aplicar algo de semaforos para que ese index no vaya cambiando
	 * en este instante a medida que se creen particiones
	 *
	 *por el momento lo paso como parametro pero hay que ver como conseguirlo
	*/

	list_remove(tabla_particiones, indiceParticion);
	list_add_in_index(tabla_particiones, indiceParticion, primeraParticion);
	list_add_in_index(tabla_particiones, indiceParticion + 1, segundaParticion);

	return;

};


void pruebaLista(){

	/*
	 * el objetivo de esta prueba es para que quede claro como removiendo un item de una lista
	 * el puntero elements_count se actualiza al instante
	 *
	 * Este caso nos viene bien para sacar un partición de la lista y dividirla en otras 2
	 *
	 * Por ejemplo: Entra un mensaje que ocupa 8 bytes.
	 * Por medio del algoritmo First fit se inserta en una de 10 bytes
	 * Esto va a generar una partición nueva de 8 bytes para el dato y una de 2 bytes restantes
	 * Estos 2 bytes podrían terminar siendo fragmentación interna por ejemplo si el tamaño min de particion es de 3 bytes.
	*/
	printf("PRUEBA LISTAS -> \n");
	t_list* lsprueba = list_create();
	list_add(lsprueba, 125);//0
	list_add(lsprueba, 198);//1
	list_add(lsprueba, 3553);//2
	list_add(lsprueba, 4128);//3

	int item2 = list_get(lsprueba, 2);
	printf("item en la pos 2: %d\n", item2);

	int i;
	for(i=0;i < lsprueba->elements_count;i++){
		printf("item en pos %d: %d\n",i, list_get(lsprueba, i));
	}


	item2 = list_remove(lsprueba, 2);
	printf("saque el elemento: %d\n", item2);

	for(i=0;i < lsprueba->elements_count;i++){
		printf("item en pos %d: %d\n",i, list_get(lsprueba, i));
	}

	list_add_in_index(lsprueba, 2, 3000);
	list_add_in_index(lsprueba, 3, 553);

	printf("agregue el elemento: 3000 en la pos 2\n");
	printf("agregue el elemento: 553 en la pos 3\n");

	for(i=0;i < lsprueba->elements_count;i++){
		printf("item en pos %d: %d\n",i, list_get(lsprueba, i));
	}

}

bool particionCandidataVictima(t_particion* particion){
	return particion->ocupada;
}

t_particion * seleccionarVictimaFIFO(){
	int i;
	t_particion* particionAEliminar = malloc(sizeof(t_particion));

	bool _particionCandidataVictima(void* element){
			return particionCandidataVictima((t_particion*)element);
		}

	t_list* particionesCandidatas = list_filter(tabla_particiones, _particionCandidataVictima);

	particionAEliminar = list_get(particionesCandidatas, 0); //agarro la primera

	for(i = 1; i < particionesCandidatas->elements_count; i++){
			t_particion* particionActual = list_get(particionesCandidatas, i);

			if(particionAEliminar->indiceParticion > particionActual->indiceParticion){
				particionAEliminar = particionActual;
			}
	}

	return particionAEliminar;
}

t_particion * seleccionarVictimaLRU(){
	int i;
	t_particion* particionAEliminar = malloc(sizeof(t_particion));

	bool _particionCandidataVictima(void* element){
			return particionCandidataVictima((t_particion*)element);
		}

	t_list* particionesCandidatas = list_filter(tabla_particiones, _particionCandidataVictima);

	particionAEliminar = list_get(particionesCandidatas, 0); //agarro la primera

	for(i = 1; i < particionesCandidatas->elements_count; i++){
			t_particion* particionActual = list_get(particionesCandidatas, i);

			if(particionAEliminar->indiceParticion < particionActual->indiceParticion){
				particionAEliminar = particionActual;
			}
	}

	return particionAEliminar;
}


