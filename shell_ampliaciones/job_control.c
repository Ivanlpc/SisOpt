/**
 * Nombre: Iván López Cervantes
 */

#include "job_control.h"
#ifndef __APPLE__
#endif

// -----------------------------------------------------------------------
//  get_command() reads in the next command line, separating it into distinct tokens
//  using whitespace as delimiters. setup() sets the args parameter as a
//  null-terminated string.
// -----------------------------------------------------------------------


void get_command(char inputBuffer[], int size, char *args[], int *background, command * list)
{
	int  /* # of characters in the command line */
		i,		/* loop index for accessing inputBuffer array */
		start,	/* index where beginning of next command parameter is */
		ct;		/* index of where to place the next parameter into args[] */

	ct = 0;
	*background = 0;
	int length = 0;
	/* read_input sirve para reconocer cuando el usuario presiona una flecha */
	read_input(inputBuffer, size, &length, list);
	//length = read(STDIN_FILENO, inputBuffer, size);
	start = -1;
	if (length == 0)
	{
		printf("\nBye\n");
		exit(0); /* ^d was entered, end of user command stream */
	}
	if (length < 0)
	{
		perror("error reading the command");
		exit(-1); /* terminate with error code of -1 */
	}
	if(strncmp(inputBuffer, "\n", 1) != 0 && strncmp(inputBuffer, " ", 1) != 0) {
		/* length - 1 para no guardar el salto de línea, ya que se añadirá uno cuando se presione `Enter` */
		add_command(list, new_command(inputBuffer, length - 1)); /*Añadimos comando al historial*/
	}
	/* examine every character in the inputBuffer */
	for (i = 0; i < length; i++)
	{
		switch (inputBuffer[i])
		{
		case ' ':
		case '\t': /* argument separators */
			if (start != -1)
			{
				args[ct] = &inputBuffer[start]; /* set up pointer */
				ct++;
			}
			inputBuffer[i] = '\0'; /* add a null char; make a C string */
			start = -1;
			break;

		case '\n': /* should be the final char examined */
			if (start != -1)
			{
				args[ct] = &inputBuffer[start];
				ct++;
			}
			inputBuffer[i] = '\0';
			args[ct] = NULL; /* no more arguments to this command */
			break;

		default: /* some other character */

			if (inputBuffer[i] == '&') // background indicator
			{
				*background = 1;
				if (start != -1)
				{
					args[ct] = &inputBuffer[start];
					ct++;
				}
				inputBuffer[i] = '\0';
				args[ct] = NULL; /* no more arguments to this command */
				i = length;		 // make sure the for loop ends now
			}
			if (inputBuffer[i] == '+') // respawned indicator
			{
				*background = 2;
				if (start != -1)
				{
					args[ct] = &inputBuffer[start];
					ct++;
				}
				inputBuffer[i] = '\0';
				args[ct] = NULL; /* no more arguments to this command */
				i = length;		 // make sure the for loop ends now
			}
			else if (start == -1)
				start = i; // start of new argument
		}				   // end switch
	}					   // end for
	args[ct] = NULL;	   /* just in case the input line was > MAXLINE */
}

// -----------------------------------------------------------------------
/* devuelve puntero a un nodo con sus valores inicializados,
devuelve NULL si no pudo realizarse la reserva de memoria*/
job *new_job(pid_t pid, const char *command, char *args[], enum job_state state)
{
	job *aux;
	aux = (job *)malloc(sizeof(job));
	aux->pgid = pid;
	aux->state = state;
	/* Si se ha indicado un comando en el trabajo, copiamos el array de argumentos al trabajo */
	if (args != NULL)
	{
		copy_args(aux->args, args);
	}
	aux->command = strdup(command);
	aux->next = NULL;

	return aux;
}

// -----------------------------------------------------------------------
/* inserta elemento en la cabeza de la lista */
void add_job(job *list, job *item)
{
	job *aux = list->next;
	list->next = item;
	item->next = aux;
	list->pgid++;
}

// -----------------------------------------------------------------------
/* elimina el elemento indicado de la lista
devuelve 0 si no pudo realizarse con exito */
int delete_job(job *list, job *item)
{
	job *aux = list;
	while (aux->next != NULL && aux->next != item)
		aux = aux->next;
	if (aux->next)
	{
		aux->next = item->next;
		delete_args(aux ->args);
		free(item->command);
		free(item);
		list->pgid--;
		return 1;
	}
	else
		return 0;
}
// -----------------------------------------------------------------------
/* busca y devuelve un elemento de la lista cuyo pid coincida con el indicado,
devuelve NULL si no lo encuentra */
job *get_item_bypid(job *list, pid_t pid)
{
	job *aux = list;
	while (aux->next != NULL && aux->next->pgid != pid)
		aux = aux->next;
	return aux->next;
}
// -----------------------------------------------------------------------
job *get_item_bypos(job *list, int n)
{
	job *aux = list;
	if (n < 1 || n > list->pgid)
		return NULL;
	n--;
	while (aux->next != NULL && n)
	{
		aux = aux->next;
		n--;
	}
	return aux->next;
}

// -----------------------------------------------------------------------
/*imprime una linea en el terminal con los datos del elemento: pid, nombre ... */
void print_item(job *item)
{

	printf("pid: %d, command: %s, state: %s\n", item->pgid, item->command, state_strings[item->state]);
}

// -----------------------------------------------------------------------
/*recorre la lista y le aplica la funcion pintar a cada elemento */
void print_list(job *list, void (*print)(job *))
{
	int n = 1;
	job *aux = list;
	printf("Contents of %s:\n", list->command);
	while (aux->next != NULL)
	{
		printf(" [%d] ", n);
		print(aux->next);
		n++;
		aux = aux->next;
	}
}

// -----------------------------------------------------------------------
/* interpretar valor estatus que devuelve wait */
enum status analyze_status(int status, int *info)
{
	if (WIFSTOPPED(status))
	{
		*info = WSTOPSIG(status);
		return (SUSPENDED);
	}
	else if (WIFCONTINUED(status))
	{
		*info = 0;
		return (CONTINUED);
	}
	else
	{
		// el proceso termio
		if (WIFSIGNALED(status))
		{
			*info = WTERMSIG(status);
			return (SIGNALED);
		}
		else
		{
			*info = WEXITSTATUS(status);
			return (EXITED);
		}
	}
	return 0;
}

// -----------------------------------------------------------------------
// cambia la accion de las seÃ±ales relacionadas con el terminal
void terminal_signals(void (*func)(int))
{
	signal(SIGINT, func);  // crtl+c interrupt tecleado en el terminal
	signal(SIGQUIT, func); // ctrl+\ quit tecleado en el terminal
	signal(SIGTSTP, func); // crtl+z Stop tecleado en el terminal
	signal(SIGTTIN, func); // proceso en segundo plano quiere leer del terminal
	signal(SIGTTOU, func); // proceso en segundo plano quiere escribir en el terminal
}

// -----------------------------------------------------------------------
void block_signal(int signal, int block)
{
	/* declara e inicializa máscara */
	sigset_t block_sigchld;
	sigemptyset(&block_sigchld);
	sigaddset(&block_sigchld, signal);
	if (block)
	{
		/* bloquea señal */
		sigprocmask(SIG_BLOCK, &block_sigchld, NULL);
	}
	else
	{
		/* desbloquea señal */
		sigprocmask(SIG_UNBLOCK, &block_sigchld, NULL);
	}
}
