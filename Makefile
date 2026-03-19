# =============================================================================
# VZ2WAV / WAV2VZ RECONSTRUCTION PROJECT - Makefile
#
# Targets:
#   make             -> linux (default)
#   make linux       -> GCC/Linux
#   make windows     -> MinGW 32-bit (i686-w64-mingw32-gcc)
#   make windows64   -> MinGW 64-bit (x86_64-w64-mingw32-gcc)
#   make windows-all -> both MinGW 32-bit and 64-bit
#   make dos         -> ia16-elf-gcc (16-bit DOS)
#   make build-all   -> linux, windows, windows64, dos
#   make package-all -> zip all builds (assumes built)
#   make install     -> install Linux binaries/docs to PREFIX
#   make uninstall   -> remove installed Linux binaries/docs from PREFIX
#   make clean       -> remove bin/ and dist/
#   make clean-bins  -> remove bin/ only
#   make clean-dist  -> remove dist/ only
#   make help        -> show this message
# =============================================================================

# ============================
#   Compiler Selection
# ============================
CC_LINUX = gcc
CC_WIN   = i686-w64-mingw32-gcc
CC_WIN64 = x86_64-w64-mingw32-gcc
IA16     = ia16-elf-gcc

CFLAGS   = -Wall -Wextra -O2 -std=c99
LDFLAGS  = -lm
VERSION_FILE ?= VERSION
PROJECT_VERSION := $(strip $(shell cat $(VERSION_FILE) 2>/dev/null || echo 0.0.0-dev))
CPPFLAGS += -DTOOL_VERSION=\"$(PROJECT_VERSION)\"

PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
DOCDIR ?= $(PREFIX)/share/doc/vz2wav
INSTALL ?= install

# ============================
#   Output Directories
# ============================
BIN_DIR     = bin
DIST_DIR    = dist

BIN_LINUX    = $(BIN_DIR)/ia64-linux
BIN_WIN      = $(BIN_DIR)/mingw-ia32
BIN_WIN64    = $(BIN_DIR)/mingw-ia64
BIN_DOS_GCC  = $(BIN_DIR)/ia16-gcc

PROGRAMS = vz2wav wav2vz text2bas-vz text2bas-cg vzexport vzpack
LINUX_PROGRAM_TARGETS = \
	$(BIN_LINUX)/vz2wav \
	$(BIN_LINUX)/wav2vz \
	$(BIN_LINUX)/text2bas-vz \
	$(BIN_LINUX)/text2bas-cg \
	$(BIN_LINUX)/vzexport \
	$(BIN_LINUX)/vzpack

# =============================================================================
.PHONY: all linux windows windows64 windows-all dos build-all package-all \
        package-linux package-windows package-windows64 package-dos \
        install uninstall \
        clean clean-bins clean-dist clean_all help dirs

# Default: linux only
all: linux

# =============================================================================
# Directories
# =============================================================================

dirs:
	mkdir -p $(BIN_DIR) $(DIST_DIR)

# =============================================================================
# Shared documentation staging helper.
# Usage: $(call stage-docs,$(BIN_LINUX))
# =============================================================================

define stage-docs
	@if [ -f README.md ]; then cp README.md $(1); fi
	@if [ -f BUILD.md ]; then cp BUILD.md $(1); fi
	@if [ -f TEXT2BAS.md ]; then cp TEXT2BAS.md $(1); fi
	@if [ -f TODO.md ]; then cp TODO.md $(1); fi
	@if [ -f UNLICENSE ]; then cp UNLICENSE $(1); fi
	@if [ -f NOTICE ]; then cp NOTICE $(1); fi
endef

# =============================================================================
# Linux Builds
# =============================================================================
linux: dirs $(BIN_LINUX) $(LINUX_PROGRAM_TARGETS)
	@echo "Built: $(BIN_LINUX)/vz2wav"
	@echo "Built: $(BIN_LINUX)/wav2vz"
	@echo "Built: $(BIN_LINUX)/text2bas-vz"
	@echo "Built: $(BIN_LINUX)/text2bas-cg"
	@echo "Built: $(BIN_LINUX)/vzexport"
	@echo "Built: $(BIN_LINUX)/vzpack"
	$(MAKE) package-linux

