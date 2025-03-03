#!/usr/bin/env fish

set SESSION_NAME "pmx-console"
tmux new-session -d -s $SESSION_NAME
tmux split-window -h
tmux select-pane -t 0
tmux send-keys 'pmx-console' C-m
tmux select-pane -t 1
tmux send-keys 'journalctl --user -u wireplumber -f' C-m
tmux attach-session -t $SESSION_NAME