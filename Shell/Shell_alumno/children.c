/**
 * Nombre: Iván López Cervantes
 */

#include "children.h"

/**
 * Devuelve el número de threads que tiene un proceso en específico.
 *
 * @param pid PID del proceso
 * @return número de threads que tiene ese proceso
 */

int get_threads_number(int pid)
{
    char path[32];
    DIR *dir;
    struct dirent *entry;
    /* Creamos la variable path con la ruta absoluta */
    snprintf(path, sizeof(path), "/proc/%d/task", pid);
    /* Abrimos la carpeta task del proceso indicado por parámetro */
    dir = opendir(path);
    int count = 0;
    /* Leemos su contenido, si es un directorio y su nombre es un PID de un proceso incrementamos el número de threads*/
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_DIR && isdigit(entry->d_name[0]))
        {
            count++;
        }
    }
    closedir(dir);
    return count;
}

/**
 * Devuelve el número de hijos que tiene un proceso en específico.
 *
 * @param pid PID del proceso
 * @return número de hijos que tiene el proceso
 */

int get_children_number(int parent_pid)
{
    char path[32];
    int count = 0;

    /* Abrimos la carpeta /proc */
    DIR *dir = opendir("/proc");
    if (!dir)
    {
        return 0;
    }
    struct dirent *process;

    /* Leemos su contenido */
    while ((process = readdir(dir)))
    {

        /* Nada más nos interesan los procesos activos, por lo que omitimos los que no son un directorio y su nombre no es su PID */

        if (process->d_type != DT_DIR || !isdigit(*process->d_name))
        {
            continue;
        }

        /* Guardamos el PID del proceso que queremos analizar */
        int child_pid = atoi(process->d_name);

        /* Comprobamos que el proceso sea diferente al que le hemos pasado por parámetro parent_pid */
        if (child_pid != parent_pid)
        {
            char child_path[32];

            /* Abrimos el archivo con la información del proceso */
            snprintf(child_path, sizeof(child_path), "/proc/%d/status", child_pid);
            FILE *child_file = fopen(child_path, "r");
            if (!child_file)
            {
                continue;
            }
            int child_ppid = 0;
            char child_line[256];

            /* Recorremos el archivo status hasta encontrar la línea PPid, que corresponde al PID del padre */
            while (fgets(child_line, sizeof(child_line), child_file))
            {
                if (strncmp(child_line, "PPid:", 5) == 0)
                {
                    child_ppid = atoi(child_line + 6);
                    break;
                }
            }
            fclose(child_file);

            /* Si el PPID del hijo coincide con el que hemos pasado por parámetro, incrementamos el contador de hijos */
            if (child_ppid == parent_pid)
            {
                count++;
            }
        }
    }
    closedir(dir);
    return count;
}
/**
 * Devuelve el comando que ha ejecutado el proceso especificado.
 *
 * @param pid PID del proceso
 * @param name Sobreescribe este parámetro con el nombre del comando que ha ejecutado el proceso
 * @return 0 si se ha podido leer el comando, -1 si hubo algún error
 */
int get_command_name(int pid, char *name)
{
    FILE *comm;
    char path[32];
    char buff[256];

    /* Creamos el path hacia el archivo `comm` del proceso con el PID indicado por parámetro */
    snprintf(path, sizeof(path), "/proc/%d/comm", pid);
    /* Abrimos el archivo */
    comm = fopen(path, "r");
    if (comm == NULL)
    {
        printf("Error opening the file /proc/%d/comm", pid);
        return -1;
    }

    /* Ese archivo contiene el comando que ha ejecutado el proceso, lo escaneamos porque solo tiene una línea */
    if (fscanf(comm, "%s", buff) == 1)
    {
        /* Devolvemos el resultado por el parámetro `name` */
        strcpy(name, buff);
    }
    else
    {
        /* Si hay error al escanear el nombre del comando, devolvemos un error*/
        fclose(comm);
        return -1;
    }
    /* Si todo funciona correctamente, devolvemos 0 */
    fclose(comm);
    return 0;
}
/**
 * Imprime una lista de los procesos que se están ejecutando en el sistema, junto con su PID, el nombre del comando que lo inició,
 * el número de hijos y el número de hilos (threads) de cada proceso.
 */
void print_children_list()
{
    DIR *proc_folder;
    FILE *comm, *stat;
    struct dirent *process;
    int pid, nchildren, nthreads;
    char command_name[256];

    /* Abrimos la carpeta que contiene a todos los procesos */
    proc_folder = opendir("/proc");
    if (proc_folder == NULL)
    {
        printf("Error opening folder /proc");
        return;
    }
    /* Imprimimos por pantalla la cabecera del comando */
    printf("%-10s %-40s %-10s %-10s\n", "PID", "COMMAND", "#CHILDREN", "#THREADS");

    /* Leemos el contenido de la carpeta `proc`, omitimos los archivos o los que no tengan su PID como nombre */
    while ((process = readdir(proc_folder)) != NULL)
    {
        if (process->d_type != DT_DIR || !isdigit(process->d_name[0]))
        {
            continue;
        }
        /* Guardamos el PID del proceso a analizar */
        pid = atoi(process->d_name);
        /* Obtenemos el número de thhreads de ese proceso */
        nthreads = get_threads_number(pid);
        /* Obtenemos el número de hijos de ese proceso */
        nchildren = get_children_number(pid);
        /* Obtenemos el nombre del comando de ese proceso */
        get_command_name(pid, command_name);
        /* Imprimimos el resultado por pantalla */
        printf("%-10d %-40s %-10d %-10d\n", pid, command_name, nchildren, nthreads);
        fflush(stdout);
    }
    closedir(proc_folder);
}
