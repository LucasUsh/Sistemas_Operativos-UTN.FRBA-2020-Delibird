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
		for(i=0; i <= tabla_particiones->elements_count -1; i++){
				t_particion * particion = list_get(tabla_particiones, i);
				printf("Particion %d: \n Posicion inicial: %d \n Posicion final: %d \n Size: %d \n Indice de particion: %d \n", i,particion->posicion_inicial, particion->posicion_final, particion->size, particion->id);
				printf("-------barra separadora-------\n");
				}
		}
}

	/*switch(tabla_particiones->elements_count){
	case 0:
		printf("No hay particiones en la tabla de particiones\n");
		break;
	case 1:
		for(int i =0; tabla_particiones->elements_count; i++){//sacamos el -1
			t_particion * particion = list_get(tabla_particiones, i);
			printf("Particion %d: \n Posicion inicial: %d \n Posicion final: %d \n Size: %d \n", i,particion->posicion_inicial, particion->posicion_final, particion->size);
			printf("-------barra separadora-------\n");
		}
		break;
	default:
		for(int i =0; tabla_particiones->elements_count-1; i++){
			t_particion * particion = list_get(tabla_particiones, i);
			printf("Particion %d: \n Posicion inicial: %d \n Posicion final: %d \n Size: %d \n", i,particion->posicion_inicial, particion->posicion_final, particion->size);
			printf("-------barra separadora-------\n");
		}
		break;
	}*/

void pruebaParticionesBuddy(t_config* config){
	int id = 0;
	int buddyLoco;
	pruebaMostrarEstadoMemoria();

	// INICIALIZAMOS
	inicializarMemoriaBS(config);
	id+=1;
	t_particion * particion0 = malloc(sizeof(t_particion));
	particion0 = list_get(tabla_particiones, 0);
	particion0->id = id;
	pruebaMostrarEstadoMemoria();

	//GENERAMOS PRIMER PARTICION
	t_particion * particion1 = malloc(sizeof(t_particion));
	particion1 = generarParticionBS(particion0);
	id+=1;
	particion0->id=id;
	id+=1;
	particion1->id=id;
	list_add(tabla_particiones, particion1);
	pruebaMostrarEstadoMemoria();

	buddyLoco = particion0->id ^particion1->id;
	printf("El buddyLoco entre idParticion: %d y el idParticion: %d, es: %d \n", particion0->id, particion1->id, buddyLoco);

	//GENERAMOS SEGUNDA PARTICION
	particion0 = list_get(tabla_particiones, 0);
	t_particion * particion2 = malloc(sizeof(t_particion));
	particion2 = generarParticionBS(particion0);
	id+=1;
	particion0->id=id;
	id+=1;
	particion2->id=id;
	list_add(tabla_particiones, particion2);
	pruebaMostrarEstadoMemoria();

	buddyLoco = particion0->id ^particion2->id;
	printf("El buddyLoco entre idParticion: %d y el idParticion: %d, es: %d \n", particion0->id, particion2->id, buddyLoco);
	buddyLoco = particion0->id ^particion1->id;
	printf("El buddyLoco entre idParticion: %d y el idParticion: %d, es: %d \n", particion0->id, particion1->id, buddyLoco);
	buddyLoco = particion2->id ^particion1->id;
	printf("El buddyLoco entre idParticion: %d y el idParticion: %d, es: %d \n", particion2->id, particion1->id, buddyLoco);

	//GENERAMOS TERCER PARTICION
	particion1 = list_get(tabla_particiones, 1);
	t_particion * particion3 = malloc(sizeof(t_particion));
	particion3 = generarParticionBS(particion1);
	id+=1;
	particion1->id=id;
	id+=1;
	particion3->id=id;
	list_add(tabla_particiones, particion3);
	pruebaMostrarEstadoMemoria();// mostramos estado al particionar la particion de la posicion 1

	printf("Termine la prueba");
	free(particion0);
	free(particion1);
	free(particion2);
	free(particion3);

}
