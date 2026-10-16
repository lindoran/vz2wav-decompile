BUILD INSTRUCTIONS FOR VZ2WAV TOOLS
===================================

This document explains how to build the Linux, Windows, and DOS versions
of the vz2wav toolchain using modern cross-compilers on Ubuntu/Kubuntu.

You can also build for free under Windows 11 using WSL (see section 1.4)

The project supports four architectures:

  Linux      -> ia64-linux    (GCC)
  Windows32  -> mingw-ia32    (MinGW-w64)
  Windows64  -> mingw-ia64    (MinGW-w64)
  DOS        -> ia16-gcc      (ia16-elf-gcc)

All builds are driven by the unified Makefile.

Versioning is centralized in the repository `VERSION` file. The Makefile
injects this into all binaries, and each tool reports it via `--version`
or `-V`.


1. INSTALL BUILD DEPENDENCIES
-----------------------------

1.1 Linux Native Compiler (GCC)
-------------------------------
Ubuntu includes GCC by default, but ensure it is installed:

    sudo apt update
    sudo apt install build-essential


1.2 Windows Cross-Compilers (MinGW-w64)
----------------------------------------
Install MinGW-w64:

    sudo apt install mingw-w64

The Makefile expects:

    i686-w64-mingw32-gcc
    x86_64-w64-mingw32-gcc

Verify installation:

    i686-w64-mingw32-gcc --version
    x86_64-w64-mingw32-gcc --version


1.3 DOS Compiler (ia16-elf-gcc)
-------------------------------
The DOS version is built using ia16-elf-gcc, a modern GCC backend for
16-bit real-mode x86.

These instructions follow the official Launchpad PPA:

    https://launchpad.net/~tkchia/+archive/ubuntu/build-ia16/

Add the PPA:

    sudo add-apt-repository ppa:tkchia/build-ia16
    sudo apt update

Install the toolchain:

    sudo apt install ia16-elf-gcc ia16-elf-binutils

Verify installation:

    ia16-elf-gcc --version


1.4 Building On Windows With WSL (Recommended)
----------------------------------------------
If you are on Windows, using WSL is often simpler than setting up native
Windows MinGW toolchains manually. You can use Ubuntu in WSL and run the
same build commands as Linux.

Typical flow:

1) Install WSL + Ubuntu (PowerShell as Administrator):

    wsl --install -d Ubuntu

2) Open Ubuntu (WSL), then install dependencies:

    sudo apt update
    sudo apt install build-essential mingw-w64

3) Clone/open this repo inside WSL and build:

    make windows-all

Windows binaries will be produced in:

    bin/mingw-ia32/
    bin/mingw-ia64/

Tip:
- Building inside the Linux filesystem (for example under `~/`) is usually
  faster than building directly under `/mnt/c/...`.
- The same WSL setup can also build and test Linux and DOS/ia16 targets.
  Install the ia16 toolchain in WSL (section 1.3), then use:

    make linux
    make dos
    make build-all


2. BUILD INSTRUCTIONS
---------------------

The Makefile supports the following build targets:

    make           -> build Linux binaries (default)
    make linux     -> build Linux binaries
    make windows   -> build Windows 32-bit binaries
    make windows64 -> build Windows 64-bit binaries
    make windows-all -> build both Windows 32-bit and 64-bit binaries
    make dos       -> build DOS binaries (ia16-gcc)
    make build-all -> build all architectures

Build only Linux (default):

    make

Build only Linux:

    make linux

Build only Windows 32-bit:

    make windows

Build only Windows 64-bit:

    make windows64

Build both Windows variants:

    make windows-all

Build only DOS:

    make dos

Build everything:

    make build-all


3. PACKAGING
------------

To generate .zip archives for each architecture:

    make package-all

This produces:

    dist/linux.zip
    dist/windows.zip
    dist/windows64.zip
    dist/dos.zip


4. INSTALLATION (LINUX)
-----------------------

Install Linux binaries and docs into `/usr/local`:

    make install

Use a custom prefix:

    make install PREFIX=/opt/vz2wav

Stage into a package root (for packaging workflows):

    make install DESTDIR=/tmp/vz2wav-pkg PREFIX=/usr

Remove installed files:

    make uninstall


5. CLEANING
-----------

Remove all compiled binaries and packages:

    make clean

Remove compiled binaries only:

    make clean-bins

Remove distribution archives only:

    make clean-dist

Backward-compatible alias:

    make clean_all


6. MAKEFILE STRUCTURE OVERVIEW
------------------------------

The Makefile is organized into four major build sections:

Linux (GCC)
-----------
- Compiles native ELF binaries
- Output: bin/ia64-linux/
- Targets:
    vz2wav
    wav2vz
    text2bas-vz
    text2bas-cg
    vzexport
    vzpack

Windows 32-bit (MinGW-ia32)
---------------------------
- Cross-compiles 32-bit Windows .exe files
- Output: bin/mingw-ia32/
- Targets:
    vz2wav.exe
    wav2vz.exe
    text2bas-vz.exe
    text2bas-cg.exe
    vzexport.exe
    vzpack.exe

Windows 64-bit (MinGW-ia64)
---------------------------
- Cross-compiles 64-bit Windows .exe files
- Output: bin/mingw-ia64/
- Targets:
    vz2wav.exe
    wav2vz.exe
    text2bas-vz.exe
    text2bas-cg.exe
    vzexport.exe
    vzpack.exe

DOS (ia16-elf-gcc)
------------------
- Builds real-mode DOS .exe files
- Uses -mcmodel=small
- Output: bin/ia16-gcc/
- Targets:
    vz2wav.exe
    wav2vz.exe
    text2bas-vz.exe
    text2bas-cg.exe
    vzexport.exe
    vzpack.exe

Packaging
---------
Each architecture output directory is zipped into dist/.


7. SUMMARY
----------

Once dependencies are installed:

    make build-all

builds all targets.

    make package-all

creates distributable archives.

The Makefile handles directory creation, compiler invocation,
architecture separation, and packaging.
