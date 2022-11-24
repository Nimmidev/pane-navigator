#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdbool.h>
#include <xdo.h>
#include <string.h>
#include <proc/readproc.h>

#include "i3.h"


#define BUF_SIZE 100

const char tmux_dir_flag[] = { 'U', 'D', 'L', 'R' };
const char *tmux_dir_name[] = {
    "top",
    "bottom",
    "left",
    "right"
};


int get_active_window_pid(){
    xdo_t *xdo = xdo_new(NULL);
    Window window;
    xdo_get_active_window(xdo, &window);
    return xdo_get_pid_window(xdo, window);
}

int get_child_pid(int pid, char *buffer){
    snprintf(buffer, BUF_SIZE, "/proc/%d/task/%d/children", pid, pid);

    FILE *fp = fopen(buffer, "r");
    fread(buffer, 1, BUF_SIZE, fp);
    fclose(fp);

    return strtol(buffer, NULL, 10);
}

void get_process_cmdline(int pid, char *buffer){
    snprintf(buffer, BUF_SIZE, "/proc/%d/cmdline", pid);
    FILE *fp = fopen(buffer, "r");
    fread(buffer, 1, BUF_SIZE, fp);
    fclose(fp);
}

void get_tmux_session_id_by_pid(int pid, char *buffer){
    char pts[20];
    snprintf(buffer, BUF_SIZE, "/proc/%d/fd/0", pid);
    realpath(buffer, pts);

    get_cmd_output("tmux list-clients -F '#{client_tty} #{client_session}'", buffer, BUF_SIZE);
    char *line_start, *session_id_start;
    char *iter = line_start = buffer;

    do {
        if(*iter == ' '){
            *iter = '\0';

            if(strcmp(line_start, pts) == 0){
                iter++;
                session_id_start = iter;
                while(*(++iter) != '\n');
                *iter = '\0';
                strcpy(buffer, session_id_start);
                break;
            }
        } else if(*iter == '\n'){
            *iter = '\0';
            line_start = iter + 1;
        }
    } while(*(++iter));
}

bool has_pane_in_direction(const char *session_id, Direction direction, char *buffer){
    snprintf(buffer, BUF_SIZE, "tmux display-message -t %s: -p '#{pane_at_%s}'", session_id, tmux_dir_name[direction]);
    char result[2];
    get_cmd_output(buffer, result, BUF_SIZE);
    result[1] = '\0';

    return strcmp(result, "0") == 0;
}

bool navigate_in_direction(Direction direction, char *buffer){
    int window_pid = get_active_window_pid();
    int child_pid = get_child_pid(window_pid, buffer);
    get_process_cmdline(child_pid, buffer);

    if(strcmp(buffer, "tmux") == 0){
        get_tmux_session_id_by_pid(child_pid, buffer);
        char session_id[10];
        strcpy(session_id, buffer);

        if(has_pane_in_direction(session_id, direction, buffer)){
            snprintf(buffer, BUF_SIZE, "tmux select-pane -%c -t %s:", tmux_dir_flag[direction], session_id);
            printf("%d\n", direction);
            system(buffer);
            return true;
        }
    }

    return false;
}

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

    bool success = navigate_in_direction(direction, buffer);
    if(!success) i3_move_focus(direction);

    return 0;
}
