VZ2WAV / WAV2VZ RECONSTRUCTION PROJECT
======================================

This project began by disassembling the original VZ2WAV.EXE and using
Reko to generate a decompiled version. With both the disassembly and the
decompiled output — plus known-good input/output files from the DOS
utility — it became possible to “clean room” reconstruct the 16-bit EXE.

The LLM was provided:

- Decompiled C-like output from Reko
- A full disassembly of the original EXE
- Sample input and output WAV files
- The text2basic source code to help identify the .vz file structure

Using these, we were able to rediscover the .vz file format and the exact
output characteristics of the original DOS tool.


FINDINGS
--------

We discovered two bugs in the original Borland C code. Both were caused
by uninitialized stack values being used instead of proper data. After
correcting these, we were able to reproduce byte-accurate output from
the original DOS program.

For compatibility testing, command-line flags were added to intentionally
reproduce the original malformed behavior. These should not be needed
for normal use, but they allow exact byte-for-byte reproduction of the
DOS output.

The wav2vz decompilation was considerably more difficult. The program
works, but it still carries over much of the original behavior around
checksums and end-of-file detection. We believe there are bugs in the
old code, but there is also a lot of work required to get it to run
without incorrectly reporting errors. For now it is kept as-is because
it does what it is supposed to do.


USAGE
-----

vz2wav:
    vz2wav [--compat] [--artifact] <input.vz> <output.wav>

    --compat
        Produce a malformed WAV file that matches the DOS original
        exactly. The RIFF and data chunk size fields are set to the raw
        DOS stack-garbage values:
            0x00146e0c
            0x01140000
        The DOS program never rewound the file to fix these. Most WAV
        players tolerate this. Omit --compat for a standards-compliant
        WAV file with correct size fields.

    --artifact
        Write the exact 80-byte uninitialized Borland C stack pattern
        into the padding gap between the filename and address block,
        instead of 80 × 0x7F. This matches the padding bytes found in
        the DOS output byte-for-byte. The VZ hardware treats both
        identically. Can be combined with --compat.

wav2vz:
    wav2vz <input.wav> <output.vz>

text2bas:
    Juergen Buchmueller's Text To BASic converter as found on Bushy's VZ200 Files Site:
    http://www.vz200.org/bushy/software.html - ported to gcc and console from Atari-ST

    text2bas-vz <input.txt> {output.vz}
    text2bas-cg <input.txt> {output.cas}
    If no output filename is given, the program generates one.

text2bas works correctly, but it is not as robust as the version at:
    https://github.com/rweather/vz200-restoration/tree/main/src/bas2vz


CURRENT STATUS
--------------

wav2vz:
    wav2vz is in a working state. It still has some cases where it
    reports that the file was read incorrectly even though the output is
    valid. More work is needed to tighten this up, but it is stable
    enough for now.

MinGW Version (Win32 console):
    The project now compiles under the Win32 version of MinGW. Being a
    console application, it should continue to work even if a 64-bit
    build is ever required. Pre-compiled DOS binaries are available in
    the dist folder.

    We target 32-bit Windows because the maximum RIFF file size is
    32 bits (2 GB). Even if a .vz file attempted to load a full 64K of
    address space (unlikely, but allowed by the format), the resulting
    WAV file would still remain within this limit. 32-bit application
    support is not in danger of removal at this time. If someone needs a
    64-bit build, the change is trivial — but the internal limits will
    remain 32-bit due to the WAV format.

gcc-ia16-elf:
    We added a build target for gcc-ia16-elf. This means you can run the
    tool on an 8088, just like the original DOS program — except now you
    have the source code. We attempted to use Watcom, but ran into
    issues with the text2bas code. For what it is worth, both vz2wav and
    wav2vz compile under Watcom without issues.

    The resulting ia16 build is only about 2% slower, so even on a
    10 MHz V20 you will barely notice the difference. gcc-ia16-elf is
    actively developed (primarily for ELKS) and is likely the better
    long-term compiler target. Watcom is powerful and mature, but
    difficult to build and configure. In contrast, ia16 is just an
    Ubuntu package.

text2bas updates:
    Found and fixed a bug that would output a zero symbol file, or skip
    the last line of a file if there was no terminated CR/LF or CR.

TODO
----

Swiss-Army Front End:
    Create a unified tool that accepts a file and produces multiple
    output formats:
        .bas
        .vz
        .cas
        .bin
        .hex
        .srec

    Add options for:
        Waveform inversion
        Input filtering
        Square-wave output patterns

    Modern sound cards do not care much about waveform shape, but square
    waves may help with emulator loading or converting sampled audio
    from the VZ.

Long-Term Goals:
    - Fully document the VZ200 BASIC ROM
    - Reverse-engineer the tape routines
    - Build a fast loader
    - Produce fast-load output similar to the ZX Spectrum or the
      “2018AD” loader

    This may require repacking data and designing a custom loader, but
    it is entirely achievable.


LICENSE
-------

Unless stated otherwise:

    Code here is public domain, as-is, and used at your own risk as of
    2026. No liability is implied.
