#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>

typedef enum Direction {
    DIRECTION_UP = 0,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT,
    DIRECTION_COUNT
} Direction;

void get_cmd_output(const char *cmd, char *buffer, size_t buffer_size);

#endif
