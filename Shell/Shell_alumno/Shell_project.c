/**
 * Nombre: Iván López Cervantes
 *
UNIX Shell Project

Sistemas Operativos
Grados I. Informatica, Computadores & Software
Dept. Arquitectura de Computadores - UMA

Some code adapted from "Fundamentos de Sistemas Operativos", Silberschatz et al.

To compile and run the program:
   $ gcc Shell_project.c job_control.c util.c children.c -o Shell
   $ ./Shell
	(then type ^D to exit program)

**/
#include "job_control.h" // remember to compile with module job_control.c
#include "children.h" // remember to compile with module children.c
#include "util.h" // remember to compile with module util.c
#include "string.h"
#define MAX_LINE 256 /* 256 chars per line, per command, should be enough. */

// -----------------------------------------------------------------------
//                            MAIN
// -----------------------------------------------------------------------
/* Variable global de la lista de trabajos*/
job *job_list;

/* Ejecuta un comando en la terminal */
void execute_command(char *args[], int background, char * file_in, char* file_out);
/* Maneja la señal SIGCHLD*/
void sigchld_handler();
void sighup_handler();
void sighup_handler() {
	FILE *fp;
    fp=fopen("hup.txt","a"); // abre un fichero en modo 'append'
	if(fp == NULL) {
		printf("Error al abrir el fichero");
	}
    fprintf(fp, "SIGHUP recibido.\n"); //escribe en el fichero
	fclose(fp);
}
/* Función principal */
int main(void);

void sigchld_handler() {
	/* Bloqueamos la señal SIGCHLD para que no entre mientras se están modificando trabajos en la lista y no lea datos erróneos*/
	block_SIGCHLD();

	/* Recorremos la lista de trabajos */
	for (int i = list_size(job_list); i >= 1; i--) {

		job *job_item = get_item_bypos(job_list, i);
		if (job_item != NULL) {
			int status, info;
			int pid = job_item->pgid;
			enum status status_res;

			/* Realizamos un wait no bloqueante al proceso para poder ver su estado */
			if (pid == waitpid(pid, &status, WUNTRACED | WNOHANG | WCONTINUED)) {

				/* Analizamos su estado */
				status_res = analyze_status(status, &info);

				/* Si el proceso ha terminado ejecutamos esto */
				if (status_res == EXITED || status_res == SIGNALED) {

					/* Si el trabajo es de tipo RESPAWNABLE (Ampliación de Shell)*/
					if(job_item -> state == RESPAWNABLE) {
						
						char * args[MAX_LINE / 2];
						printf("Respawned process %s (%d) %s\n", job_item->command, job_item->pgid, status_strings[status_res]);

						/* Copiamos el comando y sus argumentos a otro array para volver a lanzar el proceso */
						copy_args(args, job_item -> args);
						/* Eliminamos el trabajo, ya que se creará otro cuando se lance el proceso */
						delete_job(job_list, job_item);

						/* Volvemos a ejecutar el comando en modo RESPAWNABLE (2) y sin redirecciones (NULL, NULL)*/
						execute_command(args, 2, NULL, NULL);
						printf("COMMAND->");
						fflush(stdout);

					} else {
						/* Si el trabajo está en modo BACKGROUND, indicamos en la terminal que ha terminado el proceso
						por lo que borramos también el trabajo asociado al proceso */
						printf("Background process %s (%d) %s\n", job_item->command, job_item->pgid, status_strings[status_res]);
						printf("COMMAND->");
						fflush(stdout);
						delete_job(job_list, job_item);
					}
				}
				/* Si el proceso está suspendido cambiamos el estado del trabajo a STOPPED*/
				else if (status_res == SUSPENDED) {
					job_item->state = STOPPED;
				}
				/* Si el proceso está en ejecución cambiamos el estado del trabajo a BACKGROUND */
				else if (status_res == CONTINUED) {
					job_item->state == BACKGROUND;
				}
			}
		}
	}
	/* Desbloqueamos la señal SIGCHLD para que se pueda volver a usar cuando acabe un proceso */
	unblock_SIGCHLD();
}

