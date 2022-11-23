#!/bin/sh

if [ $# -lt 1 ]; then
    echo "Missing param"
    exit 1
fi

try_select_pane_in_direction(){
    is_last_pane_in_direction=$(tmux display-message -p "#{pane_at_$1}")

    if [ $is_last_pane_in_direction = "0" ]; then
        tmux select-pane "-$2" -t "$3:"
        return 0
    fi

    return 1
}

pid=$(xdotool getactivewindow getwindowpid)
child=$(ps --ppid=$pid -o pid=,tty=,command= | head -1)

if echo "$child" | grep -q tmux; then
    clients=$(tmux list-clients -F "#{client_tty} #{client_session}")
    pts=$(echo "$child" | xargs | cut -d " " -f 2)
    session_id=$(echo "$clients" | grep "$pts " | cut -d " " -f 2)

    case $1 in
        "up")
            try_select_pane_in_direction top U $session_id
            exit $?
            ;;
        "down")
            try_select_pane_in_direction bottom D $session_id
            exit $?
            ;;
        "left")
            try_select_pane_in_direction left L $session_id
            exit $?
            ;;
        "right")
            try_select_pane_in_direction right R $session_id
            exit $?
            ;;
        *)
            echo "Invalid direction $1"
            exit 1
            ;;
    esac
fi

exit 1
