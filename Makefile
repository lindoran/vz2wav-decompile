# =============================================================================
# VZ2WAV / WAV2VZ RECONSTRUCTION PROJECT - Makefile
#
# Targets:
#   make             -> linux (default)
#   make linux       -> GCC/Linux
#   make windows     -> MinGW 32-bit (i686-w64-mingw32-gcc)
#   make dos         -> ia16-elf-gcc (16-bit DOS)
#   make build-all   -> linux, windows, dos
#   make package-all -> zip all builds (assumes built)
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
IA16     = ia16-elf-gcc

CFLAGS   = -Wall -Wextra -O2 -std=c99
LDFLAGS  = -lm

# ============================
#   Output Directories
# ============================
BIN_DIR     = bin
DIST_DIR    = dist

BIN_LINUX    = $(BIN_DIR)/ia64-linux
BIN_WIN      = $(BIN_DIR)/mingw-ia32
BIN_DOS_GCC  = $(BIN_DIR)/ia16-gcc

PROGRAMS = vz2wav wav2vz text2bas-vz text2bas-cg vzexport

# =============================================================================
.PHONY: all linux windows dos build-all package-all \
        package-linux package-windows package-dos \
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
endef

# =============================================================================
# Linux Builds
# =============================================================================
linux: dirs $(BIN_LINUX) \
       $(BIN_LINUX)/vz2wav \
       $(BIN_LINUX)/wav2vz \
       $(BIN_LINUX)/text2bas-vz \
       $(BIN_LINUX)/text2bas-cg \
       $(BIN_LINUX)/vzexport
	@echo "Built: $(BIN_LINUX)/vz2wav"
	@echo "Built: $(BIN_LINUX)/wav2vz"
	@echo "Built: $(BIN_LINUX)/text2bas-vz"
	@echo "Built: $(BIN_LINUX)/text2bas-cg"
	@echo "Built: $(BIN_LINUX)/vzexport"
	$(MAKE) package-linux

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

$(BIN_LINUX)/vzexport: vzexport.c
	$(CC_LINUX) $(CFLAGS) -o $@ $<

# =============================================================================
# Windows (MinGW) Builds
# =============================================================================
windows: dirs $(BIN_WIN) \
		 $(BIN_WIN)/vz2wav.exe \
		 $(BIN_WIN)/wav2vz.exe \
		 $(BIN_WIN)/text2bas-vz.exe \
		 $(BIN_WIN)/text2bas-cg.exe \
		 $(BIN_WIN)/vzexport.exe
	@echo "Built: $(BIN_WIN)/vz2wav.exe"
	@echo "Built: $(BIN_WIN)/wav2vz.exe"
	@echo "Built: $(BIN_WIN)/text2bas-vz.exe"
	@echo "Built: $(BIN_WIN)/text2bas-cg.exe"
	@echo "Built: $(BIN_WIN)/vzexport.exe"
	$(MAKE) package-windows

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

$(BIN_WIN)/vzexport.exe: vzexport.c
	$(CC_WIN) $(CFLAGS) -o $@ $<

# =============================================================================
# DOS Builds (ia16 only)
# =============================================================================
dos: dirs $(BIN_DOS_GCC) \
	 $(BIN_DOS_GCC)/vz2wav.exe \
	 $(BIN_DOS_GCC)/wav2vz.exe \
	 $(BIN_DOS_GCC)/text2bas-vz.exe \
	 $(BIN_DOS_GCC)/text2bas-cg.exe \
	 $(BIN_DOS_GCC)/vzexport.exe
	@echo "Built: $(BIN_DOS_GCC)/vz2wav.exe"
	@echo "Built: $(BIN_DOS_GCC)/wav2vz.exe"
	@echo "Built: $(BIN_DOS_GCC)/text2bas-vz.exe"
	@echo "Built: $(BIN_DOS_GCC)/text2bas-cg.exe"
	@echo "Built: $(BIN_DOS_GCC)/vzexport.exe"
	$(MAKE) package-dos

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

$(BIN_DOS_GCC)/vzexport.exe: vzexport.c
	$(IA16) -mcmodel=small -o $@ $<

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

package-dos: dirs
	@echo "Packaging DOS build..."
	$(call stage-docs,$(BIN_DOS_GCC))
	cd $(BIN_DOS_GCC) && zip -qr ../../$(DIST_DIR)/dos.zip .

# =============================================================================
# Meta-targets
# =============================================================================
build-all: linux windows dos

package-all: dirs package-linux package-windows package-dos

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
	@echo "  make dos         - ia16-elf-gcc DOS"
	@echo "  make build-all   - All of the above"
	@echo ""
	@echo "Packaging:"
	@echo "  make package-all - Zip all builds (assumes built)"
	@echo ""
	@echo "Cleaning:"
	@echo "  make clean       - Remove bin/ and dist/"
	@echo "  make clean-bins  - Remove bin/ only"
	@echo "  make clean-dist  - Remove dist/ only"
