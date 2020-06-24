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

int obtenerPosicion(t_particion * particion){
	int i;
	for(i=0; tabla_particiones->elements_count-1; i++){
		t_particion*particionAMirar = list_get(tabla_particiones, i);
		if(particion->indiceParticion == particionAMirar->indiceParticion){
			return i;
		}
	}
	printf("No encontre el elemento de id %d", particion->indiceParticion);
	return -1;
}

t_particion * consolidarParticion(t_particion * particion){
	t_particion* particionAMirar = malloc(sizeof(t_particion));

	int posicionParticion = obtenerPosicion(particion);
	if(posicionParticion == -1){
		printf("obtenerPosicion devolvio -1 cuando se quiso consolidar");
		EXIT_FAILURE;
	}

	if(posicionParticion !=0){//si tiene una particion antes
		particionAMirar = list_get(tabla_particiones, posicionParticion-1);
		if(!particionAMirar->ocupada){//si la particion anterior esta libre
			particion->posicion_inicial = particionAMirar->posicion_inicial;
			particion->size += particionAMirar->size;
		}
	}
	if(posicionParticion != (tabla_particiones->elements_count)-1){//si tiene una particion despues
		particionAMirar = list_get(tabla_particiones, posicionParticion+1);
		if(!particionAMirar->ocupada){//si la particion siguiente esta libre
			particion->posicion_final = particionAMirar->posicion_final;
			particion->size += particionAMirar->size;
		}
	}
	return particion;
}

int tamanioMinimo(int tamanioSolicitadoEnBytes){
///algoritmo para calcular la menor potencia de 2 en la que entra el proceso; Buddy System

	if(tamanioSolicitadoEnBytes !=1){
		int menorPotenciaDeDos=2;
		while(menorPotenciaDeDos < tamanioSolicitadoEnBytes){
			menorPotenciaDeDos = menorPotenciaDeDos*2;
		}
		return menorPotenciaDeDos;
	} else return 1;
}



















