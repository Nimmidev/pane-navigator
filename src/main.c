#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <proc/readproc.h>

#include "i3.h"
#include "tmux.h"


#define BUF_SIZE 100

int main(int argc, char **argv){
    char buffer[BUF_SIZE];
    Direction direction = -1;

    if(argc < 2){
        fprintf(stderr, "Missing argument direction. Possible options: top, bottom, left, right\n");
        return 1;
    }

    for(int i = 0; i < DIRECTION_COUNT; i++){
        if(strcmp(argv[1], i3_dir_name[i]) == 0){
            direction = i;
            break;
        }
    }

    if(direction == -1){
        fprintf(stderr, "Unsupported direction. Valid options are: top, bottom, left, right\n");
        return 1;
    }

    bool success = tmux_move_focus(direction, buffer, BUF_SIZE);
    if(!success) i3_move_focus(direction);

    return 0;
}
