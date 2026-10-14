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

## Downloads:

You can download a .zip of the command line tools from the [`./dist`](./dist "Download Files") 
directory.  Current supported architectures are 16bit dos, 32bit windows,
64 bit windows and linux consoles.  

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
[`./dist`](./dist "Download Files") folder at the root of this repository.
you can place the files in a directory with the tools or you can put the 
tools in your path someplace and they will work that way as well.


Versioning:

- Tool version is sourced from the repository `VERSION` file.
- All CLI tools support `--version` / `-V`.

### vz2wav

```bash
vz2wav [options] input.vz output.wav
```

Options:

- `--fast`, `-f`
  Emit 2018AD speedload audio with a self-booting bootstrap stub (~12.5x speed).
  Stage 1 loads a 237-byte machine-code receiver via the standard VZ ROM
  `SYSTEM` or `CRUN` / `CLOAD`, which receives the payload at ~7,350 baud and
  auto-executes it (RUN for BASIC or a direct jump for machine code). If the
  payload overlaps the receiver, the receiver is relocated to a free address
  above the payload, and its internal call targets and stack are adjusted.
  The tool reports the receiver location and minimum RAM for both payload
  and loader (including its 512-byte temporary stack), rounded up to a whole
  1024-byte KB, with the exact highest required address shown too.
  Relocation is limited to the fixed RAM range below `0xC000`; conversion
  fails if no safe receiver/stack space is available.

  a hard and fast example: with the small reciver payload, a 6K program loads 
  in about 20 seconds as compared to several minutes!

- `--fast-flat-ram`
  Allow `--fast` to relocate the receiver and temporary stack into RAM above
  `0xBFFF`, up to `0xFFFF`. Use this only on modified systems with contiguous,
  flat RAM throughout `0x7800`-`0xFFFF`; the default limit remains `0xC000`
  for systems with bank-switched or absent memory above that address.

- `--fast-vram`
  Use the self-booting fast loader, but place the receiver at `0x7200` and
  its 512-byte temporary stack at `0x7600`-`0x77FF`, in the unused video-RAM
  area above the 512-byte text screen. This is opt-in and can reduce the
  ordinary RAM required by loader overhead; the minimum-RAM report then
  counts the program payload in main RAM. The machine-code program receives
  control with the temporary stack still in video RAM and is responsible for
  selecting its runtime stack, as it normally would. The payload must not
  overlap the reserved video-RAM loader/stack area.  This is useful when you:
  need to get the payload to fit in most of the ram, or if you have a stock 
  2K or 6K VZ200.  This works beccause the fast loader runs in text mode,
  and the video circut is only using 512 bytes of the full 2K of video ram.
  this may cause some snow during loading on a stock VZ...  This is normal
  and will not hurt the machine. Homebrew machines that use dual port ram
  such as the HB200 don't have this issue.

- `--fast-raw`
  Emit raw 2018AD speedload audio block without the standard bootstrap stub.
  Useful for custom loaders and demos that already have the fast receiver in RAM.

- `--fast-invert` (default), `--fast-normal`
  Phase of the fast block's bit cells. The default (`--fast-invert`, high half of
  each cell first) is the phase that loaded on real VZ200 hardware; the original
  phase (`--fast-normal`, low half first) loaded in emulators and is kept for
  systems that need it. The wrong phase is not subtle: the receiver synchronizes
  but decodes a garbage header (typically a wild start address and length) and,
  with interrupts off, stops responding to BREAK. Applies to `--fast`,
  `--fast-vram`, `--fast-flat-ram`, `--fast-debug` and `--fast-raw`; `wav2vz`
  detects either phase automatically when decoding.

- `--fast-debug`
  Like `--fast`, but the receiver is a verbose diagnostic build (about 0.9 KB)
  that paints a live status panel in the top-left of the screen while loading,
  then pauses about two seconds before running the program. It combines with
  `--fast-vram` and `--fast-flat-ram` and is relocated the same way as the
  normal receiver. Panel (all values hex):
  - `SYNC LAST` period count of the most recent pulse pair while hunting for sync
  - `SYNC END` count of the long pulse that ends sync; nominal `24`. A value near
    `1B` with a nonsense header (e.g. `F8 FD FF 1C 00` for `test.vz`) is the
    signature of the wrong phase.
  - `HDR` file type, start address (lo, hi) and length (lo, hi) as received;
    for `test.vz` it should read `F0 E9 7A 18 00`
  - `BODY` raw progress while loading, then `OK`; `END` start+length;
    `CKSUM` result of the same trailing-checksum test the normal receiver uses
    (like the normal receiver it halts on a failure, leaving the panel visible)
  - `SHORT` last short-pulse count (nominal `12`) and `THR` the pulse threshold
    actually used, calibrated as (short + sync-end long) / 2
  - `STAGE` letters appear as milestones are reached: Entered, Sync found,
    Header read, Body done, Done
  Source and generator are in `fast_stub/`.

- `--waveform <dos|square|sine>`, `-w <dos|square|sine>`
  Select the waveform profile:
  - `dos` (default): Exact Borland C DOS waveform table for byte-accurate compatibility.
  - `square` (or `--square`): Clean digital square wave (`195` high, `61` low).
  - `sine` (or `--sine`, `--sin`): Clean synthesized sine wave.
  
  testing seems to indicate that very little difference is found by modifiying this
  paramiter, but its here for completeness.  The circut as built on the VZ200 / 300 
  really cares only about the amplitude and timing of the waveform.  So changing the
  gain has more of an effect, however a bunch of testing on a lot of sound cards 
  has revealed about 110% gain with max volume on the preamp of the sound card 
  produces the best results.

- `--square`
  Convenience shorthand for `--waveform square`.

- `--sine`, `--sin`
  Convenience shorthand for `--waveform sine`.

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

- Fast-load WAVs report the packed program's file type, filename, addresses,
  size, and checksum status. The fast stream includes the standard two-byte
  VZ checksum, which the self-booting loader checks and `wav2vz` verifies;
  neither checksum byte is written to the output `.vz` payload. For
  self-booting hybrid WAVs, the filename is read from the standard VZ
  preamble; raw fast-load WAVs do not contain a filename. Fast-load WAVs
  without the checksum trailer are treated as incomplete.
  Fast blocks of either cell phase are accepted (the inverted phase is tried first,
  then the original); a decode is only accepted when its header is plausible
  (file type `F0`/`F1`, start+length within 64K).

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

Shout outs: 
- Greetz to Morbid, Imodium, vilcans and all of Five Finger Punch, for the
  fantastic fast loader they developed. All credit for that fantastic work 
  goes to them.