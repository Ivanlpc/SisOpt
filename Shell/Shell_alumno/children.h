/*--------------------------------------------------------
UNIX Shell Project
function prototypes, macros and type declarations for job_control module

Sistemas Operativos
Grados I. Informatica, Computadores & Software
Dept. Arquitectura de Computadores - UMA


--------------------------------------------------------*/

#ifndef _CHILDREN_H
#define _CHILDREN_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>


// -----------------------------------------------------------------------
//      PRIVATE FUNCTIONS: BETTER USED THROUGH MACROS BELOW
// -----------------------------------------------------------------------

void print_children();


// -----------------------------------------------------------------------
//      PUBLIC MACROS
// -----------------------------------------------------------------------


#define print_children_list() 	 print_children()


// -----------------------------------------------------------------------
#endif

