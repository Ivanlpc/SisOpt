/**
 * Nombre: Iván López Cervantes
 */

#ifndef _CHILDREN_H
#define _CHILDREN_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>

// -----------------------------------------------------------------------
//      PRIVATE FUNCTIONS: BETTER USED THROUGH MACROS BELOW
// -----------------------------------------------------------------------

void print_children();

// -----------------------------------------------------------------------
//      PUBLIC MACROS
// -----------------------------------------------------------------------

#define print_children_list() print_children()

// -----------------------------------------------------------------------
#endif
