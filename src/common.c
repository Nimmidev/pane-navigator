#include <stdlib.h>

#include "common.h"

bool get_cmd_output(const char *cmd, char *buffer, size_t buffer_size){
    FILE *fp;
    if((fp = popen(cmd, "r")) == NULL) return false;
    size_t bytes_read = fread(buffer, 1, buffer_size, fp);
    buffer[bytes_read - 1] = '\0';
    fclose(fp);

    return true;
}

bool get_process_child_pid(int process_pid, int *child_pid){
    char buffer[50];
    FILE *fp;

    snprintf(buffer, 50, "/proc/%d/task/%d/children", process_pid, process_pid);

    if((fp = fopen(buffer, "r")) == NULL) return false;
    fread(buffer, 1, 50, fp);
    fclose(fp);

    *child_pid = strtol(buffer, NULL, 10);
    return true;
}

bool get_process_cmdline(int pid, char *buffer, size_t buffer_size){
    FILE *fp;
    snprintf(buffer, buffer_size, "/proc/%d/cmdline", pid);

    if((fp = fopen(buffer, "r")) == NULL) return false;
    fread(buffer, 1, buffer_size, fp);
    fclose(fp);

    return true;
}

