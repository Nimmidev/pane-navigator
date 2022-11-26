#include <string.h>
#include "x11.h"
#include "i3.h"
#include "tmux.h"
#include "nvim.h"


#define BUF_SIZE 100

const char *terminal_class_names[] = {
    "Alacritty",
    "test"
};

const char *shell_names[] = {
    "bash"
};
const size_t shell_names_length = sizeof(shell_names) / sizeof(char *);


static inline bool is_nvim_cmdline(const char *cmdline){
    return strcmp(cmdline, "vim") == 0 || strcmp(cmdline, "nvim") == 0;
}

static bool is_nvim_in_shell(int pid, int *nvim_pid, size_t shell_cmdline_offset){
    char buffer[1024];

    if(!get_process_cmdline(pid, buffer, sizeof(buffer))) return false;
    for(int i = 0; i < shell_names_length; i++){
        if(strcmp(buffer + shell_cmdline_offset, shell_names[i]) == 0){
            get_process_child_pid(pid, nvim_pid);
            get_process_cmdline(*nvim_pid, buffer, sizeof(buffer));

            if(is_nvim_cmdline(buffer)) return true;
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

    bool moved = false;
    WindowInfo window_info;

    if(x11_get_active_window(&window_info)){
        size_t length = sizeof(terminal_class_names) / sizeof(char *);
        for(int i = 0; i < length; i++){
            if(strcmp((const char *) window_info.class, terminal_class_names[i]) == 0){
                int child_pid;
                get_process_child_pid(window_info.pid, &child_pid);
                get_process_cmdline(child_pid, buffer, BUF_SIZE);

                if(is_nvim_cmdline(buffer) || is_nvim_in_shell(child_pid, &child_pid, 0)){
                    moved = nvim_move_focus(child_pid, direction);
                    printf("NVIM moved: %d\n", moved);
                } else if(strcmp(buffer, "tmux") == 0) {
                    char session_id[16];
                    if(!tmux_get_session_id(child_pid, session_id, sizeof(session_id))) break;
                    if(!tmux_get_pane_pid(session_id, &child_pid)) break;
                    if(is_nvim_in_shell(child_pid, &child_pid, 1)) moved = nvim_move_focus(child_pid, direction);
                    if(!moved) moved = tmux_move_focus(session_id, direction);
                }

                break;
            }
        }
    }
    if(!moved) i3_move_focus(direction);

    return 0;
}
