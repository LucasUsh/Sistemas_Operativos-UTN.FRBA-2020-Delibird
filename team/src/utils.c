#include "utils.h"

char** get_array_by_index(char** array_from_config, int index){

	 /* EJEMPLO CON POKEMONES
	  * array_from_config:
	  *  0 -> Pikachu|Squirtle|Pidgey
	  *  1 -> Squirtle|Charmander
	  *  2 -> Bulbasaur
	  */

	//value_string = Pikachu|Squirtle|Pidgey
	char* value_string = array_from_config[index];
	/*
	  * value_array:
	  *  0 -> Pikachu
	  *  1 -> Squirtle
	  *  2 -> Pidgey
	  */
	char** value_array = string_split(value_string, "|");

	return value_array;
}


int array_length(char** value_array){
	int i = 0;

	while(value_array[i] != NULL){
		i++;
	}

	return i;
}

t_config* get_config(char* name){
	char* cfg_path = string_new();
	string_append(&cfg_path, "../config/");
	string_append(&cfg_path, name);

	return config_create(cfg_path);
}
