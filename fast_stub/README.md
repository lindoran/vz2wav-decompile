# fast_stub

Source for the `--fast-debug` receiver embedded in `vz2wav.c` (`FAST_DEBUG_STUB[]`, `FAST_DEBUG_RELOCS[]`).

- `gen_debug_stub.py` generates `fast_debug_stub.asm`, assembles it with z80asm 1.8 (twice, at two origins, to find every
  16-bit word that moves with the load address) and writes `fast_debug_stub.bin` and `fast_debug_stub.inc`.
- `fast_debug_stub.inc` is the C snippet to paste over the arrays in `vz2wav.c` after changing the stub.
- The production receiver (`FAST_BOOT_STUB`, 237 bytes) has no assembly source in the repo yet.
