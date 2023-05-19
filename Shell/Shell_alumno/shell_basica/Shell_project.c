/**
 * Nombre: Iván López Cervantes
 *
UNIX Shell Project

Sistemas Operativos
Grados I. Informatica, Computadores & Software
Dept. Arquitectura de Computadores - UMA

Some code adapted from "Fundamentos de Sistemas Operativos", Silberschatz et al.

To compile and run the program:
   $ gcc Shell_project.c job_control.c -o Shell
   $ ./Shell
	(then type ^D to exit program)

**/
#include "job_control.h" // remember to compile with module job_control.c
#include "util.h"
#include "string.h"
#define MAX_LINE 256 /* 256 chars per line, per command, should be enough. */

// -----------------------------------------------------------------------
//                            MAIN
// -----------------------------------------------------------------------
job *job_list;

void sigchld_handler()
{
	block_SIGCHLD();
	for (int i = list_size(job_list); i >= 1; i--)
	{
		job *job_item = get_item_bypos(job_list, i);
		if (job_item != NULL)
		{
			int status, info;
			int pid = job_item->pgid;
			enum status status_res;
			if (pid == waitpid(pid, &status, WUNTRACED | WNOHANG | WCONTINUED))
			{
				status_res = analyze_status(status, &info);
				if (status_res == EXITED || status_res == SIGNALED)
				{
					printf("Background process %s (%d) %s\n", job_item->command, job_item->pgid, status_strings[status_res]);
					printf("COMMAND->");
					fflush(stdout);
					delete_job(job_list, job_item);
				}
				else if (status_res == SUSPENDED)
				{
					job_item->state = STOPPED;
				}
				else if (status_res == CONTINUED)
				{
					job_item->state == BACKGROUND;
				}
			}
		}
	}
	unblock_SIGCHLD();
}
void execute_command(char *args[], int background, char *file_in, char *file_out)
{
	int pid_fork, pid_wait;
	int info;
	int status;
	enum status status_res;
	pid_fork = fork();
	if (pid_fork == 0)
	{ // Proceso hijo
		new_process_group(getpid());
		if (background == 0)
		{
			set_terminal(getpid());
		}
		restore_terminal_signals();
		FILE *outfile, *infile;
		int fnum1, fnum2;

		if (file_in != NULL)
		{
			if (NULL == (infile = fopen(file_in, "r")))
			{
				// si hay error, informamos y salimos
				printf("\tError: abriendo: %s\n", file_in);
				exit(-1);
			}
			fnum1 = fileno(infile);
			fnum2 = fileno(stdin);
			if (dup2(fnum1, fnum2) == -1)
			{
				// si hay error, informamos y salimos
				printf("\tError: redireccionando entrada\n");
				exit(-1);
			}
			fclose(infile);
		}
		if (file_out != NULL)
		{
			if (NULL == (outfile = fopen(file_out, "w")))
			{
				// si hay error, informamos y salimos
				printf("\tError: abriendo: %s\n", file_out);
				exit(-1);
			}
			fnum1 = fileno(outfile);
			fnum2 = fileno(stdout);
			if (dup2(fnum1, fnum2) == -1)
			{
				// si hay error, informamos y salimos
				printf("\tError: redireccionando salida\n");
				exit(-1);
			}
			fclose(outfile);
		}
		execvp(args[0], args);

		printf("Error, command not found: %s\n", args[0]);
		exit(-1);
	}
	else
	{

		if (background == 0)
		{

			waitpid(pid_fork, &status, WUNTRACED);
			set_terminal(getpid());

			status_res = analyze_status(status, &info);
			if (status_res == SUSPENDED)
			{
				add_job(job_list, new_job(pid_fork, args[0], STOPPED));
			}
			printf("Foreground pid: %d, command: %s, %s, info: %d\n", pid_fork, args[0], status_strings[status_res], info);
		}
		else
		{
			printf("Background job running... pid: %d, command: %s\n", pid_fork, args[0]);
			job *job_item = new_job(pid_fork, args[0], BACKGROUND);
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
	ignore_terminal_signals();
	signal(SIGCHLD, sigchld_handler);

	job_list = new_list("Jobs");
	while (1) /* Program terminates normally inside get_command() after ^D is typed*/
	{
		printf("COMMAND->");
		fflush(stdout);
		get_command(inputBuffer, MAX_LINE, args, &background); /* get next command */
		parse_redirections(args, &file_in, &file_out);

		if (args[0] == NULL)
			continue; // if empty command

		/* the steps are:
			 (1) fork a child process using fork()
			 (2) the child process will invoke execvp()
			 (3) if background == 0, the parent will wait, otherwise continue
			 (4) Shell shows a status message for processed command
			 (5) loop returns to get_commnad() function
		*/
		if (strcmp(args[0], "cd") == 0)
		{
			chdir(args[1]);
		}
		else if (strcmp(args[0], "jobs") == 0)
		{
			print_job_list(job_list);
		}
		else if (strcmp(args[0], "fg") == 0)
		{

			int n = 1;
			if (args[1] != NULL)
			{
				n = atoi(args[1]); // ASCII
			}
			block_SIGCHLD();
			job *job_item = get_item_bypos(job_list, n);

			if (job_item != NULL)
			{

				enum job_state job_state = job_item->state;
				if (job_state == STOPPED || job_state == BACKGROUND)
				{

					int pid_job_item = job_item->pgid;
					set_terminal(pid_job_item);
					job_item->state = FOREGROUND;
					killpg(pid_job_item, SIGCONT);
					waitpid(pid_job_item, &status, WUNTRACED);
					set_terminal(getpid());
					status_res = analyze_status(status, &info);
					char *command = strdup(job_item->command);
					if (status_res == SUSPENDED)
					{
						job_item->state = STOPPED;
					}
					else
					{
						delete_job(job_list, job_item);
					}
					printf("Foreground pid: %d, command: %s, %s, info: %d\n", pid_job_item, command, status_strings[status_res], info);
				}
			}
			else
			{
				printf("Error: Job not found: %d\n", n);
			}
			unblock_SIGCHLD();
		}
		else if (strcmp(args[0], "bg") == 0)
		{
			block_SIGCHLD();
			int n = 1;
			if (args[1] != NULL)
			{
				n = atoi(args[1]);
			}
			job *job_item = get_item_bypos(job_list, n);
			if (job_item != NULL)
			{
				if (job_item->state == STOPPED)
				{
					int pid_job_item = job_item->pgid;
					char *name = strdup(job_item->command);
					job_item->state = BACKGROUND;
					killpg(pid_job_item, SIGCONT);
					printf("Background job running... pid: %d, command: %s\n", pid_job_item, name);
				}
			}
			else
			{
				printf("Error: Job not found: %d\n", n);
			}
			unblock_SIGCHLD();
		}
		else
		{
			execute_command(args, background, file_in, file_out);
		}
	} // end while
}
