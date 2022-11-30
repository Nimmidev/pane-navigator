# pane-navigator

This project aims to provide a unified way to navigate between i3, tmux and **neovim** panes/splits with minimal configuration effort. 

## Support
At the moment only the following terminals and shells are supported:

Terminal: `alacritty` (tested), `kitty` (untested)  
Shell: `bash` (tested), `zsh` (untested), `fish` (untested)  

If you have another terminal or shell, go into `main.c` and add the shell binary name to `shell_names` and the terminal X11 class name to `terminal_class_names`. Alternatively open an issue with that information so i can add it.

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

It might be useful to keep the default `focus` bindings around on an additional modifier key. This is especially useful when using tabbed/stacked layouts. Otherwise all tmux/neovim panes have to be navigated first before the tab/stack changes.

```
bindsym $mod+Mod1+h focus left
bindsym $mod+Mod1+j down focus down
bindsym $mod+Mod1+k focus up
bindsym $mod+Mod1+lright focus right
```

# Development Status
- [x] i3 support
- [x] tmux support
- [x] neovim support in terminal
- [x] neovim support in shell
- [x] neovim support in tmux/shell

# Caveats
- The pane navigation inside neovim does currently not match neovim's native behavior on complex pane layouts
