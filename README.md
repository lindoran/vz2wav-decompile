# VZ2WAV / WAV2VZ Reconstruction Project

This project began by disassembling the original `VZ2WAV.EXE` and using
Reko to generate a decompiled version. With both the disassembly and the
decompiled output — plus known-good input/output files from the DOS
utility — it became possible to clean-room reconstruct the 16-bit EXE.

The LLM was provided:

- Decompiled C-like output from Reko
- A full disassembly of the original EXE
- Sample input and output WAV files
- The text2bas source code to help identify the `.vz` file structure

Using these, we were able to rediscover the `.vz` file format and the
exact output characteristics of the original DOS tool.

Since then, the project has grown well beyond a straight reconstruction
into a more complete CLI toolset: robust and legacy decode modes, gain
controls, capture diagnostics, modern export/pack workflows (`vzexport`
and `vzpack`), cross-platform targets (Linux, Win32/Win64, DOS), unified
versioning, and install/package automation.

## Findings

We discovered two bugs in the original Borland C code. Both were caused
by uninitialized stack values being used instead of proper data. After
correcting these, we were able to reproduce byte-accurate output from
the original DOS program.

For compatibility testing, command-line flags were added to intentionally
reproduce the original malformed behavior. These should not be needed
for normal use, but they allow exact byte-for-byte reproduction of the
DOS output.

The `wav2vz` decompilation was considerably more difficult. The original
DOS behavior around sync/checksum handling and noisy captures needed extra
work. The current code includes modern decoding improvements while still
offering a legacy mode.

## Usage
Precompiled Packages:
Please find multiple ZIP files by platform for convenience in the
`./dist` folder at the root of this repository.

Versioning:

- Tool version is sourced from the repository `VERSION` file.
- All CLI tools support `--version` / `-V`.

### vz2wav

```bash
vz2wav [--compat|-c] [--artifact|-a] [--robust|-r] [--gain|-g <percent>] input.vz output.wav
```

Options:

- `--compat`, `-c`
  Produce a malformed WAV file that matches the DOS original exactly.
  The RIFF and data chunk size fields are set to the raw DOS
  stack-garbage values `0x00146e0c` and `0x01140000`.
  The DOS program never rewound the file to fix these. Most WAV players
  tolerate this. Omit `--compat` for a standards-compliant WAV file with
  correct size fields.

- `--artifact`, `-a`
  Write the exact 80-byte uninitialized Borland C stack pattern into the
  padding gap between the filename and address block, instead of 80 ×
  `0x7F`. This matches the padding bytes found in the DOS output byte for
  byte. The VZ hardware treats both identically. Can be combined with
  `--compat`.

- `--robust`, `-r`
  Uses a longer settle/leader timing profile for noisy analog paths.

- `--gain <percent>`, `-g <percent>`
  Signed amplitude delta around center. Example: `--gain 10` means 110%
  amplitude, `--gain -10` means 90%. Default is `+10`.

### wav2vz

```bash
wav2vz [--legacy|-l] [--gain|-g <percent>] input.wav output.vz
wav2vz [--legacy|-l] [--gain|-g <percent>] --analyze input.wav
```

Options:

- default mode
  Uses the newer filtered/noise-tolerant decode path (recommended).

- `--legacy`, `-l`
  Uses original-style threshold behavior.

- `--gain <percent>`, `-g <percent>`
  Signed input gain delta around center before classification.
  Default is `0`.

- `--analyze`, `-a`
  Analyze-only mode (no `.vz` output). Prints capture diagnostics such as
  min/max/mean, first signal position, run-length stats, cycle histogram,
  and short/long/error classification counts.

### text2bas

Juergen Buchmueller's Text To BASic converter as found on Bushy's VZ200
Files Site:

- http://www.vz200.org/bushy/software.html

Ported to gcc and console from Atari ST:

```bash
text2bas-vz input.txt [output.vz]
text2bas-cg input.txt [output.cas]
```

If no output filename is given, the program generates one.

`text2bas-vz` now writes a normalized VZ header filename from the input
basename: extension removed, uppercased, null-padded to the VZ field.

`text2bas` works correctly, but it is not as robust as the version at:

