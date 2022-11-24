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

// TODO: update both method to return success/failure and take a int pointer for pid
bool tmux_move_focus(Direction direction, char *buffer, size_t buffer_size){
    int window_pid = x11_get_active_window_pid();
    if(window_pid == 0) return false;

    int child_pid = get_process_child_pid(window_pid, buffer, buffer_size);
    if(child_pid == 0) return false;
    get_process_cmdline(child_pid, buffer, buffer_size);

    if(strcmp(buffer, "tmux") == 0){
        tmux_get_session_by_pid(child_pid, buffer, buffer_size);
        char session_id[10];
        strcpy(session_id, buffer);

        if(tmux_has_pane_in_direction(session_id, direction, buffer, buffer_size)){
            snprintf(buffer, buffer_size, "tmux select-pane -%c -t %s:", tmux_dir_flag[direction], session_id);
            printf("%d\n", direction);
            system(buffer);
            return true;
        }
    }

    return false;
}

