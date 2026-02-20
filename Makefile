# ============================
#   Compiler Selection
# ============================
CC_LINUX = gcc
CC_WIN   = i686-w64-mingw32-gcc
IA16     = ia16-elf-gcc

CFLAGS   = -Wall -Wextra -O2 -std=c99
LDFLAGS  = -lm

# ============================
#   Output Directories
# ============================
BIN_LINUX    = bin/ia64-linux
BIN_WIN      = bin/mingw-ia32
BIN_DOS_GCC  = bin/ia16-gcc

DIST_DIR = dist

PROGRAMS = vz2wav wav2vz text2bas-vz text2bas-cg

all: linux windows dos

# ============================
#   Linux Builds
# ============================
linux: $(BIN_LINUX) \
       $(BIN_LINUX)/vz2wav \
       $(BIN_LINUX)/wav2vz \
       $(BIN_LINUX)/text2bas-vz \
       $(BIN_LINUX)/text2bas-cg

$(BIN_LINUX):
	mkdir -p $(BIN_LINUX)

$(BIN_LINUX)/vz2wav: vz2wav.c
	$(CC_LINUX) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(BIN_LINUX)/wav2vz: wav2vz.c
	$(CC_LINUX) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(BIN_LINUX)/text2bas-vz: text2bas.c
	$(CC_LINUX) $(CFLAGS) -DCGENIE=0 -o $@ $<

$(BIN_LINUX)/text2bas-cg: text2bas.c
	$(CC_LINUX) $(CFLAGS) -DCGENIE=1 -o $@ $<

# ============================
#   Windows (MinGW) Builds
# ============================
windows: $(BIN_WIN) \
		 $(BIN_WIN)/vz2wav.exe \
		 $(BIN_WIN)/wav2vz.exe \
		 $(BIN_WIN)/text2bas-vz.exe \
		 $(BIN_WIN)/text2bas-cg.exe

$(BIN_WIN):
	mkdir -p $(BIN_WIN)

$(BIN_WIN)/vz2wav.exe: vz2wav.c
	$(CC_WIN) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(BIN_WIN)/wav2vz.exe: wav2vz.c
	$(CC_WIN) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(BIN_WIN)/text2bas-vz.exe: text2bas.c
	$(CC_WIN) $(CFLAGS) -DCGENIE=0 -o $@ $<

$(BIN_WIN)/text2bas-cg.exe: text2bas.c
	$(CC_WIN) $(CFLAGS) -DCGENIE=1 -o $@ $<

# ============================
#   DOS Builds (ia16 only)
# ============================
dos: $(BIN_DOS_GCC) \
	 $(BIN_DOS_GCC)/vz2wav.exe \
	 $(BIN_DOS_GCC)/wav2vz.exe \
	 $(BIN_DOS_GCC)/text2bas-vz.exe \
	 $(BIN_DOS_GCC)/text2bas-cg.exe

$(BIN_DOS_GCC):
	mkdir -p $(BIN_DOS_GCC)

$(BIN_DOS_GCC)/vz2wav.exe: vz2wav.c
	$(IA16) -mcmodel=small -o $@ $<

$(BIN_DOS_GCC)/wav2vz.exe: wav2vz.c
	$(IA16) -mcmodel=small -o $@ $<

$(BIN_DOS_GCC)/text2bas-vz.exe: text2bas.c
	$(IA16) -mcmodel=small -DCGENIE=0 -o $@ $<

$(BIN_DOS_GCC)/text2bas-cg.exe: text2bas.c
	$(IA16) -mcmodel=small -DCGENIE=1 -o $@ $<
# ============================
#   Packaging
# ============================
package: all
	mkdir -p $(DIST_DIR)

	# Linux package
	cd bin/ia64-linux && cp ../../README.md . && zip -r ../../dist/linux.zip . && rm -f README.md

	# Windows package
	cd bin/mingw-ia32 && cp ../../README.md . && zip -r ../../dist/windows.zip . && rm -f README.md

	# DOS package
	cd bin/ia16-gcc && cp ../../README.md . && zip -r ../../dist/dos.zip . && rm -f README.md


# ============================
#   Cleaning
# ============================
clean:
	rm -rf bin/*

clean_all:
	rm -rf bin/*
	rm -rf dist/*

.PHONY: all linux windows dos package clean clean_all
