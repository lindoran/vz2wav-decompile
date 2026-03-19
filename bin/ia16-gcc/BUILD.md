BUILD INSTRUCTIONS FOR VZ2WAV TOOLS
===================================

This document explains how to build the Linux, Windows, and DOS versions
of the vz2wav toolchain using modern cross‑compilers on Ubuntu/Kubuntu.

The project supports three architectures:

  Linux    ->  ia64-linux     (GCC)
  Windows  ->  mingw-ia32     (MinGW-w64)
  DOS      ->  ia16-gcc       (ia16-elf-gcc)

All builds are driven by the unified Makefile.


1. INSTALL BUILD DEPENDENCIES
-----------------------------

1.1 Linux Native Compiler (GCC)
-------------------------------
Ubuntu includes GCC by default, but ensure it is installed:

    sudo apt update
    sudo apt install build-essential


1.2 Windows Cross‑Compiler (MinGW‑w64)
--------------------------------------
Install the 32‑bit MinGW‑w64 toolchain:

    sudo apt install mingw-w64

The Makefile expects:

    i686-w64-mingw32-gcc

Verify installation:

    i686-w64-mingw32-gcc --version


1.3 DOS Compiler (ia16‑elf‑gcc)
-------------------------------
The DOS version is built using ia16‑elf‑gcc, a modern GCC backend for
16‑bit real‑mode x86.

These instructions follow the official Launchpad PPA:

    https://launchpad.net/~tkchia/+archive/ubuntu/build-ia16/

Add the PPA:

    sudo add-apt-repository ppa:tkchia/build-ia16
    sudo apt update

Install the toolchain:

    sudo apt install ia16-elf-gcc ia16-elf-binutils

Verify installation:

    ia16-elf-gcc --version


2. BUILD INSTRUCTIONS
---------------------

The Makefile supports the following build targets:

    make         -> build all architectures
    make linux   -> build Linux binaries
    make windows -> build Windows binaries
    make dos     -> build DOS binaries (ia16-gcc)

Build everything:

    make

Build only Linux:

    make linux

Build only Windows:

    make windows

Build only DOS:

    make dos


3. PACKAGING
------------

To generate .zip archives for each architecture:

    make package

This produces:

    dist/linux.zip
    dist/windows.zip
    dist/dos.zip


4. CLEANING
-----------

Remove all compiled binaries:

    make clean

Remove everything including dist packages:

    make clean_all


5. MAKEFILE STRUCTURE OVERVIEW
------------------------------

The Makefile is organized into three major build sections:

Linux (GCC)
-----------
- Compiles native ELF binaries
- Output: bin/ia64-linux/
- Targets:
    vz2wav
    wav2vz
    text2bas-vz
    text2bas-cg

Windows (MinGW‑ia32)
--------------------
- Cross‑compiles 32‑bit Windows .exe files
- Output: bin/mingw-ia32/
- Targets:
    vz2wav.exe
    wav2vz.exe
    text2bas-vz.exe
    text2bas-cg.exe

DOS (ia16‑elf‑gcc)
------------------
- Builds real‑mode DOS .exe files
- Uses -mcmodel=small
- Output: bin/ia16-gcc/
- Targets:
    vz2wav.exe
    wav2vz.exe
    text2bas-vz.exe
    text2bas-cg.exe

Packaging
---------
Each architecture’s output directory is zipped into dist/.

Cleaning
--------
make clean      -> removes bin/
make clean_all  -> removes bin/ and dist/


6. SUMMARY
----------

Once dependencies are installed:

    make

builds all targets.

    make package

creates distributable archives.

The Makefile handles directory creation, compiler invocation,
architecture separation, and packaging. Everything is reproducible and
cross‑platform.
