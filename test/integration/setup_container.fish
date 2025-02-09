#!/bin/fish
mkdir -p ~/Development/testing/containers/$argv[1]
sudo pacstrap -c ~/Development/testing/containers/$argv[1] base base-devel systemd \
    pipewire wireplumber boost grpc protobuf yaml-cpp libsystemd dbus fish git \
    meson neovim pipewire-audio pipewire-jack calf jalv dpf-plugins-lv2

sudo systemd-nspawn -D ~/Development/testing/containers/$argv[1] \
    /usr/bin/fish -c "useradd -m -G wheel -s /usr/bin/fish pmx"
sudo systemd-nspawn -D ~/Development/testing/containers/$argv[1] \
    /usr/bin/fish -c "passwd -d pmx"
sudo systemd-nspawn -D ~/Development/testing/containers/$argv[1] \
    /usr/bin/fish -c 'echo "%wheel ALL=(ALL:ALL) NOPASSWD: ALL" >> /etc/sudoers'
sudo systemd-nspawn -D ~/Development/testing/containers/$argv[1] \
    /usr/bin/fish -c "passwd -d root"

sudo systemd-nspawn -D ~/Development/testing/containers/$argv[1] -u pmx --chdir=/home/pmx \
    /usr/bin/fish -c "git clone https://github.com/performance-mixer/pmx-git-arch"

sudo systemd-nspawn -D ~/Development/testing/containers/$argv[1] -u pmx --chdir=/home/pmx/pmx-git-arch \
    /usr/bin/fish -c makepkg

sudo systemd-nspawn -D ~/Development/testing/containers/$argv[1] -u pmx --chdir=/home/pmx/pmx-git-arch \
    /usr/bin/fish -c "sudo pacman -U pmx-git-0.0.1-1-x86_64.pkg.tar.zst --noconfirm"

sudo systemd-nspawn -D ~/Development/testing/containers/$argv[1] -u pmx --chdir=/home/pmx/pmx-git-arch \
    /usr/bin/fish -c "systemctl --user enable dbus"
sudo systemd-nspawn -D ~/Development/testing/containers/$argv[1] -u pmx --chdir=/home/pmx/pmx-git-arch \
    /usr/bin/fish -c "systemctl --user enable pipewire"
sudo systemd-nspawn -D ~/Development/testing/containers/$argv[1] -u pmx --chdir=/home/pmx/pmx-git-arch \
    /usr/bin/fish -c "systemctl --user enable wireplumber"

sudo cp ./assertions/* ~/Development/testing/containers/$argv[1]/home/pmx
sudo systemd-nspawn -D ~/Development/testing/containers/$argv[1] --chdir=/home/pmx \
    /usr/bin/fish -c "chown pmx *.lua"
