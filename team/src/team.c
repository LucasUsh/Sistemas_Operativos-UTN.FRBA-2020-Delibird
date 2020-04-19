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

int main(void) {
	char* string = string_new();
	string_append(&string, "!!!Proceso Team!!!");
	puts(string); /* prints !!!Hello World!!! */
	return EXIT_SUCCESS;
}