void execute_command(char *args[], int background, char * file_in, char* file_out) {
	int pid_fork, pid_wait;
	int info;
	int status;				
	enum status status_res;
	pid_fork = fork();
			if (pid_fork == 0) { // Proceso hijo
			/* El proceso hijo creará un grupo de procesos con su PID */
				new_process_group(getpid());
				/* Si el proceso debe ejecutarse en primer plano, le cedemos la terminal al hijo */
				if (background == 0) {
					set_terminal(getpid());
				}
				restore_terminal_signals();
				FILE *outfile, *infile;
				int fnum1, fnum2;

				/* Si hemos indicado un archivo como redirección de entrada*/
				if (file_in != NULL) {
					/* Lo abrimos y comprobamos de que esté correcto*/
					if (NULL == (infile = fopen(file_in, "r"))) {
						// Si hay error, informamos y salimos
						printf("\tError: abriendo: %s\n", file_in);
						exit(-1);
					}
					/* Guardamos el número de la entrada stdin y el archivo indicado */
					fnum1 = fileno(infile);
					fnum2 = fileno(stdin);
					/* Cambiamos la entrada stdin por el archivo indicado */
					if (dup2(fnum1, fnum2) == -1) {
						// Si hay error, informamos y salimos
						printf("\tError: redireccionando entrada\n");
						exit(-1);
					}
					fclose(infile);
				}
				/* Si hemos indicado un archivo como redirección de salida */
				if (file_out != NULL) {
					/* Lo abrimos y comprobamos que esté correcto */
					if (NULL == (outfile = fopen(file_out, "w"))) {
						// si hay error, informamos y salimos
						printf("\tError: abriendo: %s\n", file_out);
						exit(-1);
					}
					/* Guardamos el número de la salida stdout y el archivo indicado */
					fnum1 = fileno(outfile);
					fnum2 = fileno(stdout);
					/* Cambiamos la salida stdout por el archivo indicado */
					if (dup2(fnum1, fnum2) == -1) {
						// Si hay error, informamos y salimos
						printf("\tError: redireccionando salida\n");
						exit(-1);
					}
					fclose(outfile);
				}
				/* Ejecutamos el comando */
				execvp(args[0], args);
				/* Si hay algún error al ejecutar el comando, se ejecutan estas líneas */
				printf("Error, command not found: %s\n", args[0]);
				exit(-1);
			}
			else {
				/* Si el comando es en primer plano */
				if (background == 0) {
					/* Esperamos a que termine el proceso */
					waitpid(pid_fork, &status, WUNTRACED);
					/* Devolvemos la terminal al padre */
					set_terminal(getpid());
					/* Analizamos el estado del proceso */
					status_res = analyze_status(status, &info);
					/* Si se ha suspendido creamos un trabajo y lo añadimos a la lista */
					if (status_res == SUSPENDED) {
						add_job(job_list, new_job(pid_fork, args[0], args, STOPPED));
					}
					printf("Foreground pid: %d, command: %s, %s, info: %d\n", pid_fork, args[0], status_strings[status_res], info);
				}
				/* Si el comando es en modo RESPAWNABLE (Ampliación Shell)*/
				else if (background == 2) {
					/* Creamos un nuevo trabajo de tipo RESPAWNABLE y lo añadimos a la lista */
					printf("Respawnable job running... pid: %d, command: %s\n", pid_fork, args[0]);
					job *job_item = new_job(pid_fork, args[0], args, RESPAWNABLE);
					add_job(job_list, job_item);
				/* Si el comando es en modo BACKGROUND */
				} else {
					/* Creamos un nuevo trabajo de tipo RESPAWNABLE y lo añadimos a la lista */
					printf("Background job running... pid: %d, command: %s\n", pid_fork, args[0]);
					job *job_item = new_job(pid_fork, args[0], args, BACKGROUND);
					add_job(job_list, job_item);
				}
			}
}