$(BIN_LINUX):
	mkdir -p $(BIN_LINUX)

$(BIN_LINUX)/vz2wav: vz2wav.c
	$(CC_LINUX) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(BIN_LINUX)/wav2vz: wav2vz.c
	$(CC_LINUX) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(BIN_LINUX)/text2bas-vz: text2bas.c
	$(CC_LINUX) $(CPPFLAGS) $(CFLAGS) -DCGENIE=0 -o $@ $<

$(BIN_LINUX)/text2bas-cg: text2bas.c
	$(CC_LINUX) $(CPPFLAGS) $(CFLAGS) -DCGENIE=1 -o $@ $<

$(BIN_LINUX)/vzexport: vzexport.c
	$(CC_LINUX) $(CPPFLAGS) $(CFLAGS) -o $@ $<

$(BIN_LINUX)/vzpack: vzpack.c
	$(CC_LINUX) $(CPPFLAGS) $(CFLAGS) -o $@ $<

# =============================================================================
# Windows (MinGW) Builds
# =============================================================================
windows: dirs $(BIN_WIN) \
		 $(BIN_WIN)/vz2wav.exe \
		 $(BIN_WIN)/wav2vz.exe \
		 $(BIN_WIN)/text2bas-vz.exe \
		 $(BIN_WIN)/text2bas-cg.exe \
		 $(BIN_WIN)/vzexport.exe \
		 $(BIN_WIN)/vzpack.exe
	@echo "Built: $(BIN_WIN)/vz2wav.exe"
	@echo "Built: $(BIN_WIN)/wav2vz.exe"
	@echo "Built: $(BIN_WIN)/text2bas-vz.exe"
	@echo "Built: $(BIN_WIN)/text2bas-cg.exe"
	@echo "Built: $(BIN_WIN)/vzexport.exe"
	@echo "Built: $(BIN_WIN)/vzpack.exe"
	$(MAKE) package-windows

$(BIN_WIN):
	mkdir -p $(BIN_WIN)

$(BIN_WIN)/vz2wav.exe: vz2wav.c
	$(CC_WIN) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(BIN_WIN)/wav2vz.exe: wav2vz.c
	$(CC_WIN) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(BIN_WIN)/text2bas-vz.exe: text2bas.c
	$(CC_WIN) $(CPPFLAGS) $(CFLAGS) -DCGENIE=0 -o $@ $<

$(BIN_WIN)/text2bas-cg.exe: text2bas.c
	$(CC_WIN) $(CPPFLAGS) $(CFLAGS) -DCGENIE=1 -o $@ $<

$(BIN_WIN)/vzexport.exe: vzexport.c
	$(CC_WIN) $(CPPFLAGS) $(CFLAGS) -o $@ $<

$(BIN_WIN)/vzpack.exe: vzpack.c
	$(CC_WIN) $(CPPFLAGS) $(CFLAGS) -o $@ $<

# =============================================================================
# Windows 64-bit (MinGW) Builds
# =============================================================================
windows64: dirs $(BIN_WIN64) \
		   $(BIN_WIN64)/vz2wav.exe \
		   $(BIN_WIN64)/wav2vz.exe \
		   $(BIN_WIN64)/text2bas-vz.exe \
		   $(BIN_WIN64)/text2bas-cg.exe \
		   $(BIN_WIN64)/vzexport.exe \
		   $(BIN_WIN64)/vzpack.exe
	@echo "Built: $(BIN_WIN64)/vz2wav.exe"
	@echo "Built: $(BIN_WIN64)/wav2vz.exe"
	@echo "Built: $(BIN_WIN64)/text2bas-vz.exe"
	@echo "Built: $(BIN_WIN64)/text2bas-cg.exe"
	@echo "Built: $(BIN_WIN64)/vzexport.exe"
	@echo "Built: $(BIN_WIN64)/vzpack.exe"
	$(MAKE) package-windows64

$(BIN_WIN64):
	mkdir -p $(BIN_WIN64)

