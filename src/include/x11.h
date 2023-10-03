#ifndef _X11_H
#define _X11_H

#include <stdbool.h>

typedef struct {
    int pid;
    unsigned char *class;
} WindowInfo;

bool x11_get_active_window_info(WindowInfo *window_info);
void x11_free_window_info(WindowInfo *window_info);

#endif
