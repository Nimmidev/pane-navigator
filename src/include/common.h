#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <stdbool.h>

typedef enum Direction {
    DIRECTION_UP = 0,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT,
    DIRECTION_COUNT
} Direction;

bool get_cmd_output(const char *cmd, char *buffer, size_t buffer_size);
bool get_process_child_pid(int process_pid, int *child_pid);
bool get_process_cmdline(int pid, char *buffer, size_t buffer_size);

#endif
