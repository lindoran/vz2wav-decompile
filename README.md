# VZ2WAV / WAV2VZ Reconstruction Project

This project began by disassembling the original **VZ2WAV.EXE** and using **Reko** to generate a decompiled version. With both the disassembly and decompiled output — plus known‑good input/output files from the DOS utility — it became possible to “clean room” reconstruct the 16‑bit EXE.

The LLM was provided:

- Decompiled C‑like output from Reko
- A full disassembly of the original EXE
- Sample input and output WAV files
- The `text2basic` source code to help identify `.vz` file structure

Using these, we were able to rediscover the `.vz` file format and the exact output characteristics of the original DOS tool.

## Findings

We discovered **two bugs** in the original Borland C code. Both were caused by **uninitialized stack values** being used in place of proper data. After correcting these, we were able to reproduce **byte‑accurate output** from the original DOS program.

For compatibility testing, command‑line flags were added to intentionally reproduce the original malformed behavior. These should not be needed for normal use, but they allow exact byte‑for‑byte reproduction of the DOS output.

The wav2vz decompilation was considerably more difficult. The program works, but it still copies a lot of the behavior around checksums and end‑of‑file detection. We believe there are bugs in the old code, but there is also a lot of work required to get it to run without reporting an error — in error. For now it’s kept as‑is because it does what it’s supposed to do.

---

## Usage

vz2wav [--compat] [--artifact] <input.vz> <output.wav>

### `--compat`
Produce a **malformed WAV file** that matches the DOS original exactly.  
The RIFF and data chunk size fields are set to the raw DOS stack‑garbage values:

- `0x00146e0c`
- `0x01140000`

The DOS program never rewound the file to fix these. Most WAV players tolerate this.  
Omit `--compat` for a standards‑compliant WAV file with correct size fields.

### `--artifact`
Write the exact **80‑byte uninitialized Borland C stack pattern** into the padding gap between the filename and address block, instead of `80 × 0x7F`.

This matches the padding bytes found in the DOS output byte‑for‑byte.  
The VZ hardware treats both identically (both are below the comparator threshold).  
Can be combined with `--compat`.

wav2vz <input.wav> <output.vz>

text2bas-vz <input.txt> {output.vz} - The program names the output if not given any  
text2bas-cg <input.txt> {output.cas}

---

## Current Status

wav2vz is in a working state. It still has some issues where it reports the file was read incorrectly, but it worked anyway. We think there is still work to be done to tighten this up, but it’s fixed enough for now.

We made it compile under the Win32 version of MinGW. Being a console app, it should just work if someday you *have* to make a 64‑bit binary, but for now you can find pre‑compiled DOS binaries in the `dist` folder of this distro. The reason 32‑bit was targeted is that the max file size for RIFF is 32 bits (2 GB). So even if we try to make a file that loads a full 64K of address space to the memory map (basically impossible, but the `.vz` format allows this), the resulting output file won’t exceed this. Additionally, 32‑bit application support is in no danger of being removed at this time. That being said, the change is trivial, so if anybody absolutely *has* to have 64‑bit, we can modify the build chain. It will, however, still limit everything to 32 bits internally, since this is the limitation for `.wav` files.

We can probably still produce a cross‑compiled version that works with plain old DOS. As stated before, we think it will just compile on TCC, and with that it should work under DOS16. We are investigating Watcom to see if it will compile there, as it’s a decent cross‑compiler for Linux and wouldn’t require any datatype changes as far as we can tell. This may actually be slower because we would want to target the 8088; otherwise it would limit execution to the 386 and above. It would be absolutely possible to use 32‑bit math directly on the 386 with a protected‑mode extension, but if we are doing DOS support that runs on anything supporting 16‑bit DOS, we see no reason for a protected‑mode version.

`text2bas` was updated to not skip the last line if the programmer forgot to close the file with an end‑of‑line character.

`text2bas` works correctly, but it is not as robust as the version at:  
https://github.com/rweather/vz200-restoration/tree/main/src/bas2vz

---

## Portability and Datatype Fixes

The code has been updated to use **MinGW‑safe datatypes**. This was necessary because:

- The original DOS file used 16‑bit (or possibly 32‑bit) `long` types
- Linux GCC uses **64‑bit** `long` types
- Borland C treats `char` as **unsigned**, while GCC treats it as **signed**

We capped integer sizes to **32 bits** and used explicit datatypes to ensure consistent behavior. More testing is needed, but this should work across compilers.

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
