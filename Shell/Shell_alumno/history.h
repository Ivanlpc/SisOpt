/**
 * Nombre: Iván López Cervantes
 */

#ifndef _HISTORY_H
#define _HISTORY_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE 256

// -----------------------------------------------------------------------
//      PRIVATE FUNCTIONS: BETTER USED THROUGH MACROS BELOW
// -----------------------------------------------------------------------
typedef struct command_ {
	char *args; /* comando con sus argumentos*/
    int length; /* Longitud del comando */
    struct command_ *next; /* Siguiente comando en el historial*/
}command;

command *new_command(char args[], int length);
void read_input(char inputBuffer[], int size, int * length, command* list);
void add_command(command *list, command *item);
void print_command_list(command *list);
void print_command_item(command *item, int n);
command *get_command_bypos(command *list, int n);
#define new_history_list() new_command(NULL, 0) // name must be const char *
// -----------------------------------------------------------------------
//      PUBLIC MACROS
// -----------------------------------------------------------------------
#define print_history(list) print_command_list(list);

#endif
