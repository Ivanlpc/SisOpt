#include "history.h"
#include "job_control.h"
#include "util.h"
#define CLEAN_CURRENT_LINE() printf("\r\033[K");

char getch();
void read_input(char inputBuffer[], int size, int * length, command* list);
command *get_command_bypos(command *list, int n);
/**
 * Lee el comando introducido por el usuario y reconoce las pulsaciones de flechas para moverse por los comandos
 * del historial.
 *
 * @param inputBuffer Array donde guardará los carácteres introducidos por el usuario
 * @param size Tamaño máximo que tendrá inputBuffer
 * @param length Devuelve la longitud del comando introducido por el usuario
 * @param list Historial donde guardará y leerá los comandos anteriores
 */

void read_input(char inputBuffer[], int size, int * length, command* list) {
	/* Inicializamos la posición del cursor e historial a 0*/
	int pos = 0;
    int his_pos = 0;
	while(1) {
		/* Esperamos a obtener el primer carácter */
		int ch = getch();
		/* Si el carácter es Ctrl + D */
		if(ch == 4) {
			/* Establecemos la longitud a 0 para decirle a get_command que ha pulsado ctrl + D*/
			*length = 0;
			break;
		};
		/* Si es un salto de línea `Enter` */
		if(ch == '\n') {
			/* Añadimos el salto de línea al buffer para que get_command sepa cuándo terminar */
			inputBuffer[pos] = ch;
			/* Establecemos la longitud total del comando introducido */
            pos++;
			*length = pos;
			/* Terminamos el bucle para ir a get_command */
			break;

			/* Si es la tecla Escape*/
		} else if (ch == 27) {
			/* Obtenemos el siguiente carácter */
			ch = getch();
			/* Si es la tecla `[`*/
			if(ch == 91) {
				/* Obtenemos el siguiente carácter */
				ch = getch();
				/* Si es la flecha hacia arriba */
				if(ch == 65) {
					/* Obtenemos el siguiente comando del historial */
					command * last_command = get_command_bypos(list, his_pos + 1);
					/* Si existe el comando*/
					if(last_command != NULL) {
						/* Establecemos la nueva posición en el historial para indicar que nos encontramos ahí*/
						his_pos++;
						/* Copiamos al buffer el comando*/
						strcpy(inputBuffer, last_command -> args);
						/* Actualizamos la nueva longitud del comando en el buffer */
						*length = last_command->length;
						/* Actualizamos la posición del cursor en el buffer */
						pos = *length;
						/* Borramos la última línea impresa en la terminal */
						CLEAN_CURRENT_LINE()
						/* Imprimimos una nueva línea con el comando del historial*/
						printf("COMMAND->%s", last_command->args);
            			fflush(stdout);
					} 

				/* Si es la flecha hacia abajo*/
				}else if(ch == 66) {
					/* Si la posición es la primera en el historial, no hacemos nada*/
					if(his_pos == 0) continue;
					/* Recuperamos el siguiente comando del historial */
					command * last_command = get_command_bypos(list, his_pos - 1);
					/* Si existe */
					if(last_command != NULL) {
						/* Actualizamos la posición en la que nos encontramos en el historial */
						his_pos--;
						/* Copiamos al buffer el comando del historial */
						strcpy(inputBuffer, last_command -> args);
						/* Actualizamos la longitud del comando por la del historial */
						*length = last_command->length;
						/* Actualizamos la posición del cursor a la longitud del comando */
						pos = *length;
						/* Borramos la última línea que se ha impreso en la terminal */
						CLEAN_CURRENT_LINE()
						/* Imprimimos una nueva línea con el comando del historial */
						printf("COMMAND->%s", last_command->args);
            			fflush(stdout);

					/* Si no se encuentra el comando en el historial */
					} else {
						/* Actualizamos la posición del historial, cursor y longitud del comando a 0 */
						his_pos = 0;
						pos = 0;
						*length = 0;
						/* Borramos la última línea que se ha impreso en la terminal */
						CLEAN_CURRENT_LINE()
						/* Imprimimos un comando vacío */
						printf("COMMAND->");
						fflush(stdout);
					}
				}
				
			}
		/* Si el carácter es `BACKSPACE` (Retroceso)*/
		}else if(ch == 127 ) { 
			/* Si la posición del cursor es mayor que 0 indica que hay algo en el buffer de entrada*/
            if(pos > 0) { 
				/* Actualizamos la posición a una menos*/
                pos--; 
				/* Actualizamos la longitud del comando a una menos */
				*length = pos - 1;
				/* Establecemos en el buffer el carácter vacío */
                inputBuffer[pos] = '\0';
				/* Borramos la última línea impresa en la terminal */
                CLEAN_CURRENT_LINE() 
				/* Imprimimos en la terminal el buffer con la última posición borrada */
                printf("COMMAND->%s", inputBuffer); 
				fflush(stdout);
            }
		/* Para cualquier otro carácter introducido */
        } else { 
			/* Si queda espacio en el buffer */
			if(pos < size - 1){
				/* Añadimos el carácter al buffer */
				inputBuffer[pos] = ch; 
				/* Actualizamos el cursor a una posición más */
            	pos++; 
				/* Imprimimos el nuevo carácter introducido */
            	printf("%c", ch); 
				fflush(stdout);
			}
        }
	}
	/* Al terminar de introducir un comando en el buffer, hacer un salto de línea para obtener la salida del comando*/
	printf("\n");
}
/**
 * Devuelve un comando en una posición concreta de la lista del historial.
 *
 * @param list Historial de comandos
 * @param n Posición del comando
 * @return Puntero hacia el comando o NULL si no existe
 */
