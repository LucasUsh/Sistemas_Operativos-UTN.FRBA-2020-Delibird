/*
 * pruebas.c
 *
 *  Created on: 24 jun. 2020
 *      Author: utnso
 */

#include <commons/collections/list.h>
#include "pruebas.h"

/*
 * No me preguntes por qué pero si el archivo se llama pruebas.c y lo incluímos en Broker se rompe to2
 */

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

void pruebaTamanioMinimo(){
	t_list* listaTamanios = list_create();
	list_add(listaTamanios, 1); //debe asignar 1
	list_add(listaTamanios, 2); //debe asignar 2
	list_add(listaTamanios, 3), list_add(listaTamanios, 4);//debe asignar 4
	list_add(listaTamanios, 5),	list_add(listaTamanios, 7),	list_add(listaTamanios, 8);//debe asignar 8
	list_add(listaTamanios, 9), list_add(listaTamanios, 12), list_add(listaTamanios, 16);//debe asignar 16
	list_add(listaTamanios, 17), list_add(listaTamanios, 25), list_add(listaTamanios, 32);//debe asignar 32
	list_add(listaTamanios, 33), list_add(listaTamanios, 48), list_add(listaTamanios, 64);//debe asignar 64
	list_add(listaTamanios, 65), list_add(listaTamanios, 102), list_add(listaTamanios, 128);//debe asignar 128
	list_add(listaTamanios, 129), list_add(listaTamanios, 200), list_add(listaTamanios, 256);//debe asignar 256
	list_add(listaTamanios, 257), list_add(listaTamanios, 401), list_add(listaTamanios, 512);//debe asignar 512
	list_add(listaTamanios, 513), list_add(listaTamanios, 999), list_add(listaTamanios, 1024);//debe asignar 1024
	list_add(listaTamanios, 1025), list_add(listaTamanios, 1777), list_add(listaTamanios, 2048);//debe asignar 2048
	list_add(listaTamanios, 2049), list_add(listaTamanios, 3010), list_add(listaTamanios, 4096);//debe asignar 4096
	list_add(listaTamanios, 4097), list_add(listaTamanios, 6677), list_add(listaTamanios, 8192);//debe asignar 8192
	list_add(listaTamanios, 8193), list_add(listaTamanios, 12222), list_add(listaTamanios, 16384);//debe asignar 16384
	list_add(listaTamanios, 16385), list_add(listaTamanios, 26061), list_add(listaTamanios, 32768);//debe asignar 32768

	int i;
	for(i=0; i < listaTamanios->elements_count; i++){
		int tamanioPedido= list_get(listaTamanios, i);
		printf("En la iteracion n: %d me pediste %d y te asigne %d \n", i ,tamanioPedido, tamanioMinimo(tamanioPedido));
	}
}

void pruebaMostrarEstadoMemoria(){
	printf("----->ESTADO DE LA MEMORIA\n");
	printf("Hay %d particiones en este momento\n", tabla_particiones->elements_count);
	if(tabla_particiones->elements_count == 0){
		printf("No hay particiones en la tabla de particiones\n");
	}else {
		int i;
		for(i=0; i < tabla_particiones->elements_count; i++){
				t_particion * particion = list_get(tabla_particiones, i);
				printf("Index lista: %d \n Posicion inicial: %d \n Posicion final: %d \n Size: %d \n "
						"id de particion: %f \n Rama buddy: %d \n \n", i,particion->posicion_inicial,
						particion->posicion_final, particion->size, particion->id, particion->ramaBuddy);
				}
		}
}

void pruebaParticionesBuddy(){
	t_particion * particionPrueba;

	//GENERAMOS PRIMER BUDDY
	particionPrueba = list_get(tabla_particiones, 0);
	generarParticionBS(particionPrueba);

	//GENERAMOS CUATRO PARTICIONES A IZQUIERDA
	particionPrueba = list_get(tabla_particiones, 0);
	generarParticionBS(particionPrueba);
	particionPrueba = list_get(tabla_particiones, 0);
	generarParticionBS(particionPrueba);
	particionPrueba = list_get(tabla_particiones, 0);
	generarParticionBS(particionPrueba);
	particionPrueba = list_get(tabla_particiones, 0);
	generarParticionBS(particionPrueba);
	pruebaMostrarEstadoMemoria();

	//GENERAMOS CUATRO PARTICIONES A DERECHA
	particionPrueba = list_get(tabla_particiones, 5);
	generarParticionBS(particionPrueba);
	particionPrueba = list_get(tabla_particiones, 6);
	generarParticionBS(particionPrueba);
	particionPrueba = list_get(tabla_particiones, 7);
	generarParticionBS(particionPrueba);
	particionPrueba = list_get(tabla_particiones, 8);
	generarParticionBS(particionPrueba);

	pruebaMostrarEstadoMemoria();
	printf("Termine la prueba");
}

