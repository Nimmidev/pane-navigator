# i3-tmux-vim-nav

# Usage
In the i3 config replace the default focus bindings with the following:
```
bindsym $mod+$left exec --no-startup-id "path-to/tmux-nav.sh left || i3-msg focus left"
bindsym $mod+$down exec --no-startup-id "path-to/tmux-nav.sh down || i3-msg focus down"
bindsym $mod+$up exec --no-startup-id "path-to/tmux-nav.sh up || i3-msg focus up"
bindsym $mod+$right exec --no-startup-id "path-to/tmux-nav.sh right || i3-msg focus right"
```
