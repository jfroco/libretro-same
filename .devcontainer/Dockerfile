FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# Based on libretro_sample_core's devcontainer, without quasi-msys2 and Wine:
# MAME builds its libraries from 3rdparty/, and the quasi-msys2 environment
# (OS=Windows_NT, MSYSTEM=MINGW64, CC=win-gcc) makes MAME's makefile think it
# runs on Windows.

# 1. Build tools, Python (used by MAME's build scripts), the MinGW-w64 compiler
#    (posix threading model, needed by MAME) and the Linux aarch64 cross compiler.
#    The cross compilers' runtime and glibc match Ubuntu's: glibc 2.39 on 24.04.
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    ca-certificates \
    file \
    git \
    make \
    python3 \
    gcc-mingw-w64-x86-64-posix \
    g++-mingw-w64-x86-64-posix \
    binutils-mingw-w64-x86-64 \
    gcc-aarch64-linux-gnu \
    g++-aarch64-linux-gnu \
    binutils-aarch64-linux-gnu \
    && rm -rf /var/lib/apt/lists/*

# 2. SDL2 headers for MinGW (headers only; the core does not link SDL).
#    On Windows the libretro OSD defines SDLMAME_WIN32, and
#    src/osd/modules/input/input_common.cpp then includes <SDL2/SDL.h> for the
#    keyboard scancodes. Copying them into the MinGW sysroot makes the compiler
#    find them without extra flags.
ARG SDL2_VERSION=2.30.9
ADD https://github.com/libsdl-org/SDL/releases/download/release-${SDL2_VERSION}/SDL2-devel-${SDL2_VERSION}-mingw.tar.gz /tmp/sdl2.tar.gz
RUN tar -xzf /tmp/sdl2.tar.gz -C /tmp && \
    cp -r /tmp/SDL2-${SDL2_VERSION}/x86_64-w64-mingw32/include/SDL2 /usr/x86_64-w64-mingw32/include/ && \
    rm -rf /tmp/sdl2.tar.gz /tmp/SDL2-${SDL2_VERSION}

# Keep `git` commands in the build from failing: the mounted repo belongs to a
# user other than root.
RUN git config --global --add safe.directory '*'

WORKDIR /workspaces