void pruebaBuscarParticionDeSizeMsg(int sizeMsg){
	//Creamos varias particiones (las mismas que pruebaParticionesBuddy)
	t_particion * particionPrueba;
	particionPrueba = list_get(tabla_particiones, 0);
	generarParticionBS(particionPrueba);
	particionPrueba = list_get(tabla_particiones, 0);
	generarParticionBS(particionPrueba);
	particionPrueba = list_get(tabla_particiones, 0);
	generarParticionBS(particionPrueba);
	particionPrueba = list_get(tabla_particiones, 0);
	generarParticionBS(particionPrueba);
	particionPrueba = list_get(tabla_particiones, 0);
	generarParticionBS(particionPrueba);
	particionPrueba = list_get(tabla_particiones, 5);
	generarParticionBS(particionPrueba);
	particionPrueba = list_get(tabla_particiones, 6);
	generarParticionBS(particionPrueba);
	particionPrueba = list_get(tabla_particiones, 7);
	generarParticionBS(particionPrueba);
	particionPrueba = list_get(tabla_particiones, 8);
	generarParticionBS(particionPrueba);
	pruebaMostrarEstadoMemoria(); //mostramos el estado de la memoria

	//Calculamos la menor potencia de 2 en la que entra el mensaje y buscamos particiones candidatas
	int tamanio = tamanioMinimo(sizeMsg);
	t_particion * particionCandidata = getParticionBS(tamanio);

	printf("La particion candidata es: \n");
	printf("Index lista: %d \n Posicion inicial: %d \n Posicion final: %d \n Size: %d \n id de particion: %f \n "
			"Rama buddy: %d \n \n", obtenerPosicion(particionCandidata), particionCandidata->posicion_inicial,
			particionCandidata->posicion_final, particionCandidata->size, particionCandidata->id,
			particionCandidata->ramaBuddy);
}

void pruebaEncontrarBuddyTrasUnaParticion(){ // Debe imprimir buddyLoco = 1
	bool buddyLoco;
	t_particion * particion0;
	t_particion * particion1;

	//Creamos el primer par de buddies
	particion0 = list_get(tabla_particiones, 0);
	generarParticionBS(particion0);
	pruebaMostrarEstadoMemoria(); //mostramos el estado de la memoria

	particion0 = list_get(tabla_particiones, 0);
	particion1 = list_get(tabla_particiones, 1);
	buddyLoco = particion0->posicion_inicial == particion1->posicion_inicial ^ 1024;
	printf("El buddyLoco entre idParticion: %f y el idParticion: %f, es: %d \n", particion0->id, particion1->id, buddyLoco);
}

void * pruebaEncontrarBuddyTrasDosParticiones(){
	bool buddyLoco;
	t_particion * particion0;
	t_particion * particion1;
	t_particion * particion2;

	//Creamos el primer par de buddies y al de la izquierda lo volvemos a partir
	particion0 = list_get(tabla_particiones, 0);
	generarParticionBS(particion0);
	particion0 = list_get(tabla_particiones, 0);
	generarParticionBS(particion0);
	pruebaMostrarEstadoMemoria(); //mostramos el estado de la memoria, deberia ser: 512  512  1024

	//Tomamos los de las posiciones 0 y 1: 512 y 512 --> deberian ser buddies
	particion0 = list_get(tabla_particiones, 0);
	particion1 = list_get(tabla_particiones, 1);

	if(particion0->size==particion1->size){// Comparamos solo si tienen el mismo size
		buddyLoco = particion0->posicion_inicial == (particion1->posicion_inicial^particion1->size);
		if(buddyLoco == 0){
			printf("La particion de id: %f es buddy de la particion de id: %f \n", particion0->id, particion1->id);
		}else printf("Las particiones de id: %f y %f no son buddies\n", particion0->id, particion1->id);
	}else printf("Las particiones de id: %f y %f no son buddies porque tienen distinto size\n", particion0->id, particion1->id);

	//Tomamos los de las posiciones 0 y 2: 512 y 1024 --> no deberian ser buddies
	particion0 = list_get(tabla_particiones, 0);
	particion2 = list_get(tabla_particiones, 2);
	if(particion0->size==particion2->size){// Comparamos solo si tienen el mismo size
		buddyLoco = particion0->posicion_inicial == (particion2->posicion_inicial^particion2->size);
		if(buddyLoco == 0){
			printf("La particion de id: %f es buddy de la particion de id: %f \n", particion0->id, particion2->id);
		}else printf("Las particiones de id: %f y %f no son buddies\n", particion0->id, particion2->id);
	}else printf("Las particiones de id: %f y %f no son buddies porque tienen distinto size\n", particion0->id, particion2->id);

	//Tomamos los de las posiciones 1 y 2: 512 y 1024 --> no deberian ser buddies
	particion1 = list_get(tabla_particiones, 1);
	particion2 = list_get(tabla_particiones, 2);
	if(particion1->size==particion2->size){// Comparamos solo si tienen el mismo size
		buddyLoco = particion1->posicion_inicial == (particion2->posicion_inicial^particion2->size);
		if(0==buddyLoco){
			printf("La particion de id: %f es buddy de la particion de id: %f \n", particion1->id, particion2->id);
		}else printf("Las particiones de id: %f y %f no son buddies\n", particion1->id, particion2->id);
	}else printf("Las particiones de id: %f y %f no son buddies porque tienen distinto size\n", particion1->id, particion2->id);

}
