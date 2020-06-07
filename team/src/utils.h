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

char** get_array_by_index(char** array_from_config, int index);
int array_length(char** value_array);

#endif /* UTILS_H_ */