int main(void)
{
	char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
	int background;				/* equals 1 if a command is followed by '&' */
	char *args[MAX_LINE / 2];	/* command line (of 256) has max of 128 arguments */
	// probably useful variables:
	int pid_fork, pid_wait; /* pid for created and waited process */
	int status;				/* status returned by wait */
	enum status status_res; /* status processed by analyze_status() */
	int info;				/* info processed by analyze_status() */
	char *file_in, *file_out;
	/* Ignoramos las señales de la terminal */
	ignore_terminal_signals();
	/* Usamos la función sigchld_handler para manejar la señal SIGCHLD*/
	signal(SIGCHLD, sigchld_handler);
	signal(SIGHUP, sighup_handler);
	/* Creamos una nueva lista de trabajos con nombre `Jobs`*/
	job_list = new_list("Jobs");
	while (1) /* Program terminates normally inside get_command() after ^D is typed*/
	{
		printf("COMMAND->");
		fflush(stdout);
		get_command(inputBuffer, MAX_LINE, args, &background); /* get next command */
		parse_redirections(args, &file_in, &file_out); //Comprobamos si se han especificado redirecciones en el comando

		if (args[0] == NULL)
			continue; // if empty command

		/* the steps are:
			 (1) fork a child process using fork()
			 (2) the child process will invoke execvp()
			 (3) if background == 0, the parent will wait, otherwise continue
			 (4) Shell shows a status message for processed command
			 (5) loop returns to get_commnad() function
		*/

		/* Si el comando es `cd` */
		if (strcmp(args[0], "cd") == 0) {
			/* Cambiamos el directorio al indicado por el primer parámetro del comando*/
			chdir(args[1]);
		}
		/* Si el comando es `jobs`*/
		else if (strcmp(args[0], "jobs") == 0) {
			/* Imprimimos la lista de trabajos */
			print_job_list(job_list);
		}
		/* Si el comando es `fg` */
		else if (strcmp(args[0], "fg") == 0) {
			int n = 1;
			/* Si no se ha especificado el número del trabajo, se usa 1 por defecto*/
			if (args[1] != NULL) {
				n = atoi(args[1]);
			}
			/* Bloqueamos la señal SIGCHLD ya que estaremos editando la lista de trabajos*/
			block_SIGCHLD();
			/* Obtenemos el trabajo indicado por comando */
			job *job_item = get_item_bypos(job_list, n);

			if (job_item != NULL) {

				enum job_state job_state = job_item->state;
				/* Si el trabajo está detenido, en segundo plano o en modo respawnable (Ampliación Shell)*/
				if (job_state == STOPPED || job_state == BACKGROUND || job_state == RESPAWNABLE) {

					int pid_job_item = job_item->pgid;
					/* Le damos la terminal al proceso que contiene el trabajo */
					set_terminal(pid_job_item);
					/* Le cambiamos el estado del trabajo a primer plano */
					job_item->state = FOREGROUND;
					/* Le indicamos con la señal SIGCONT que debe continuar su ejecución */
					killpg(pid_job_item, SIGCONT);
					/* Esperamos su finalización */
					waitpid(pid_job_item, &status, WUNTRACED);
					/* Le devolvemos la terminal al padre */
					set_terminal(getpid());
					/* Analizamos su estado */
					status_res = analyze_status(status, &info);
					char *command = strdup(job_item->command);
					/* Si está suspendido le cambiamos el estado en el trabajo, si no lo está significa que ha terminado
					por lo que lo borramos de la lista de trabajos */
					if (status_res == SUSPENDED) {
						job_item->state = STOPPED;
					}
					else {
						delete_job(job_list, job_item);
					}
					printf("Foreground pid: %d, command: %s, %s, info: %d\n", pid_job_item, command, status_strings[status_res], info);
				}
			}
			else {
				printf("Error: Job not found: %d\n", n);
			}
			/* Desbloqueamos la señal SIGCHLD para indicar que hemos terminado de modificar la lista de trabajos */
			unblock_SIGCHLD();
		}
		/* Si el comando es `bg` */
		else if (strcmp(args[0], "bg") == 0) {
			block_SIGCHLD();
			int n = 1;
			/* Si no indicamos número del trabajo, se usa 1 por defecto */
			if (args[1] != NULL) {
				n = atoi(args[1]);
			}
			/* Obtenemos el trabajo de la lista */
			job *job_item = get_item_bypos(job_list, n);
			if (job_item != NULL) {
				/* Si está detenido o en modo Respawnable (Ampliación de Shell)*/
				if (job_item->state == STOPPED || job_item -> state == RESPAWNABLE) {
					int pid_job_item = job_item->pgid;
					char *name = strdup(job_item->command);
					/* Cambiamos el estado a BACKGROUND y le indicamos que continúe ejecutándose con la señal SIGCONT */
					job_item->state = BACKGROUND;
					killpg(pid_job_item, SIGCONT);
					printf("Background job running... pid: %d, command: %s\n", pid_job_item, name);
				}
			}
			else {
				printf("Error: Job not found: %d\n", n);
			}
			/* Desbloqueamos la señal SIGCHLD para indicar que hemos terminado de editar la lista de trabajos*/
			unblock_SIGCHLD();
		}
		/* Si el comando es `children` (Ampliación de Shell)*/
		else if (strcmp(args[0], "children") == 0) {
			/* Imprimimos los procesos en la terminal */
			print_children_list();
		}
		else if (strcmp(args[0], "currjob") == 0) {
			job * item = get_item_bypos(job_list, 1);
			if(item == NULL) {
				printf("No hay trabajo actual\n");
			} else {
				printf("Trabajo actual: PID=%d command=%s\n", item ->pgid, item ->command);
			}
		}
		else if (strcmp(args[0], "deljob") == 0) {
		job * item = get_item_bypos(job_list, 1);
			if(item == NULL) {
				printf("No hay trabajo actual\n");
			} else {
				if(item ->state != BACKGROUND) {
					printf("No se permiten borrar trabajos en segundo plano suspendidos\n");
				} else {
					printf("Borrando trabajo actual de la lista de jobs: PID=%d command=%s\n", item ->pgid, item ->command);
					delete_job(job_list, item);
				}
				
			}
		}
		else if (strcmp(args[0], "zjobs") == 0) {
			print_zombies();
		}
		else if(strcmp(args[0], "bgteam") == 0) {
			if(args[1] == NULL || args[2] == NULL) {
				printf("El comando bgteam requiere dos argumentos\n");
				continue;
			}
			int n = atoi(args[1]);
			if(n <= 0) continue;
			for(int i = 0; i < atoi(args[1]); i++) {
				
				execute_command(args + 2, 1, file_in, file_out);
			}
		}
		/* Si no es un comando interno de la terminal, lo ejecutamos buscando el archivo en su path*/
		else {
			execute_command(args, background, file_in, file_out);
		}
	} // fin del while
}
