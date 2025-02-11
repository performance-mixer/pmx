#!/bin/fish
sudo cp ../../wireplumber/*.lua \
    ~/Development/testing/containers/$argv[1]/usr/share/wireplumber/scripts/pmx

sudo cp ../../wireplumber/*.lua \
    ~/Development/testing/containers/$argv[1]/usr/share/lua/5.4

sudo cp ./assertions/* ~/Development/testing/containers/$argv[1]/home/pmx
