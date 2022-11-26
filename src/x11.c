#include <xdo.h>
#include <stdio.h>

#include "x11.h"

bool x11_get_active_window(WindowInfo *window_info){
    xdo_t *xdo = xdo_new(NULL);
    Window window;

    if(xdo_get_active_window(xdo, &window) == XDO_ERROR) return false;
    if(xdo_get_window_classname(xdo, window, &window_info->class) == XDO_ERROR) return false;
    window_info->pid = xdo_get_pid_window(xdo, window);

    return true;
}
