#include <stdlib.h>
#include <string.h>

#include "x11.h"
#include "nvim.h"
#include "tmux.h"

static const char tmux_dir_flag[] = { 'U', 'D', 'L', 'R' };
const char *tmux_dir_name[] = {
    "top",
    "bottom",
    "left",
    "right"
};

void tmux_get_session_by_pid(int pid, char *buffer, size_t buffer_size){
    char pts[20];
    snprintf(buffer, buffer_size, "/proc/%d/fd/0", pid);
    realpath(buffer, pts);

    get_cmd_output("tmux list-clients -F '#{client_tty} #{client_session}'", buffer, buffer_size);
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

static bool tmux_has_pane_in_direction(const char *session_id, Direction direction, char *buffer, size_t buffer_size){
    snprintf(buffer, buffer_size, "tmux display-message -t %s: -p '#{pane_at_%s}'", session_id, tmux_dir_name[direction]);
    char result[2];
    get_cmd_output(buffer, result, buffer_size);
    result[1] = '\0';

    return strcmp(result, "0") == 0;
}

int tmux_get_pane_pid(const char *session_id){
    char buffer[128];
    snprintf(buffer, 128, "tmux display-message -t %s: -p -F '#{pane_pid}'", session_id);
    char result[10], *iter = result;
    get_cmd_output(buffer, result, 10);
    while(*(++iter) != '\n');
    *iter = '\0';

    return strtol(result, NULL, 10);
}


static bool tmux_get_pid(int *pid){
    int window_pid = x11_get_active_window_pid();
    if(window_pid == 0) return false;

    int child_pid = *pid = get_process_child_pid(window_pid);
    if(child_pid == 0) return false;

    char buffer[32];
    get_process_cmdline(child_pid, buffer, 32);

    return strcmp(buffer, "tmux") == 0;
}

// TODO: update both method to return success/failure and take a int pointer for pid
bool tmux_move_focus(Direction direction, char *buffer, size_t buffer_size){
    int tmux_pid;

    if(tmux_get_pid(&tmux_pid)){
        tmux_get_session_by_pid(tmux_pid, buffer, buffer_size);
        char session_id[10];
        strcpy(session_id, buffer);

        int nvim_pid;
        if(nvim_get_pid(session_id, &nvim_pid)){
            nvim_get_socket_path(nvim_pid, buffer, buffer_size);
            bool moved = nvim_move_focus(buffer, direction);
            if(moved) return true;
        } 
        if(tmux_has_pane_in_direction(session_id, direction, buffer, buffer_size)){
            snprintf(buffer, buffer_size, "tmux select-pane -%c -t %s:", tmux_dir_flag[direction], session_id);
            system(buffer);
            return true;
        }
    }

    return false;
}

