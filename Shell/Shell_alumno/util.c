/**
 * Nombre: Iván López Cervantes
 */
#include "util.h"
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
void delete_string_array(char * arr []) {
	for (int i = 0; i < MAX_LINE / 2 && arr[i] != NULL; i++)
	{
		free(arr[i]);
	}
}


void print_jobs_with_status(char status) {
    DIR *d; 
    struct dirent *dir;
    char buff[2048];
    d = opendir("/proc");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            sprintf(buff, "/proc/%s/stat", dir->d_name); 
            FILE *fd = fopen(buff, "r");
            if (fd){
                long pid;     // pid
                long ppid;    // ppid
                char state;   // estado: R (runnable), S (sleeping), T(stopped), Z (zombie)

                // La siguiente línea lee pid, state y ppid de /proc/<pid>/stat
                fscanf(fd, "%ld %s %c %ld", &pid, buff, &state, &ppid);
				if(state == status) {
					printf("%ld\n", pid);
				}
                fclose(fd);
            }
        }
        closedir(d);
    }
}
