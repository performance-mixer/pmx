#!/bin/sh
systemctl enable dbus
systemctl start dbus
systemctl enable --user pipewire
systemctl start --user pipewire
systemctl enable --user wireplumber
systemctl start --user wireplumber