$(BIN_WIN64)/vz2wav.exe: vz2wav.c
	$(CC_WIN64) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(BIN_WIN64)/wav2vz.exe: wav2vz.c
	$(CC_WIN64) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(BIN_WIN64)/text2bas-vz.exe: text2bas.c
	$(CC_WIN64) $(CPPFLAGS) $(CFLAGS) -DCGENIE=0 -o $@ $<

$(BIN_WIN64)/text2bas-cg.exe: text2bas.c
	$(CC_WIN64) $(CPPFLAGS) $(CFLAGS) -DCGENIE=1 -o $@ $<

$(BIN_WIN64)/vzexport.exe: vzexport.c
	$(CC_WIN64) $(CPPFLAGS) $(CFLAGS) -o $@ $<

$(BIN_WIN64)/vzpack.exe: vzpack.c
	$(CC_WIN64) $(CPPFLAGS) $(CFLAGS) -o $@ $<

# Convenience target: build/package both Windows variants.
windows-all: windows windows64

# =============================================================================
# DOS Builds (ia16 only)
# =============================================================================
dos: dirs $(BIN_DOS_GCC) \
	 $(BIN_DOS_GCC)/vz2wav.exe \
	 $(BIN_DOS_GCC)/wav2vz.exe \
	 $(BIN_DOS_GCC)/text2bas-vz.exe \
	 $(BIN_DOS_GCC)/text2bas-cg.exe \
	 $(BIN_DOS_GCC)/vzexport.exe \
	 $(BIN_DOS_GCC)/vzpack.exe
	@echo "Built: $(BIN_DOS_GCC)/vz2wav.exe"
	@echo "Built: $(BIN_DOS_GCC)/wav2vz.exe"
	@echo "Built: $(BIN_DOS_GCC)/text2bas-vz.exe"
	@echo "Built: $(BIN_DOS_GCC)/text2bas-cg.exe"
	@echo "Built: $(BIN_DOS_GCC)/vzexport.exe"
	@echo "Built: $(BIN_DOS_GCC)/vzpack.exe"
	$(MAKE) package-dos

$(BIN_DOS_GCC):
	mkdir -p $(BIN_DOS_GCC)

$(BIN_DOS_GCC)/vz2wav.exe: vz2wav.c
	$(IA16) $(CPPFLAGS) -mcmodel=small -o $@ $<

$(BIN_DOS_GCC)/wav2vz.exe: wav2vz.c
	$(IA16) $(CPPFLAGS) -mcmodel=small -o $@ $<

$(BIN_DOS_GCC)/text2bas-vz.exe: text2bas.c
	$(IA16) $(CPPFLAGS) -mcmodel=small -DCGENIE=0 -o $@ $<

$(BIN_DOS_GCC)/text2bas-cg.exe: text2bas.c
	$(IA16) $(CPPFLAGS) -mcmodel=small -DCGENIE=1 -o $@ $<

$(BIN_DOS_GCC)/vzexport.exe: vzexport.c
	$(IA16) $(CPPFLAGS) -mcmodel=small -o $@ $<

$(BIN_DOS_GCC)/vzpack.exe: vzpack.c
	$(IA16) $(CPPFLAGS) -mcmodel=small -o $@ $<

# =============================================================================
# Packaging rules
# =============================================================================
package-linux: dirs
	@echo "Packaging Linux build..."
	$(call stage-docs,$(BIN_LINUX))
	cd $(BIN_LINUX) && zip -qr ../../$(DIST_DIR)/linux.zip .

package-windows: dirs
	@echo "Packaging Windows build..."
	$(call stage-docs,$(BIN_WIN))
	cd $(BIN_WIN) && zip -qr ../../$(DIST_DIR)/windows.zip .

package-windows64: dirs
	@echo "Packaging Windows 64-bit build..."
	$(call stage-docs,$(BIN_WIN64))
	cd $(BIN_WIN64) && zip -qr ../../$(DIST_DIR)/windows64.zip .

package-dos: dirs
	@echo "Packaging DOS build..."
	$(call stage-docs,$(BIN_DOS_GCC))
	cd $(BIN_DOS_GCC) && zip -qr ../../$(DIST_DIR)/dos.zip .

