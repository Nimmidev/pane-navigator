#ifndef _TMUX_H
#define _TMUX_H

#include <stdbool.h>

#include "common.h"

extern const char *tmux_dir_name[];
bool tmux_move_focus(Direction direction, char *buffer, size_t buffer_size);
int tmux_get_pane_pid(const char *session_id);

#endif
