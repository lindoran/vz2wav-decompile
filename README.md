# VZ2WAV / WAV2VZ Reconstruction Project

This project began by disassembling the original **VZ2WAV.EXE** and using **Reko** to generate a decompiled version. With both the disassembly and decompiled output — plus known-good input/output files from the DOS utility — it became possible to “clean room” reconstruct the 16‑bit EXE.

The LLM was provided:

- Decompiled C-like output from Reko  
- A full disassembly of the original EXE  
- Sample input and output WAV files  
- The `text2basic` source code to help identify `.vz` file structure  

Using these, we were able to rediscover the `.vz` file format and the exact output characteristics of the original DOS tool.

## Findings

We discovered **two bugs** in the original Borland C code. Both were caused by **uninitialized stack values** being used in place of proper data. After correcting these, we were able to reproduce **byte‑accurate output** from the original DOS program.

For compatibility testing, command‑line flags were added to intentionally reproduce the original malformed behavior. These should not be needed for normal use, but they allow exact byte‑for‑byte reproduction of the DOS output.

---

## Usage

vz2wav [--compat] [--artifact] <input.vz> <output.wav>

### `--compat`
Produce a **malformed WAV file** that matches the DOS original exactly.  
The RIFF and data chunk size fields are set to the raw DOS stack garbage values:

- `0x00146e0c`
- `0x01140000`

The DOS program never rewound the file to fix these. Most WAV players tolerate this.  
Omit `--compat` for a standards‑compliant WAV file with correct size fields.

### `--artifact`
Write the exact **80‑byte uninitialized Borland C stack pattern** into the padding gap between the filename and address block, instead of `80 × 0x7F`.

This matches the padding bytes found in the DOS output byte‑for‑byte.  
The VZ hardware treats both identically (both are below the comparator threshold).  
Can be combined with `--compat`.

---

## Current Status

`wav2vz` is still broken. The program produces output that the DOS `wav2vz_2.exe` can convert back, but both tools report an error even though they still build a `.vz` file. This suggests the issue may be:

- A misunderstanding of how the VZ expects WAV files  
- A flaw in the original `wav2vz` logic  
- A remaining issue in the cleaned‑up `vz2wav` output  

Because it is so broken, the source has been temporarily removed until it can be fixed.

`text2bas` works correctly, but it is not as robust as the version at:

https://github.com/rweather/vz200-restoration/tree/main/src/bas2vz

---

## Portability and Datatype Fixes

The code has been updated to use **MinGW‑safe datatypes**. This was necessary because:

- The original DOS file used 16‑bit (or possibly 32‑bit) `long` types  
- Linux GCC uses **64‑bit** `long` types  
- Borland C treats `char` as **unsigned**, while GCC treats it as **signed**  

We capped integer sizes to **32 bits** and used explicit datatypes to ensure consistent behavior. More testing is needed, but this should work across compilers.

This should also compile under **TCC**, which opens the possibility of producing a version that runs under **16‑bit DOS** again — not especially useful unless you have piles of `.vz` files on a DOS machine, but technically possible.

---

## TODO

### Build System
- Fix the build chain so it works cleanly with MinGW  
- Update the Makefile to produce Windows `.exe` files directly from Linux or Windows  

### WAV2VZ
- Decompile `WAV2VZ_2.EXE`  
- Understand how it accepts nearly any input  
- Determine why the cleaned‑up output triggers an error even though it still converts  

### Swiss‑Army Front End
Create a unified tool that accepts a file and produces multiple output formats:

- `.bas`  
- `.vz`  
- `.cas`  
- `.bin`  
- `.hex`  
- `.srec`  

Add options for:

- Waveform inversion  
- Input filtering (like many Windows tools)  
- Square‑wave output patterns  

Modern sound cards don’t care much about waveform shape, but square waves may help with emulator loading or converting actual sampled audio from the VZ.

### Long‑Term Goals
- Fully document the **VZ200 BASIC ROM**  
- Reverse‑engineer the tape routines  
- Build a **fast loader**  
- Produce fast‑load output similar to the ZX Spectrum or the “2018AD” loader  

This may require repacking data and designing a custom loader, but it is entirely achievable.
