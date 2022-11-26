#ifndef _NVIM_H
#define _NVIM_H

#include <stdio.h>
#include <stdbool.h>

#include "common.h"

bool nvim_move_focus(const char *socket_path, Direction direction);
void nvim_get_socket_path(int pid, char *buffer, size_t buffer_size);
bool nvim_get_pid(const char *tmux_session_id, int *nvim_pid);

#endif
