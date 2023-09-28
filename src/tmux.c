#include <stdlib.h>
#include <string.h>

#include "x11.h"
#include "tmux.h"

static const char tmux_dir_flag[] = { 'U', 'D', 'L', 'R' };
const char *tmux_dir_name[] = {
    "top",
    "bottom",
    "left",
    "right"
};

bool tmux_get_session_id(int pid, char *session_id, size_t session_id_size){
    char pts[20];
    char buffer[512];

    snprintf(buffer, sizeof(buffer), "/proc/%d/fd/0", pid);
    realpath(buffer, pts);
    size_t buffer_content_size = strlen(buffer) + 1;

    if(!get_cmd_output("tmux list-clients -F '#{client_tty} #{client_session}'", buffer, sizeof(buffer))) return false;
    char *line_start, *session_id_start;
    char *iter = line_start = buffer;

    do {
        if(*iter == ' '){
            *iter = '\0';
        } else if(*iter == '\n'){
            *iter = '\0';
            line_start = iter + 1;
        }

        if(strcmp(line_start, pts) == 0){
            iter++;
            session_id_start = iter;
            while(++iter <= buffer + buffer_content_size && *iter != '\n');
            if(iter <= buffer + sizeof(buffer)) *iter = '\0';
            strcpy(buffer, session_id_start);
            break;
        }
    } while(*(++iter));

    strncpy(session_id, buffer, session_id_size);
    return true;
}

static bool tmux_has_pane_in_direction(const char *session_id, Direction direction){
    char buffer[64];
    char result[2];

    snprintf(buffer, sizeof(buffer), "tmux display-message -t %s: -p '#{pane_at_%s}'", session_id, tmux_dir_name[direction]);
    get_cmd_output(buffer, result, sizeof(result));

    return strcmp(result, "0") == 0;
}

bool tmux_get_pane_pid(const char *session_id, int *pid){
    char buffer[128];
    char result[10];

    snprintf(buffer, sizeof(buffer), "tmux display-message -t %s: -p -F '#{pane_pid}'", session_id);
    if(!get_cmd_output(buffer, result, sizeof(result))) return false;

    *pid = strtol(result, NULL, 10);
    return true;
}

bool tmux_move_focus(char *session_id, Direction direction){
    char buffer[64];
    if(tmux_has_pane_in_direction(session_id, direction)){
        snprintf(buffer, sizeof(buffer), "tmux select-pane -%c -t %s:", tmux_dir_flag[direction], session_id);
        system(buffer);
        return true;
    }

    return false;
}