command *get_command_bypos(command *list, int n) {
	command *aux = list;
	if (n < 1)
		return NULL;
	n--;
	while (aux->next != NULL && n)
	{
		aux = aux->next;
		n--;
	}
	return aux->next;
};
/**
 * Crea un nuevo comando.
 *
 * @param args Array de carácteres que componen el comando completo
 * @param length Longitud total del comando sin incluir el salto de línea (\n)
 * @return Puntero hacia el nuevo comando
 */
command *new_command(char *args, int length) {
	command *aux;
	aux = (command *)malloc(sizeof(command));
	/* Si se ha indicado un comando en el trabajo, copiamos el array de argumentos al trabajo */
	if(args != NULL) {
		aux -> args = strndup(args, length);
		aux ->length = length;

	} else {
		aux ->args = NULL;
		aux -> length = 0;
	}
	aux->next = NULL;

	return aux;
}
/**
 * Añade un comando a la lista de comandos (Historial).
 *
 * @param lista Puntero hacia la lista donde debe ser guardado
 * @param item Puntero hacia el item que debe ser añadido a la lista
 */
void add_command(command *list, command *item) {
	command *aux = list->next;
	list->next = item;
	item->next = aux;
}
/**
 * Imprime por pantalla el comando.
 *
 * @param item Puntero hacia el comando
 * @param n Posición que ocupa el comando en la lista
 */
void print_command_item(command *item, int n) {

	printf("%d %s\n", n, item->args);

}
/**
 * Imprime por pantalla la lista de comandos (Historial).
 *
 * @param list Puntero hacia el primer elemento de la lista
 */
void print_command_list(command *list) {
	int n = 1;
	command * command = get_command_bypos(list, n);
	while(command != NULL) {
		print_command_item(command, n);
		n++;
		command = get_command_bypos(list, n);
	}
}

char getch() {
 int shell_terminal = STDIN_FILENO;
 struct termios conf;
 struct termios conf_new;
 char c;

 tcgetattr(shell_terminal,&conf); /* leemos la configuracion actual */
 conf_new = conf;

 conf_new.c_lflag &= (~(ICANON|ECHO)); /* configuramos sin buffer ni eco */
 conf_new.c_cc[VTIME] = 0;
 conf_new.c_cc[VMIN] = 1;

 tcsetattr(shell_terminal,TCSANOW,&conf_new); /* establecer configuracion */

 c = getc(stdin); /* leemos el caracter */
 
 tcsetattr(shell_terminal,TCSANOW,&conf); /* restauramos la configuracion */
 return c;
} 