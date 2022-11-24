#include <stdlib.h>

#include "common.h"

void get_cmd_output(const char *cmd, char *buffer, size_t buffer_size){
    FILE *fp = popen(cmd, "r");
    fread(buffer, 1, buffer_size, fp);
    fclose(fp);
}

int get_process_child_pid(int pid, char *buffer, size_t buffer_size){
    snprintf(buffer, buffer_size, "/proc/%d/task/%d/children", pid, pid);

    FILE *fp = fopen(buffer, "r");
    fread(buffer, 1, buffer_size, fp);
    fclose(fp);

    return strtol(buffer, NULL, 10);
}

void get_process_cmdline(int pid, char *buffer, size_t buffer_size){
    snprintf(buffer, buffer_size, "/proc/%d/cmdline", pid);
    FILE *fp = fopen(buffer, "r");
    fread(buffer, 1, buffer_size, fp);
    fclose(fp);
}

