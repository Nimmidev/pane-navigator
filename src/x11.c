#include <xdo.h>

#include "x11.h"

int x11_get_active_window_pid(){
    xdo_t *xdo = xdo_new(NULL);
    Window window;
    xdo_get_active_window(xdo, &window);
    return xdo_get_pid_window(xdo, window);
}
