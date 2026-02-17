# VZ to WAV Converter

A clean, modern rewrite of a VZ tape format to WAV audio converter.

## About

This tool converts VZ tape format files (.vz) (used by VTech VZ200/VZ300 computers) into WAV audio files that can be played back to load programs on original hardware or emulators.

The code is a complete rewrite of decompiled Borland C DOS code, modernized for Linux/Unix systems.

## Building

Simple compilation:
```bash
make
```

Or manually:
```bash
gcc -Wall -O2 -o vz2wav vz2wav.c
```

## Installation

```bash
sudo make install
```

This installs the binary to `/usr/local/bin/`.

## Usage

```bash
./vz2wav input.vz output.wav
```

Example:
```bash
./vz2wav game.vz game.wav
```

## How It Works

The converter:

1. **Reads the VZ file** - Skips the 24-byte VZ header and reads the data
2. **Creates a WAV file** with:
   - 22050 Hz sample rate
   - 8-bit mono PCM
   - Proper WAV header
3. **Encodes the data** using VZ tape format:
   - Initial silence (1 second)
   - Preamble sync bytes (255 × 0x80, 5 × 0xFF)
   - Filename from VZ header
   - Data length (4 bytes)
   - Actual program data
   - Checksum (2 bytes)
   - Trailing silence (1 second)

Each byte is converted to 8 bits (MSB first), and each bit is represented by a specific audio waveform:
- **Bit 0**: 19 low samples, 19 high samples (longer wavelength)
- **Bit 1**: 9 low, 10 high, 10 low, 9 high (two shorter wavelengths)

## Technical Details

### VZ Tape Format

- Each bit is encoded as a specific waveform pattern (38 samples)
- Bit 0: Single long wave cycle
- Bit 1: Two shorter wave cycles
- Bytes are transmitted MSB first
- Includes preamble, data length, checksum

### WAV Format

- 8-bit unsigned PCM (128 = silence)
- Mono channel
- 22050 Hz sample rate
- Standard RIFF WAV header

## Differences from Original

This rewrite:
- ✅ Clean, readable, well-commented code
- ✅ Standard C99 (not Borland C)
- ✅ No DOS dependencies
- ✅ Proper error handling
- ✅ Memory safety
- ✅ Progress indication
- ✅ Works on modern Linux/Unix systems
- ✅ Fix some encoding bugs present in orignal tool

## License

This is a clean-room rewrite based on reverse-engineered functionality.
Use freely for educational and preservation purposes.

## Author

Rewritten from decompiled Borland C source code (circa 1990s).
