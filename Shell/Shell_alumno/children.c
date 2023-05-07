#include "children.h"
#include <string.h>

int get_threads_number(int pid) {
    char path[32];
    DIR *dir;
    struct dirent *entry;
    snprintf(path, sizeof(path), "/proc/%d/task", pid);
    dir = opendir(path);
    int count = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && isdigit(entry->d_name[0])) {
            count++;
        }
    }
    closedir(dir);
    return count;
    
}

int get_children_number(int parent_pid) {
    char path[32];
    int count = 0;
    DIR* dir = opendir("/proc");
    if (!dir) {
        return 0;
    }
    struct dirent* process;
    while ((process = readdir(dir))) {
        if (process -> d_type != DT_DIR || !isdigit(*process -> d_name)) {
            continue;
        }
        int child_pid = atoi(process -> d_name);
        if (child_pid != parent_pid) {
            char child_path[32];
            snprintf(child_path, sizeof(child_path), "/proc/%d/status", child_pid);
            FILE* child_file = fopen(child_path, "r");
            if (!child_file) {
                continue;
            }
            int child_ppid = 0;
            char child_line[256];
            while (fgets(child_line, sizeof(child_line), child_file)) {
                if (strncmp(child_line, "PPid:", 5) == 0) {
                    child_ppid = atoi(child_line + 6);
                    break;
                }
            }
            fclose(child_file);
            if (child_ppid == parent_pid) {
                count++;
            }
        }
    }
    closedir(dir);
    return count;
}

int get_command_name(int pid, char *name) {
    FILE * comm;
    char path[32] = "/proc/";
    char buff[256];
    snprintf(path, sizeof(path), "/proc/%d/comm", pid);
    comm = fopen(path, "r");
    if(comm == NULL) {
        printf("Error opening the file /proc/%d/comm", pid);
        return -1;
    }
    if(fscanf(comm, "%s", buff) == 1) {
        strcpy(name, buff);
    } else {
        fclose(comm);
        return -1;
    }
    fclose(comm);
    return 0;
    
}
void print_children_list() {
    DIR * proc_folder;
    FILE *comm, *stat;
    struct dirent *process;
    int pid, nchildren, nthreads;
    char command_name[256];
    proc_folder = opendir("/proc");
    if(proc_folder == NULL) {
        printf("Error opening folder /proc");
        return;
    }
    printf("%-10s %-40s %-10s %-10s\n", "PID", "COMMAND", "#CHILDREN", "#THREADS");
        while((process = readdir(proc_folder)) != NULL) {
            if (process -> d_type != DT_DIR || !isdigit(process->d_name[0])) {
				continue;
			}
            pid = atoi(process -> d_name);
            nthreads = get_threads_number(pid);
            nchildren = get_children_number(pid);
            get_command_name(pid, command_name);
            printf("%-10d %-40s %-10d %-10d\n", pid, command_name, nchildren, nthreads);
            fflush(stdout);
        }
        closedir(proc_folder);
}
