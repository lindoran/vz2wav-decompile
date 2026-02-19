# --- Compilers ---
CC      = gcc
CC_DOS = i686-w64-mingw32-gcc


# --- Flags ---
CFLAGS  = -Wall -Wextra -O2 -std=c99
LDFLAGS = -lm

# --- Targets ---
LINUX_TARGETS = vz2wav wav2vz text2bas-vz text2bas-cg
DOS_TARGETS   = vz2wav.exe wav2vz.exe text2bas-vz.exe text2bas-cg.exe

all: linux dos

# -------------------------
# Linux builds
# -------------------------
linux: $(LINUX_TARGETS)

vz2wav: vz2wav.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

wav2vz: wav2vz.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

text2bas-vz: text2bas.c
	$(CC) $(CFLAGS) -DCGENIE=0 -o $@ $<

text2bas-cg: text2bas.c
	$(CC) $(CFLAGS) -DCGENIE=1 -o $@ $<

# -------------------------
# DOS / MinGW builds
# -------------------------
dos: $(DOS_TARGETS)

vz2wav.exe: vz2wav.c
	$(CC_DOS) $(CFLAGS) -o $@ $< $(LDFLAGS)

wav2vz.exe: wav2vz.c
	$(CC_DOS) $(CFLAGS) -o $@ $< $(LDFLAGS)

text2bas-vz.exe: text2bas.c
	$(CC_DOS) $(CFLAGS) -DCGENIE=0 -o $@ $<

text2bas-cg.exe: text2bas.c
	$(CC_DOS) $(CFLAGS) -DCGENIE=1 -o $@ $<

# -------------------------
# Packaging DOS binaries
# -------------------------
package: dos
	mkdir -p dist
	mv $(DOS_TARGETS) dist/
	cd dist && zip vztools-dos.zip $(DOS_TARGETS)

# -------------------------
# Install Linux binaries
# -------------------------
install: linux
	install -m 755 vz2wav /usr/local/bin/
	install -m 755 wav2vz /usr/local/bin/
	install -m 755 text2bas-vz /usr/local/bin/
	install -m 755 text2bas-cg /usr/local/bin/

uninstall:
	rm -f /usr/local/bin/vz2wav \
		   /usr/local/bin/wav2vz \
		   /usr/local/bin/text2bas-vz \
		   /usr/local/bin/text2bas-cg

clean:
	rm -f $(LINUX_TARGETS) $(DOS_TARGETS)
	rm -rf dist

.PHONY: all linux dos clean install uninstall package