# =============================================================================
# Meta-targets
# =============================================================================
build-all: linux windows windows64 dos

package-all: dirs package-linux package-windows package-windows64 package-dos

# =============================================================================
# Install / Uninstall (Linux artifacts)
# =============================================================================
install: dirs $(BIN_LINUX) $(LINUX_PROGRAM_TARGETS)
	@echo "Installing binaries to $(DESTDIR)$(BINDIR)"
	$(INSTALL) -d "$(DESTDIR)$(BINDIR)"
	@for prog in $(PROGRAMS); do \
		$(INSTALL) -m 0755 "$(BIN_LINUX)/$$prog" "$(DESTDIR)$(BINDIR)/$$prog"; \
	done
	@echo "Installing docs to $(DESTDIR)$(DOCDIR)"
	$(INSTALL) -d "$(DESTDIR)$(DOCDIR)"
	@if [ -f README.md ]; then $(INSTALL) -m 0644 README.md "$(DESTDIR)$(DOCDIR)/README.md"; fi
	@if [ -f BUILD.md ]; then $(INSTALL) -m 0644 BUILD.md "$(DESTDIR)$(DOCDIR)/BUILD.md"; fi
	@if [ -f TEXT2BAS.md ]; then $(INSTALL) -m 0644 TEXT2BAS.md "$(DESTDIR)$(DOCDIR)/TEXT2BAS.md"; fi
	@if [ -f TODO.md ]; then $(INSTALL) -m 0644 TODO.md "$(DESTDIR)$(DOCDIR)/TODO.md"; fi
	@if [ -f UNLICENSE ]; then $(INSTALL) -m 0644 UNLICENSE "$(DESTDIR)$(DOCDIR)/UNLICENSE"; fi
	@if [ -f NOTICE ]; then $(INSTALL) -m 0644 NOTICE "$(DESTDIR)$(DOCDIR)/NOTICE"; fi

uninstall:
	@echo "Removing binaries from $(DESTDIR)$(BINDIR)"
	@for prog in $(PROGRAMS); do \
		rm -f "$(DESTDIR)$(BINDIR)/$$prog"; \
	done
	@echo "Removing docs from $(DESTDIR)$(DOCDIR)"
	@rm -f "$(DESTDIR)$(DOCDIR)/README.md" "$(DESTDIR)$(DOCDIR)/BUILD.md" "$(DESTDIR)$(DOCDIR)/TEXT2BAS.md" "$(DESTDIR)$(DOCDIR)/TODO.md" "$(DESTDIR)$(DOCDIR)/UNLICENSE" "$(DESTDIR)$(DOCDIR)/NOTICE"
	@rmdir --ignore-fail-on-non-empty "$(DESTDIR)$(DOCDIR)" 2>/dev/null || true

# =============================================================================
# Cleaning
# =============================================================================
clean:
	rm -rf $(BIN_DIR) $(DIST_DIR)

clean-bins:
	rm -rf $(BIN_DIR)

clean-dist:
	rm -rf $(DIST_DIR)

# Backward-compatible alias
clean_all: clean

# =============================================================================
# Help
# =============================================================================
help:
	@echo "VZ2WAV / WAV2VZ Reconstruction Project - Build Targets"
	@echo ""
	@echo "Builds:"
	@echo "  make linux       - GCC/Linux"
	@echo "  make windows     - MinGW 32-bit Windows"
	@echo "  make windows64   - MinGW 64-bit Windows"
	@echo "  make windows-all - Build both Windows targets"
	@echo "  make dos         - ia16-elf-gcc DOS"
	@echo "  make build-all   - All of the above"
	@echo ""
	@echo "Packaging:"
	@echo "  make package-all - Zip all builds (assumes built)"
	@echo ""
	@echo "Install:"
	@echo "  make install     - Install Linux binaries/docs to PREFIX ($(PREFIX))"
	@echo "  make uninstall   - Remove installed Linux binaries/docs from PREFIX ($(PREFIX))"
	@echo ""
	@echo "Cleaning:"
	@echo "  make clean       - Remove bin/ and dist/"
	@echo "  make clean-bins  - Remove bin/ only"
	@echo "  make clean-dist  - Remove dist/ only"
