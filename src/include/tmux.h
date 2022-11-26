#ifndef _TMUX_H
#define _TMUX_H

#include <stdbool.h>

#include "common.h"

extern const char *tmux_dir_name[];

bool tmux_get_session_id(int pid, char *session_id, size_t session_id_size);
bool tmux_get_pane_pid(const char *session_id, int *pid);
bool tmux_move_focus(char *session_id, Direction direction);

#endif
