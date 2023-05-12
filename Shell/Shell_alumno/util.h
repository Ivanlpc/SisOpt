/**
 * Nombre: Iván López Cervantes
 */

#ifndef _UTIL_H
#define _UTIL_H

#include <stdio.h>
#include <dirent.h>
#include <string.h>
void parse_redirections(char **args, char **file_in, char **file_out);
void copy_string_array(char *dst[], char *src[]);
void print_jobs_with_status(char state);

#define copy_args(dst, src) copy_string_array(dst, src); // Copiamos el array de strings
#define print_zombies() print_jobs_with_status('Z');

#endif