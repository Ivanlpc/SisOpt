/**
 * Nombre: Iván López Cervantes
 */

#include <stdio.h>
#include <string.h>
#define MAX_LINE 256

void parse_redirections(char **args, char **file_in, char **file_out)
{
	*file_in = NULL;
	*file_out = NULL;
	char **args_start = args;
	while (*args)
	{
		int is_in = !strcmp(*args, "<");
		int is_out = !strcmp(*args, ">");
		if (is_in || is_out)
		{
			args++;
			if (*args)
			{
				if (is_in)
					*file_in = *args;
				if (is_out)
					*file_out = *args;
				char **aux = args + 1;
				while (*aux)
				{
					*(aux - 2) = *aux;
					aux++;
				}
				*(aux - 2) = NULL;
				args--;
			}
			else
			{
				/* Syntax error */
				fprintf(stderr, "syntax error in redirection\n");
				args_start[0] = NULL; // Do nothing
			}
		}
		else
		{
			args++;
		}
	}
}
/**
 * Copia los elementos de una cadena de carácteres en un array fuente a otro array destino.
 *
 * @param dst el array de destino donde se copiarán los elementos de la cadena fuente
 * @param src el array fuente del que se copiarán los elementos
 *
 */

void copy_string_array(char *dst[], char *src[])
{
	/* Recorremos el array src mientras el valor no sea NULL y nos encontremos dentro del límite del Array */
	for (int i = 0; i < MAX_LINE / 2 && src[i] != NULL; i++)
	{
		/* Copiamos cada cadena de carácteres (String) al nuevo array */
		dst[i] = strdup(src[i]);
	}
}
