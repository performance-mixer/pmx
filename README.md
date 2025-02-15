# Performance Mixer

A micro-app based mixer build with pipewire, wireplumber and a bunch of sweat
and love.

## Installation

Performance mixer uses meson to compile and install the services from source.
There also is a [PKGBUILD](https://github.com/performance-mixer/pmx-git-arch)
file for arch linux available.

### Install from Source

Performance Mixer requires the following dependencies:

- pipewire,
- wireplumber,
- boost,
- grpc,
- protobuf,
- yaml-cpp, and
- libsystemd.

Install them using the package manager of your distribution.

This repository contains all the applications and scripts necessary to run
performance mixer, but it needs another, related repository to build. Check both
repositories out next to each other in one directory.

To compile the application, checkout the source code for both applications first, enter the git
repository for `pmx`, set up a build directory, and compile.

```bash
git clone https://github.com/performance-mixer/pwcpp
git clone https://github.com/performance-mixer/pmx-grpc
git clone https://github.com/performance-mixer/pmx
cd pmx
meson setup build
cd build
meson compile
```

If everything is successful, you can install the application using:

```bash
meson install
```