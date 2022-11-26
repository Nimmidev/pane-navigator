# pane-navigator

This project aims provides a unified way to navigate between between i3, tmux and vim panes/splits with minimal configuration effort. 
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
bindsym $mod+$left exec --no-startup-id pane-navigator left
bindsym $mod+$down exec --no-startup-id pane-navigator down
bindsym $mod+$up exec --no-startup-id pane-navigator up
bindsym $mod+$right exec --no-startup-id pane-navigator right
```

# Development Status
- [x] i3 support
- [x] tmux support
- [x] vim support in tmux
- [ ] vim support
