# pane-navigator

This project aims provides a unified way to navigate between between i3, tmux and vim panes/splits with minimal configuration effort. 

# Dependencies
- x11
- libprocps
- msgpack

# Build/Install
```bash
mkdir build
cd build
cmake ..
make install
```

# Usage
In the i3 config replace the default focus bindings with the following:
```
bindsym $mod+h exec --no-startup-id pane-navigator left
bindsym $mod+j exec --no-startup-id pane-navigator down
bindsym $mod+k exec --no-startup-id pane-navigator up
bindsym $mod+l exec --no-startup-id pane-navigator right
```

# Development Status
- [x] i3 support
- [x] tmux support
- [x] vim support in terminal
- [x] vim support in shell
- [x] vim support in tmux/shell

# Caveats
- The pane navigation inside vim does currently not match vim's native behavior on complex pane layouts