- https://github.com/rweather/vz200-restoration/tree/main/src/bas2vz

### vzexport

```bash
vzexport input.vz [--info|-i] [--out-bin|-b FILE] [--out-hex|-x FILE] [--out-srec|-s FILE] [--out-bas|-B FILE]
```

Options:

- `--info`, `-i`
  Print parsed VZ metadata (magic, type, filename, start/end, payload size).

- `--out-bin FILE`, `-b FILE`
  Write raw payload bytes (no VZ header).

- `--out-hex FILE`, `-x FILE`
  Write Intel HEX from payload bytes using VZ start address.

- `--out-srec FILE`, `-s FILE`
  Write Motorola S-record output from payload bytes using VZ start address.

- `--out-bas FILE`, `-B FILE`
  Detokenize BASIC payload to ASCII (only valid for BASIC type `0xF0`).

### vzpack

```bash
vzpack [input] -o output.vz [options]
```

Input (exactly one):

- `--in-bin FILE`, `-b FILE`
  Pack raw binary payload into `.vz` (requires `--start`).

- `--in-hex FILE`, `-x FILE`
  Pack Intel HEX. Out-of-order records are accepted; non-contiguous ranges
  require `--fill`.

- `--in-srec FILE`, `-r FILE`
  Pack Motorola S-record. Out-of-order records are accepted; non-contiguous
  ranges require `--fill`.

- `--in-bas FILE`, `-B FILE`
  Tokenize ASCII BASIC and pack as BASIC `.vz`.

Options:

- `--out FILE`, `-o FILE`
  Output `.vz` path.

- `--name NAME`, `-n NAME`
  Override VZ header filename (normalized/truncated to VZ limits).

- `--type basic|mc|auto`, `-t basic|mc|auto`
  Force BASIC (`0xF0`) or machine-code (`0xF1`) type. Default is `auto`.

- `--start ADDR`, `-s ADDR`
  Override start address (required for `--in-bin`; optional override for others).

- `--fill BYTE`, `-f BYTE`
  Fill byte (`0..255`, decimal/hex) used for holes when packing sparse
  HEX/SREC address ranges.

## Current Status

### MinGW Versions (Win32 and Win64)

The project now compiles under MinGW for both 32-bit and 64-bit Windows.
Use `make windows`, `make windows64`, or `make windows-all`.
Pre-compiled DOS binaries are available in the `dist` folder.

Both Windows targets are supported. The 32-bit target remains useful for
older systems/toolchains, while the 64-bit target is available for modern
hosts. Internal WAV-size limits are still 32-bit by format definition
(RIFF/data chunk fields), regardless of binary bitness.

### Install target (Linux)

```bash
make install
```

Optional overrides:

- `PREFIX=/opt/vz2wav`
- `DESTDIR=/tmp/vz2wav-pkg` (staged/package install)

### gcc-ia16-elf

We added a build target for `gcc-ia16-elf`. This means you can run the
tool on an 8088, just like the original DOS program — except now you
have the source code. We attempted to use Watcom, but ran into issues
with the `text2bas` code. For what it is worth, both `vz2wav` and
`wav2vz` compile under Watcom without issues.

The resulting ia16 build is only about 2% slower, so even on a 10 MHz
V20 you will barely notice the difference. `gcc-ia16-elf` is actively
developed (primarily for ELKS) and is likely the better long-term
compiler target. Watcom is powerful and mature, but difficult to build
and configure. In contrast, ia16 is just an Ubuntu package.

### text2bas updates

Found and fixed a bug that would output a zero symbol file, or skip the
last line of a file if there was no terminated CR/LF or CR.

## TODO

Roadmap and outstanding work items now live in:

- `TODO.md`

## License

Repository-default license for original project contributions is the
UNLICENSE (public domain dedication), see:

- `UNLICENSE`
- `NOTICE`

Scope clarification:

- This dedication applies only to rights held by this repository's
  contributors.
- Third-party/inherited components (notably `text2bas.c` and
  `AtariST_Version_SRC/`) keep their own provenance/licensing status as
  documented in `TEXT2BAS.md`.
- This project is provided as-is, without warranty.
