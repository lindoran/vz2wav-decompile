# TEXT2BAS - BASIC Tokenizer for VZ200/300 and Colour Genie

Linux port of the TEXT2BAS tokenizer for converting ASCII BASIC programs to tokenized format for vintage computers.

## What This Does

Converts human-readable ASCII BASIC programs into the tokenized binary format used by:
- **VZ200/300** (VTech/Dick Smith computers) - creates `.vz` files
- **Colour Genie** (EACA computers) - creates `.cas` files

## Building

### VZ200/300 version (default):
```bash
make
```
or
```bash
gcc -Wall -O2 -DCGENIE=0 -o text2bas-vz text2bas.c
```

### Colour Genie version:
```bash
make cgenie
```
or
```bash
gcc -Wall -O2 -DCGENIE=1 -o text2bas-cg text2bas.c
```

## Usage

```bash
./text2bas-vz input.bas [output.vz] [options]
```

### Options
- `-n, --no-tokenize` - Don't tokenize BASIC keywords (keep them as text)
- `-s, --no-squeeze` - Don't squeeze multiple blanks into single spaces
- `-h, --help` - Show help message

### Examples

Basic conversion:
```bash
./text2bas-vz program.bas
# Creates program.vz
```

Specify output filename:
```bash
./text2bas-vz mycode.txt output.vz
```

Disable tokenization:
```bash
./text2bas-vz program.bas -n
```

## Input Format

Your ASCII BASIC file should look like:
```basic
10 PRINT "HELLO WORLD"
20 FOR I=1 TO 10
30 PRINT I
40 NEXT I
50 END
```

### Notes:
- Line numbers are optional (will be auto-numbered if missing)
- Keywords will be tokenized (converted to single-byte tokens)
- For VZ200/300: All text is converted to uppercase
- For Colour Genie: Case is preserved
- Tabs are expanded to spaces
- Multiple spaces are squeezed to single spaces (unless disabled)
- **Line endings**: Handles both Unix (LF) and DOS/Windows (CR or CRLF) formats automatically

## Output Format

### VZ200/300 (.vz files)
- VZF0 header with filename
- Load address: 0x7ae9
- Tokenized BASIC program
- Compatible with VZ200/VZ300 emulators

### Colour Genie (.cas files)
- Colour Genie tape format
- Load address: 0x5801
- Tokenized BASIC program
- Compatible with Colour Genie emulators

## Supported BASIC Keywords

The tokenizer recognizes standard BASIC keywords including:
- Control: `FOR`, `NEXT`, `IF`, `THEN`, `GOTO`, `GOSUB`, `RETURN`, `END`
- I/O: `PRINT`, `INPUT`, `DATA`, `READ`
- Math: `SIN`, `COS`, `TAN`, `ABS`, `INT`, `SQR`, `RND`
- String: `LEFT$`, `RIGHT$`, `MID$`, `CHR$`, `ASC`
- And many more...

See the token tables in `text2bas.c` for the complete list.

## Installation

vz version:

```bash
make vz
```
or color genie:

```bash
make cgenie
```

```bash
sudo make install
```

This installs to `/usr/local/bin/text2bas-vz`

To uninstall:
```bash
sudo make uninstall
```

## Differences from Atari ST Version

This Linux port removes all Atari ST GEM dependencies:
- No `cflib.h` requirement
- No graphical file selector
- Standard command-line interface
- Uses standard C library functions
- Cross-platform compatible (Linux, macOS, BSD, etc.)

## License

Original code appeared to be public domain. This Linux port maintains that status.

## Credits

Juergen Buchmueller's Text To BASic converter as found on Bushy's VZ200 Files Site:
http://www.vz200.org/bushy/software.html
Linux port: 2024 - Z8D (David Collins And Gemini LLM)
