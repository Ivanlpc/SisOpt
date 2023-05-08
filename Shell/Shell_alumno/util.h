/**
 * Nombre: Iván López Cervantes
 */

#ifndef _UTIL_H
#define _UTIL_H

#include <stdio.h>
#include <string.h>
void parse_redirections(char **args, char **file_in, char **file_out);
void copy_string_array(char *dst[], char *src[]);

#define copy_args(dst, src) copy_string_array(dst, src); // Copy a string array into another string array

#endif