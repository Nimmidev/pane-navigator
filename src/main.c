#include <string.h>
#include "x11.h"
#include "i3.h"
#include "tmux.h"
#include "nvim.h"

const char *terminal_class_names[] = {
    "Alacritty",
    "kitty"
};

const char *shell_names[] = {
    "sh",
    "bash",
    "zsh",
    "fish"
};


static inline bool is_nvim_cmdline(const char *cmdline){
    return strcmp(cmdline, "vim") == 0 || strcmp(cmdline, "nvim") == 0;
}

static bool is_nvim_in_shell(int pid, int *nvim_pid){
    char buffer[1024];

    if(!get_process_cmdline(pid, buffer, sizeof(buffer))) return false;

    for(int i = 0; i < sizeof(shell_names) / sizeof(char *); i++){
        char *match = strstr(buffer, shell_names[i]);

        if(match != NULL && match - buffer == strlen(buffer) - strlen(shell_names[i])){
            get_process_child_pid(pid, nvim_pid);
            get_process_cmdline(*nvim_pid, buffer, sizeof(buffer));

            if(is_nvim_cmdline(buffer)) return true;
        }
    }

    return false;
}

static inline void move_pane(Direction direction){
    char buffer[128];
    bool moved = false;
    WindowInfo window_info;

    if(x11_get_active_window_info(&window_info)){
        for(int i = 0; i < sizeof(terminal_class_names) / sizeof(char *); i++){
            if(strcmp((const char *) window_info.class, terminal_class_names[i]) == 0){
                int child_pid;
                get_process_child_pid(window_info.pid, &child_pid);
                get_process_cmdline(child_pid, buffer, sizeof(buffer));

                if(is_nvim_cmdline(buffer) || is_nvim_in_shell(child_pid, &child_pid)){
                    moved = nvim_move_focus(child_pid, direction);
                } else if(strcmp(buffer, "tmux") == 0) {
                    char session_id[16];
                    if(!tmux_get_session_id(child_pid, session_id, sizeof(session_id))) break;
                    if(!tmux_get_pane_pid(session_id, &child_pid)) break;
                    if(is_nvim_in_shell(child_pid, &child_pid)) moved = nvim_move_focus(child_pid, direction);
                    if(!moved) moved = tmux_move_focus(session_id, direction);
                }

                break;
            }
        }
    }
    if(!moved) i3_move_focus(direction);
}

int main(int argc, char **argv){
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

    move_pane(direction);

    return 0;
}
