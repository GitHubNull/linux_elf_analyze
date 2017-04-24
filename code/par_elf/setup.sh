#!/bin/sh
chmod +x scripts/*
cd scripts/
sudo ./insert_model.sh \
    && gnome-terminal -t "kernel_model output" -e "./kernel_output.sh"
    # & gnome-terminal -t "client_output" -x "../build/user/app"
    # gnome-terminal -t "client_output" -e "./start_client.sh"
cd -
