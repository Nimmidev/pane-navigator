#include <xdo.h>

#include "x11.h"

int x11_get_active_window_pid(){
    xdo_t *xdo = xdo_new(NULL);
    Window window;
    if(xdo_get_active_window(xdo, &window) == 1) return 0;
    return xdo_get_pid_window(xdo, window);
}
