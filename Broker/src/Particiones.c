/*
 * Particiones.c
 *
 *  Created on: 22 jun. 2020
 *      Author: utnso
 */

#include "Particiones.h"

#include<commons/string.h>

t_particion* crearParticion(int inicio, int size, bool ocupada, int ramaBuddy){
	t_particion* newParticion = malloc(sizeof(t_particion));
	newParticion->ocupada = false;
	newParticion->posicion_inicial = inicio;
	newParticion->posicion_final = inicio+size-1; //para contemplar el 0. Ej: si es de 128bytes arranca en 0 y va a 127
	newParticion->size = size;
	newParticion->id = get_id();
	newParticion->ramaBuddy = ramaBuddy;
	newParticion->codigo_operacion = NEW_POKEMON;
	//newParticion->id_Mensaje = 0;

	return newParticion;
}

t_particion generarParticionDinamicamente(int32_t sizeMensaje, int32_t sizeMinParticion){
	// Se genera una partición del tamanio del Mensaje
	t_particion particionNueva;

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

bool hayParticionesCandidatas(int32_t sizeMsg){
	bool _particionCandidata(void* element){
		return particionCandidata((t_particion*)element, sizeMsg);
	}
	t_list* particionesCandidatas = list_filter(tabla_particiones, _particionCandidata);
	return particionesCandidatas->elements_count != 0;
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


	t_particion* primeraParticion = crearParticion(particionOriginal->posicion_inicial, sizeMsg, false, 0);

	int inicioSegundaParticion = primeraParticion->posicion_final + 1;

	int sizeSegundaParticion = particionOriginal->size - primeraParticion->size;

	t_particion* segundaParticion = crearParticion(inicioSegundaParticion,
			inicioSegundaParticion + sizeSegundaParticion, false, 0);

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

void liberar(char algoritmoReemplazo){
	switch(algoritmoReemplazo){
	case FIFO: //(strcmp(algoritmoReemplazo, "FIFO") == 0):
		algoritmoFIFO();
		break;
	case LRU: //(strcmp(algoritmoReemplazo, "LRU") == 0):
		algoritmoLRU();
		break;
	default:
		printf("Ese algoritmo no esta implementado\n");
		return;
	}
}

void algoritmoFIFO(){ //debemos tomar la de id mas chico
	int i;
	t_particion* particionAEliminar;// = malloc(sizeof(t_particion));

	bool _particionCandidataVictima(void* element){
		return particionCandidataVictima((t_particion*)element);
	}

	t_list* particionesCandidatas = list_filter(tabla_particiones, _particionCandidataVictima);

	particionAEliminar = list_get(particionesCandidatas, 0); //agarro la primera
	for(i = 1; i < particionesCandidatas->elements_count; i++){ //comparo con la que sigue
		t_particion* particionActual = list_get(particionesCandidatas, i);
		if(particionAEliminar->id > particionActual->id){
			particionAEliminar = particionActual;
		}
	}
	particionAEliminar->ocupada = false;
	particionAEliminar->codigo_operacion =0;
	particionAEliminar->id=0;
	int posicion = obtenerPosicion(particionAEliminar);
	list_remove(tabla_particiones, posicion);
	list_add_in_index(tabla_particiones,posicion, particionAEliminar);
	free(particionAEliminar);
	//consolidar
}

void algoritmoLRU(){ //debemos tomar la de id mas grande
	int i;
	t_particion* particionAEliminar;// = malloc(sizeof(t_particion));

	bool _particionCandidataVictima(void* element){
		return particionCandidataVictima((t_particion*)element);
	}

	t_list* particionesCandidatas = list_filter(tabla_particiones, _particionCandidataVictima);

	particionAEliminar = list_get(particionesCandidatas, 0); //agarro la primera
	for(i = 1; i < particionesCandidatas->elements_count; i++){ //comparo con la que sigue
		t_particion* particionActual = list_get(particionesCandidatas, i);
		if(particionAEliminar->id < particionActual->id){
			particionAEliminar = particionActual;
		}
	}
	particionAEliminar->ocupada = false;
	particionAEliminar->codigo_operacion =0;
	particionAEliminar->id=0;
	int posicion = obtenerPosicion(particionAEliminar);
	list_remove(tabla_particiones, posicion);
	list_add_in_index(tabla_particiones,posicion, particionAEliminar);
	free(particionAEliminar);
	//consolidar
}

int obtenerPosicion(t_particion * particion){
	int i;
	for(i=0; i < tabla_particiones->elements_count; i++){
		t_particion*particionAMirar = list_get(tabla_particiones, i);
		if(particion->id == particionAMirar->id){
			return i;
		}
	}
	printf("No encontre el elemento de id %d \n", particion->id);
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

int32_t algoritmoCompactacion(int32_t frecuenciaCompactacion){
	if(frecuenciaCompactacion!=-1){
		//compactar
		frecuenciaCompactacion=frecuenciaCompactacion-1;
		return frecuenciaCompactacion;
	}else return frecuenciaCompactacion;
}

int32_t tamanioMinimo(int32_t sizeMsg){
///algoritmo para calcular la menor potencia de 2 en la que entra un mensaje; Buddy System

	if(sizeMsg !=1){
		int32_t menorPotenciaDeDos=2;
		while(menorPotenciaDeDos < sizeMsg){
			menorPotenciaDeDos = menorPotenciaDeDos*2;
		}
		return menorPotenciaDeDos;
	} else return 1;
}

void generarParticionBS(t_particion* particionInicial){
	int halfSize= particionInicial->size * 0.5;

	int inicioIzquierda = particionInicial->posicion_inicial;
	int inicioDerecha = particionInicial->posicion_inicial+((particionInicial->size)*0.5);

	t_particion *particionIzquierda = crearParticion(inicioIzquierda, halfSize, false, 1);
	sleep(0.25);
	t_particion *particionDerecha = crearParticion(inicioDerecha, halfSize, false, 2);

	int posicion = obtenerPosicion(particionInicial);
	list_remove(tabla_particiones, posicion);
	list_add_in_index(tabla_particiones,posicion, particionDerecha);
	list_add_in_index(tabla_particiones,posicion, particionIzquierda);
	free(particionInicial);
}

// En Buddy System una particion es candidata si su tamaño es el de la menor potencia de dos
bool particionCandidataBS(t_particion* particion, int32_t tamanioMinimo){
	return !particion->ocupada && tamanioMinimo <= particion->size;
}

bool hayParticionesCandidatasBS(int32_t sizeMsg){
	bool _particionCandidataBS(void* element){
		return particionCandidataBS((t_particion*)element, sizeMsg);
	}
	t_list* particionesCandidatas = list_filter(tabla_particiones, _particionCandidataBS);
	return particionesCandidatas->elements_count != 0;
}

t_particion* getParticionBS(int32_t tamanioMinimo){
	bool _particionCandidataBS(void* element){
		return particionCandidata((t_particion*)element, tamanioMinimo);
	}
	t_list* particionesCandidatas = list_filter(tabla_particiones, _particionCandidataBS);
	return list_get(particionesCandidatas, 0);
}


int algoritmoParticion(char algoritmoMemoria, info_mensaje mensaje, int32_t frecuenciaCompactacion, char algoritmoReemplazo, char algoritmoParticionLibre){
	int i;
	for(i=0; i<tabla_particiones->elements_count; i++){
		switch(algoritmoMemoria){
		case BS:
			algoritmoBuddySystem(mensaje, frecuenciaCompactacion, algoritmoReemplazo);
			break;
		case PARTICIONES:
			algoritmoParticionDinamica(mensaje, frecuenciaCompactacion, algoritmoReemplazo, algoritmoParticionLibre);
			break;
			}
		}
	return 0;
}

void algoritmoBuddySystem(info_mensaje mensaje, int32_t frecuenciaCompactacion, char algoritmoReemplazo){
	int32_t tamanio= tamanioMinimo(mensaje.sizeMsg);
	if(hayParticionesCandidatasBS(tamanio) == true){ //hay una particion libre del tamanio exacto que necesito?
		t_particion * particion = getParticionBS(tamanio);
		//guardo el mensaje
	}else {if(hayParticionesCandidatas(tamanio) == true){ //hay una particion libre que pueda truncar?
		while(hayParticionesCandidatasBS(tamanio) != true){
			t_particion * particion = getParticionFirstFit(tamanio);
			generarParticionBS(particion);
			}
		//guardo el mensaje
		}else algoritmoLiberacion(frecuenciaCompactacion, algoritmoReemplazo);
	}
}

void algoritmoParticionDinamica(info_mensaje mensaje, int32_t frecuenciaCompactacion, char algoritmoReemplazo, char algoritmoParticionLibre){
	int32_t tamanio= mensaje.sizeMsg;
	t_particion * particion;
	if(hayParticionesCandidatas(tamanio) == true){ //hay una particion libre que pueda truncar?
		switch(algoritmoParticionLibre){
		case FF:
			particion = getParticionFirstFit(tamanio);
			//genero particion
			//guardo el mensaje
			break;
		case BF:
			particion = getParticionBestFit(tamanio);
			//genero particion
			//guardo el mensaje
			break;
		default:
			printf("Ese algoritmo no esta implementado\n");
			return;
		}
		}else algoritmoLiberacion(frecuenciaCompactacion, algoritmoReemplazo);
}

void algoritmoLiberacion(int32_t frecuenciaCompactacion, char algoritmoReemplazo){
	int liberadas;
	switch(frecuenciaCompactacion){
	case -1:
		liberar(algoritmoReemplazo);
		break;
	case 0 || 1:
		liberar(algoritmoReemplazo);
		//compactar
		break;
	default:
		for(liberadas=0; liberadas<frecuenciaCompactacion;liberadas++){
			liberar(algoritmoReemplazo);
			}
		//compactar
		return;
		}
}








