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
				printf("Particion %d: \n Posicion inicial: %d \n Posicion final: %d \n Size: %d \n "
						"id de particion: %f \n Rama buddy: %d \n Posicion en la lista: %d \n", i,particion->posicion_inicial,
						particion->posicion_final, particion->size, particion->id, particion->ramaBuddy, obtenerPosicion(particion));
				printf("-------barra separadora-------\n");
				}
		}
}

void pruebaParticionesBuddy(){
	t_particion * particionPrueba;
	pruebaMostrarEstadoMemoria();

	//GENERAMOS PRIMER PARTICION
	particionPrueba = list_get(tabla_particiones, 0);
	generarParticionBS(particionPrueba);
	pruebaMostrarEstadoMemoria();

	//GENERAMOS SEGUNDA PARTICION DE LA RAMA IZQUIERDA
	particionPrueba = list_get(tabla_particiones, 0);
	generarParticionBS(particionPrueba);
	pruebaMostrarEstadoMemoria();

	//GENERAMOS TERCER PARTICION
	particionPrueba = list_get(tabla_particiones, 0);
	generarParticionBS(particionPrueba);
	pruebaMostrarEstadoMemoria();

	printf("Termine la prueba");

}

t_particion * pruebaBuscarParticionDeSizeMsgLuegoDe6Particiones(int sizeMsg){
	//GENERAMOS SEIS PARTICIONES
	t_particion * particionPrueba = list_get(tabla_particiones, 0);
	generarParticionBS(particionPrueba);
	particionPrueba = list_get(tabla_particiones, 0);
	generarParticionBS(particionPrueba);
	particionPrueba = list_get(tabla_particiones, 0);
	generarParticionBS(particionPrueba);
	//Estado que deberia haber en este momento: 256  256  512  1024

	particionPrueba = list_get(tabla_particiones, 2);
	generarParticionBS(particionPrueba);
	//Estado que deberia haber en este momento: 256  256  256  256 1024

	particionPrueba = list_get(tabla_particiones, 4);
	generarParticionBS(particionPrueba);
	//Estado que deberia haber en este momento: 256  256  256  256  512  512

	particionPrueba = list_get(tabla_particiones, 4);
	generarParticionBS(particionPrueba);
	//Estado que deberia haber en este momento: 256  256  256  256  256  256  512

	pruebaMostrarEstadoMemoria();


	//CALCULAMOS TAMANIO MINIMO POTENCIA DE DOS Y BUSCAMOS EN LA LISTA
	int tamanio = tamanioMinimo(sizeMsg);
	int i;
	for(i=0; i<tabla_particiones->elements_count; i++){
		particionPrueba = list_get(tabla_particiones, i);
		if(particionPrueba->size == tamanio){
			return particionPrueba;
		}
	}
	printf("No encontre una particion de ese tamaño \n");
	return 0;
}

void pruebaConsolidarParticionesBuddy(){
	//int buddyLoco;

	//buddyLoco = particion0->id ^particion1->id;
	//printf("El buddyLoco entre idParticion: %d y el idParticion: %d, es: %d \n", particion0->id, particion1->id, buddyLoco);

	/*buddyLoco = particion0->id ^particion2->id;
	printf("El buddyLoco entre idParticion: %d y el idParticion: %d, es: %d \n", particion0->id, particion2->id, buddyLoco);
	buddyLoco = particion0->id ^particion1->id;
	printf("El buddyLoco entre idParticion: %d y el idParticion: %d, es: %d \n", particion0->id, particion1->id, buddyLoco);
	buddyLoco = particion2->id ^particion1->id;
	printf("El buddyLoco entre idParticion: %d y el idParticion: %d, es: %d \n", particion2->id, particion1->id, buddyLoco);
	*/

}
