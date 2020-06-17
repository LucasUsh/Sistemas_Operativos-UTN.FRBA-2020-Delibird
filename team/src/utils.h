/*
 * utils.h
 *
 *  Created on: 4 jun. 2020
 *      Author: utnso
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/config.h>

char** get_array_by_index(char** array_from_config, int32_t index);
int32_t array_length(char** value_array);
t_config* get_config(char* name);

#endif /* UTILS_H_ */
