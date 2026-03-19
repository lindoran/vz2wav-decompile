# TODO / Roadmap

## Near-term tooling

- Build a true single front-end command that auto-detects input type and
  dispatches to the right conversion path (today this is split across
  `text2bas`, `vz2wav`, `wav2vz`, `vzexport`, and `vzpack`).
- Add `.cas` and additional output pathways to `vzexport` where practical.
- Extend BASIC detokenization output quality (formatting, edge-case tokens,
  and comments/strings fidelity checks).
- Add more end-to-end fixtures (BASIC and machine-code) for round-trip tests
  across Linux/Windows/ia16 builds.

## Container and format support

- Add `.cvz` container support (read/write) with clear compatibility notes
  for emulators that support it.
- Extend `vzpack`/`vzexport` to handle `.cvz` where feasible, or provide
  dedicated subcommands if cleaner.
- Add a third data-prep tool (working name: `vzdisk`) for manipulating common
  disk-image workflows and converting linked/program outputs into forms that
  feed cleanly into `vzpack`.
- Define supported disk formats and metadata rules for `vzdisk` (for example:
  image type, load/run addresses, file naming, and segment mapping policy).

## Archival workflow (wav2vz)

- Add explicit archival mode/features focused on preservation and recovery.
- Validate incoming WAV format early (sample rate, bit depth, channels) and
  produce actionable diagnostics.
- Add optional conversion assist for unsupported WAV formats by integrating
  with common CLI audio tools (for example `ffmpeg`/`sox`) when available.
- Discuss and define whether conversion/conditioning should remain external
  helper orchestration or be implemented natively inside `wav2vz`.
- Add optional pre-decode conditioning pipeline (normalization, DC handling,
  filtering) with clear reporting of what was applied.
- Emit archival sidecar reports (decode confidence, cycle/error stats, and
  any conversion/conditioning steps used).

## Signal path options

- Add optional waveform inversion controls for encode/decode paths.
- Add optional square-wave output profile in `vz2wav` for compatibility
  experiments on difficult analog/emulator setups.

## Long-term goals

- Fully document the VZ200 BASIC ROM.
- Reverse-engineer the tape routines.
- Build a fast loader.
- Produce fast-load output similar to the ZX Spectrum or the "2018AD" loader.

This may require repacking data and designing a custom loader, but it is
entirely achievable.
