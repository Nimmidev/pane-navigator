#ifndef _NVIM_H
#define _NVIM_H

#include <stdio.h>
#include <stdbool.h>

#include "common.h"

bool nvim_move_focus(int pid, Direction direction);

#endif
