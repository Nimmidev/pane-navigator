#include <stdio.h>
#include <stdlib.h>
#include <X11/Xutil.h>

#include "x11.h"

// Based on https://github.com/jordansissel/xdotool/blob/master/xdo.c

static unsigned char *x11_get_window_property_by_atom(
    Display *xdpy,
    Window window,
    Atom atom,
    long *nitems,
    Atom *type,
    int *size
) {
    Atom actual_type;
    int actual_format;
    unsigned long _nitems;
    unsigned long bytes_after; /* unused */
    unsigned char *prop;
    int status;

    status = XGetWindowProperty(
        xdpy, window, atom, 0, (~0L),
        False, AnyPropertyType, &actual_type,
        &actual_format, &_nitems, &bytes_after,&prop
    );

    if(status == BadWindow){
        fprintf(stderr, "window id # 0x%lx does not exists!", window);
        return NULL;
    } else if (status != Success){
        fprintf(stderr, "XGetWindowProperty failed!");
        return NULL;
    }

    if(nitems != NULL) *nitems = _nitems;
    if(type != NULL) *type = actual_type;
    if(size != NULL) *size = actual_format;

    return prop;
}

static bool x11_is_ewmh_supported(Display *xdpy, const char *feature){
    Atom type = 0;
    long nitems = 0L;
    int size = 0;
    Atom *results = NULL;

    Window root;
    Atom request;
    Atom feature_atom;

    request = XInternAtom(xdpy, "_NET_SUPPORTED", False);
    feature_atom = XInternAtom(xdpy, feature, False);
    root = XDefaultRootWindow(xdpy);
    results = (Atom *) x11_get_window_property_by_atom(xdpy, root, request, &nitems, &type, &size);

    for(long i = 0L; i < nitems; i++){
        if(results[i] == feature_atom){
            free(results);
            return true;
        }
    }

    free(results);

    return false;
}

static bool x11_get_active_window(Display *xdpy, Window *window){
    Atom type;
    int size;
    long nitems = 0;
    Atom request = XInternAtom(xdpy, "_NET_ACTIVE_WINDOW", false);
    Window root = XDefaultRootWindow(xdpy);
    unsigned char *data = x11_get_window_property_by_atom(xdpy, root, request, &nitems, &type, &size);

    if(nitems > 0) *window = *((Window *) data);
    else *window = 0;

    free(data);

    if(*window == 0){
        fprintf(stderr, "Get active window failed\n");
        return false;
    }

    return true;
}

bool x11_get_window_class_name(Display *xdpy, Window window, unsigned char **class){
    XClassHint class_hint;
    Status ret = XGetClassHint(xdpy, window, &class_hint);

    if(ret){
        XFree(class_hint.res_name);
        *class = (unsigned char *) class_hint.res_class;
    } else {
        *class = NULL;
    }

    if(ret == 0){
        fprintf(stderr, "Get window class name failed\n");
        return false;
    }

    return true;
}

int x11_get_window_pid(Display *xdpy, Window window){
    Atom type;
    int size;
    long nitems = 0;
    Atom atom_NET_WM_PID = XInternAtom(xdpy, "_NET_WM_PID", false);
    unsigned char *data = x11_get_window_property_by_atom(xdpy, window, atom_NET_WM_PID, &nitems, &type, &size);

    int pid = 0;
    if(nitems > 0) pid = (int) *((unsigned long *) data);
    free(data);

    return pid;
}

bool x11_get_active_window_info(WindowInfo *window_info){
    Display *xdpy;
    Window window;

    if((xdpy = XOpenDisplay(NULL)) == NULL){
        fprintf(stderr, "Error while trying to open display\n");
        return false;
    }

    if(x11_is_ewmh_supported(xdpy, "_NET_ACTIVE_WINDOW") == false){
        fprintf(stderr, "_NET_ACTIVE_WINDOW not supported\n");
        return false;
    }

    if(!x11_get_active_window(xdpy, &window)) return false;
    if(!x11_get_window_class_name(xdpy, window, &window_info->class)) return false;
    window_info->pid = x11_get_window_pid(xdpy, window);

    return true;
}
