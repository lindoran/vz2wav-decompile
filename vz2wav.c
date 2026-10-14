/*
 * vz2wav.c  --  Convert a VZ/VZ200/VZ300 tape image file to a WAV audio file.
 *
 * Reconstructed from the decompiled 16-bit DOS binary (vz2wav.exe), with the
 * actual waveform tables and file structure verified against a known-good WAV
 * output produced by the original DOS application.
 *
 * Usage:
 *   vz2wav [--compat|-c] [--artifact|-a] [--robust|-r] [--gain|-g <percent>] <input.vz> <output.wav>
 *
 *   --compat, -c
 *               Produce a "malformed" WAV file that matches the DOS original
 *               exactly: the RIFF and data chunk size fields are set to the
 *               raw DOS stack garbage values (0x00146e0c and 0x01140000) because
 *               the DOS program never rewound the file to fix them.
 *               Most WAV players handle this gracefully.  Omit --compat for a
 *               standards-compliant WAV file with correct size fields.
 *
 *   --artifact, -a
 *               Write the exact 80-byte uninitialized Borland C stack pattern
 *               into the padding gap between the filename and address block,
 *               instead of 80 x 0x7F.  This matches the padding bytes found
 *               in the known-good DOS binary output byte-for-byte.  The VZ
 *               hardware treats both identically (both are below the comparator
 *               threshold).  Can be combined with --compat.
 *
 *   --robust, -r
 *               Emit a more tolerant waveform envelope for real-world analog
 *               paths (longer settle time, leader, and sync burst). This is
 *               intended for noisy line-in/line-out chains.
 *
 *   --gain, -g  Signed percent delta applied around 0x7F center.
 *               Example: --gain 10 => 110% amplitude, --gain -10 => 90%.
 *
 * Build (Linux / GCC):
 *   gcc -Wall -o vz2wav vz2wav.c
 *
 * Build (Windows / MinGW):
 *   gcc -Wall -o vz2wav.exe vz2wav.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#ifndef TOOL_VERSION
#define TOOL_VERSION "dev"
#endif

/* -------------------------------------------------------------------------
 * Constants
 * ------------------------------------------------------------------------- */

#define SAMPLE_RATE         22050
#define SILENCE_BYTE        0x7F
#define SILENCE_SAMPLES     22050
#define LEADER_BYTE         0x80
#define LEADER_COUNT        255
#define SYNC_BYTE           0xFE
#define SYNC_COUNT          5
#define PADDING_RAW_BYTES   80
#define VZ_HEADER_SIZE      24
#define FILENAME_FIELD_LEN  16
#define SAMPLES_PER_BIT     38
#define POST_CKSUM_GUARD    0x00
#define ROBUST_PRE_SILENCE_SAMPLES  ((uint32_t)SAMPLE_RATE * 2u)
#define ROBUST_POST_SILENCE_SAMPLES ((uint32_t)SAMPLE_RATE * 2u)
#define ROBUST_LEADER_COUNT         384
#define ROBUST_SYNC_COUNT           SYNC_COUNT
#define SIGNAL_CENTER               127
#define DEFAULT_GAIN_PERCENT        10
#define MIN_GAIN_PERCENT           -90
#define MAX_GAIN_PERCENT           300

static int g_output_gain_percent = DEFAULT_GAIN_PERCENT;
/* Fast block cell phase. 1 (default) = high half first, verified on real VZ200 hardware;
 * 0 = low half first (original phase, matches the emulators tried so far). */
static int g_fast_invert = 1;

static int memory_range_overlaps(uint16_t load_addr, uint32_t length,
                                 uint32_t range_start, uint32_t range_end)
{
    uint32_t end;

    if (length == 0)
        return 0;
    if (length >= 0x10000u)
        return 1;

    end = (uint32_t)load_addr + length;
    if (end <= 0x10000u)
        return (uint32_t)load_addr < range_end && range_start < end;

    return (uint32_t)load_addr < range_end ||
           range_start < end - 0x10000u;
}

typedef enum {
    WAVEFORM_DOS = 0,
    WAVEFORM_SQUARE,
    WAVEFORM_SINE
} WaveformMode;

static WaveformMode g_waveform_mode = WAVEFORM_DOS;

/* -------------------------------------------------------------------------
 * Bit waveform tables
 * ------------------------------------------------------------------------- */

/* Exact values from the DOS binary */
static const unsigned char BIT1_WAVE_DOS[SAMPLES_PER_BIT] = {
    156, 178, 189, 191, 195, 179, 118,  88,  75,  70,
     70,  67, 110, 163, 180, 190, 190, 195, 168, 108,
     84,  73,  69,  69,  68, 117, 165, 181, 190, 190,
    195, 161, 104,  83,  72,  69,  67,  72
};

static const unsigned char BIT0_WAVE_DOS[SAMPLES_PER_BIT] = {
    165, 181, 190, 190, 195, 161, 104,  83,  72,  69,
     67,  72, 129, 169, 183, 190, 192, 192, 191, 191,
    189, 189, 186, 189, 157,  98,  76,  65,  62,  61,
     61,  62,  63,  64,  64,  67,  65, 106
};

/* Clean square wave (high = 195, low = 61, centered around 127) */
static const unsigned char BIT1_WAVE_SQUARE[SAMPLES_PER_BIT] = {
    195, 195, 195, 195, 195, 195,  61,  61,  61,  61,
     61,  61,  61, 195, 195, 195, 195, 195, 195,  61,
     61,  61,  61,  61,  61,  61, 195, 195, 195, 195,
    195, 195,  61,  61,  61,  61,  61,  61
};

static const unsigned char BIT0_WAVE_SQUARE[SAMPLES_PER_BIT] = {
    195, 195, 195, 195, 195, 195,  61,  61,  61,  61,
     61,  61, 195, 195, 195, 195, 195, 195, 195, 195,
    195, 195, 195, 195, 195,  61,  61,  61,  61,  61,
     61,  61,  61,  61,  61,  61,  61,  61
};

/* Clean sine wave (amplitude 68, peak values 195 / 59, centered around 127) */
static const unsigned char BIT1_WAVE_SINE[SAMPLES_PER_BIT] = {
    143, 172, 191, 195, 183, 159, 127,  95,  71,  59,
     63,  82, 111, 143, 172, 191, 195, 183, 159, 127,
     95,  71,  59,  63,  82, 111, 145, 175, 193, 193,
    175, 145, 109,  79,  61,  61,  79, 109
};

static const unsigned char BIT0_WAVE_SINE[SAMPLES_PER_BIT] = {
    145, 175, 193, 193, 175, 145, 109,  79,  61,  61,
     79, 109, 135, 151, 166, 178, 187, 193, 195, 193,
    187, 178, 166, 151, 135, 119, 103,  88,  76,  67,
     61,  59,  61,  67,  76,  88, 103, 119
};

/* -------------------------------------------------------------------------
 * Fast loader bootstrap stub (Five Finger Punch 2018AD protocol)
 * ------------------------------------------------------------------------- */

#define FAST_BOOT_STUB_LOAD_ADDR    0x7AE9
#define FAST_SYNC_BYTE_COUNT        7
#define FAST_INTERSTAGE_GAP_SAMPLES 11025   /* 0.5 sec silence between stub and fast data */
#define FAST_BOOT_READ_BIT_OFFSET   149u
#define FAST_BOOT_CHECKSUM_OFFSET  173u
#define FAST_BOOT_STACK_RESERVE     512u
#define FAST_BOOT_RAM_START         0x7800u
#define FAST_BOOT_RAM_END           0xC000u
#define FAST_BOOT_FLAT_RAM_END      0x10000u
#define FAST_BOOT_VRAM_LOADER_START 0x7200u
#define FAST_BOOT_VRAM_STACK_START  0x7600u
#define FAST_BOOT_VRAM_STACK_TOP    0x7800u

/*
 * Z80 machine code bootstrap receiver (237 bytes, assembled for base 0x7AE9).
 * Relocation patches its absolute internal CALL targets and private stack top.
 * - Disables interrupts, configures SP, sets IY=0x6800, D=26 (pulse threshold)
 * - Synchronizes on 60 short pulses, followed by the sync end (long pulse)
 * - Reads 1-byte file_type (0xF0=BASIC, 0xF1=MC)
 * - Reads 2-byte start address (little endian)
 * - Reads 2-byte length (little endian)
 * - Loops reading bytes at high speed (~7,350 baud) directly into destination RAM
 * - Verifies the trailing checksum (address bytes + payload) without storing it
 * - Updates system variables (0x78F9 end address, 0x78A4 PRGEND if BASIC)
 * - If BASIC: calls RENEW_LINE_PTRS (0x1AFC), VRESET (0x1B5D), writes ASCII
 *             "RUN\0" into I/O buffer (0x79E8), then executes via JP 0x1A81
 * - If MC:   enables interrupts and jumps directly to start address via JP (HL)
 *
 * Layout / notable addresses (load base = 0x7AE9):
 *   0x7AE9 +   0 : DI / LD SP,patched_top / LD IY,0x6800 / LD D,0x1A / LD E,0x3C
 *   0x7AE9 +  51 : receive-header section (5x CALL read_bit @ 0x7B7E)
 *   0x7AE9 +  74 : receive-body loop
 *   0x7AE9 + 112 : CP 0xF1
 *   0x7AE9 + 114 : JR Z,+0x1F  -> 0x7B7C (MC path)
 *   0x7AE9 + 116 : BASIC path: LD(PRGEND),HL / CALL RENEW_LINE_PTRS / CALL VRESET
 *   0x7AE9 + 125 : BASIC completion: EI / write "RUN\0" to 0x79E8 / JP 0x1A81
 *   0x7AE9 + 147 : MC completion: EI / JP (HL)
 *   0x7AE9 + 149 : read_bit subroutine (0x7B7E, 24 bytes)
 *   0x7AE9 + 173 : checksum verifier (0x7B96, 64 bytes)
 */
static const uint8_t FAST_BOOT_STUB[237] = {
    /* [  0] 0x7AE9 : DI / LD SP,patched_top / LD IY,0x6800 / LD D,0x1A / LD E,0x3C */
    0xF3, 0x31, 0xFF, 0x7F, 0xFD, 0x21, 0x00, 0x68, 0x16, 0x1A, 0x1E, 0x3C,
    /* [ 12] 0x7AF5 : sync loop (short-pulse counting) */
    0xAF, 0x3C, 0xFD, 0xCB, 0x00, 0x76, 0x20, 0xF9, 0x3C, 0xFD, 0xCB, 0x00,
    0x76, 0x28, 0xF9, 0xBA, 0x30, 0xEC, 0x1D, 0x20, 0xEB, 0xAF, 0x3C, 0xFD,
    0xCB, 0x00, 0x76, 0x20, 0xF9, 0x3C, 0xFD, 0xCB, 0x00, 0x76, 0x28, 0xF9,
    0xBA, 0x38, 0xEE,
    /* [ 51] 0x7B1C : read header: file_type, addr lo/hi, len lo/hi */
    /*        CALLs target read_bit at 0x7B7E                        */
    0xCD, 0x7E, 0x7B, 0x7B, 0xF5, 0xCD, 0x7E, 0x7B, 0x6B,
    0xCD, 0x7E, 0x7B, 0x63, 0xE5, 0xCD, 0x7E, 0x7B, 0x4B, 0xCD, 0x7E, 0x7B,
    0x43, 0xC5,
    /* [ 74] 0x7B33 : receive-body loop */
    0xCD, 0x7E, 0x7B, 0x73, 0x23, 0x0B, 0x78, 0xB1, 0x20, 0xF6,
    /* Capture checksum immediately: the fast stream has no pause after the payload. */
    0xCD, 0x7E, 0x7B, 0xD5, 0xCD, 0x7E, 0x7B, 0xD5, 0xC1, 0xD1, 0xD9,
    /* [ 95] 0x7B48 : post-load: compute end_addr, store in 0x78F9, restore regs */
    0xC1, 0xD1, 0xD5, 0x62, 0x6B, 0x09, 0x22, 0xF9, 0x78, 0xE1,
    0xCD, 0x96, 0x7B, 0x30, 0x01, 0x76, 0xF1, 0xFE,
    /* [112] 0x7B59 : CP 0xF1 (machine code?) */
    /* [114] 0x7B5B : JR Z, +0x1F  ->  0x7B7C (MC path at offset 147) */
    0xF1, 0x28, 0x1F,
    /* [116] 0x7B5D : BASIC path: store start_addr in PRGEND, renew ptrs, reset vars */
    0x22, 0xA4, 0x78, 0xCD, 0xFC, 0x1A, 0xCD, 0x5D, 0x1B,
    /* [125] 0x7B66 : BASIC completion:
     *   EI
     *   LD HL, 0x79E8       ; I/O buffer address
     *   LD (HL), 'R'        ; write ASCII "RUN\0" so 0x1A81 executes RUN
     *   INC HL
     *   LD (HL), 'U'
     *   INC HL
     *   LD (HL), 'N'
     *   INC HL
     *   LD (HL), 0x00       ; NUL terminator
     *   LD HL, 0x79E7       ; buffer-1 (as expected by 0x1A81 entry)
     *   XOR A               ; clear A (direct-mode, no line number)
     *   JP  0x1A81          ; parse & execute direct command -> RUN
     */
    0xFB, 0x21, 0xE8, 0x79,
    0x36, 0x52,              /* LD (HL),'R' */
    0x23,
    0x36, 0x55,              /* LD (HL),'U' */
    0x23,
    0x36, 0x4E,              /* LD (HL),'N' */
    0x23,
    0x36, 0x00,              /* LD (HL),0x00 */
    0x21, 0xE7, 0x79,        /* LD HL,0x79E7 */
    0xAF,                    /* XOR A */
    0xC3, 0x81, 0x1A,        /* JP  0x1A81 */
    /* [147] 0x7B7C : MC completion: EI / JP (HL) -> start_addr */
    0xFB, 0xE9,
    /* [149] 0x7B7E : read_bit subroutine (24 bytes) */
    0x1E, 0x01, 0xAF, 0x3C, 0xFD, 0xCB,
    0x00, 0x76, 0x20, 0xF9, 0x3C, 0xFD, 0xCB, 0x00, 0x76, 0x28, 0xF9, 0xBA,
    0x3F, 0xCB, 0x13, 0x30, 0xEB, 0xC9,
    /* [173] 0x7B96 : sum start address, end address and payload; compare captured trailer */
    0xC5, 0xD5, 0xE5, 0x7B, 0x82, 0x6F, 0x3E, 0x00, 0xCE, 0x00, 0x67,
    0x1A, 0x85, 0x6F, 0x7C, 0xCE, 0x00, 0x67, 0x13, 0x0B, 0x78, 0xB1, 0x20, 0xF3,
    0x3A, 0xF9, 0x78, 0x85, 0x6F, 0x7C, 0xCE, 0x00, 0x67,
    0x3A, 0xFA, 0x78, 0x85, 0x6F, 0x7C, 0xCE, 0x00, 0x67,
    0xD9, 0x7B, 0xD9, 0xBD, 0x20, 0x0B,
    0xD9, 0x79, 0xD9, 0xBC, 0x20, 0x05,
    0xE1, 0xD1, 0xC1, 0xB7, 0xC9,
    0xE1, 0xD1, 0xC1, 0x37, 0xC9
};

/*
 * Verbose DEBUG variant of the bootstrap stub (--fast-debug). Source and generator: fast_stub/.
 * Same bit decoder and trailing-checksum test as the production stub, but it paints a live status
 * panel in the top-left of the screen (direct video-RAM writes, timing-compensated):
 *     SYNC LAST:xx   period count of the most recent pulse while hunting for sync (hex)
 *     SYNC END :xx   count of the long pulse that ended sync (nominal 0x24)
 *     HDR:tt ll hh ll hh   file type, start lo/hi, length lo/hi as received
 *     BODY: ..       raw remaining-length bytes while loading, then OK
 *     END:  eeee     start+length            CKSUM:OK/BAD  trailing checksum result
 *     SHORT:xx THR:xx  last short pulse count (nominal 0x12) and the threshold actually used,
 *                      calibrated as (short + sync-end long) / 2
 *     STAGE: ESHBD   Entered / Sync found / Header read / Body done / Done
 * After the load it pauses ~2 s (interrupts off) so the panel can be read, then runs the program like
 * the production stub (HALTs on a checksum failure, like the production stub).
 * Relocatable: FAST_DEBUG_RELOCS lists the 16-bit words that move with the load address.
 */
#define FAST_DEBUG_STUB_BASE        0x7C00u
static const uint8_t FAST_DEBUG_STUB[918] = {
    0xF3, 0x31, 0xFF, 0x7F, 0xFD, 0x21, 0x00, 0x68, 0x21, 0x00, 0x70, 0x11,
    0x01, 0x70, 0x01, 0xFF, 0x00, 0x36, 0x60, 0xED, 0xB0, 0x21, 0x3E, 0x7F,
    0x11, 0x00, 0x70, 0xCD, 0x34, 0x7F, 0x21, 0x50, 0x7F, 0x11, 0x20, 0x70,
    0xCD, 0x34, 0x7F, 0x21, 0x5B, 0x7F, 0x11, 0x40, 0x70, 0xCD, 0x34, 0x7F,
    0x21, 0x66, 0x7F, 0x11, 0x60, 0x70, 0xCD, 0x34, 0x7F, 0x21, 0x6B, 0x7F,
    0x11, 0x80, 0x70, 0xCD, 0x34, 0x7F, 0x21, 0x71, 0x7F, 0x11, 0xA0, 0x70,
    0xCD, 0x34, 0x7F, 0x21, 0x76, 0x7F, 0x11, 0xAC, 0x70, 0xCD, 0x34, 0x7F,
    0x21, 0x7D, 0x7F, 0x11, 0xC0, 0x70, 0xCD, 0x34, 0x7F, 0x21, 0x84, 0x7F,
    0x11, 0xCA, 0x70, 0xCD, 0x34, 0x7F, 0x21, 0x89, 0x7F, 0x11, 0xE0, 0x70,
    0xCD, 0x34, 0x7F, 0x3E, 0x45, 0x32, 0xE7, 0x70, 0x16, 0x16, 0x1E, 0x3C,
    0x3E, 0x04, 0x3C, 0xFD, 0xCB, 0x00, 0x76, 0x20, 0xF9, 0x3C, 0xFD, 0xCB,
    0x00, 0x76, 0x28, 0xF9, 0x4F, 0x79, 0x0F, 0x0F, 0x0F, 0x0F, 0xE6, 0x0F,
    0xC6, 0x90, 0x27, 0xCE, 0x40, 0x27, 0xF6, 0x40, 0x32, 0x2A, 0x70, 0x79,
    0xE6, 0x0F, 0xC6, 0x90, 0x27, 0xCE, 0x40, 0x27, 0xF6, 0x40, 0x32, 0x2B,
    0x70, 0x79, 0xBA, 0x30, 0xC9, 0x41, 0x1D, 0x20, 0xC7, 0x3E, 0x04, 0x3C,
    0xFD, 0xCB, 0x00, 0x76, 0x20, 0xF9, 0x3C, 0xFD, 0xCB, 0x00, 0x76, 0x28,
    0xF9, 0x4F, 0xBA, 0x30, 0x23, 0x41, 0x79, 0x0F, 0x0F, 0x0F, 0x0F, 0xE6,
    0x0F, 0xC6, 0x90, 0x27, 0xCE, 0x40, 0x27, 0xF6, 0x40, 0x32, 0xC6, 0x70,
    0x79, 0xE6, 0x0F, 0xC6, 0x90, 0x27, 0xCE, 0x40, 0x27, 0xF6, 0x40, 0x32,
    0xC7, 0x70, 0x18, 0xC9, 0x79, 0x0F, 0x0F, 0x0F, 0x0F, 0xE6, 0x0F, 0xC6,
    0x90, 0x27, 0xCE, 0x40, 0x27, 0xF6, 0x40, 0x32, 0x4A, 0x70, 0x79, 0xE6,
    0x0F, 0xC6, 0x90, 0x27, 0xCE, 0x40, 0x27, 0xF6, 0x40, 0x32, 0x4B, 0x70,
    0x78, 0x81, 0x1F, 0x57, 0x32, 0xD1, 0x70, 0x3E, 0x53, 0x32, 0xE8, 0x70,
    0xCD, 0xD0, 0x7E, 0x7B, 0x0F, 0x0F, 0x0F, 0x0F, 0xE6, 0x0F, 0xC6, 0x90,
    0x27, 0xCE, 0x40, 0x27, 0xF6, 0x40, 0x32, 0x64, 0x70, 0x7B, 0xE6, 0x0F,
    0xC6, 0x90, 0x27, 0xCE, 0x40, 0x27, 0xF6, 0x40, 0x32, 0x65, 0x70, 0x7B,
    0xF5, 0xCD, 0xD0, 0x7E, 0x7B, 0x0F, 0x0F, 0x0F, 0x0F, 0xE6, 0x0F, 0xC6,
    0x90, 0x27, 0xCE, 0x40, 0x27, 0xF6, 0x40, 0x32, 0x67, 0x70, 0x7B, 0xE6,
    0x0F, 0xC6, 0x90, 0x27, 0xCE, 0x40, 0x27, 0xF6, 0x40, 0x32, 0x68, 0x70,
    0x6B, 0xCD, 0xD0, 0x7E, 0x7B, 0x0F, 0x0F, 0x0F, 0x0F, 0xE6, 0x0F, 0xC6,
    0x90, 0x27, 0xCE, 0x40, 0x27, 0xF6, 0x40, 0x32, 0x6A, 0x70, 0x7B, 0xE6,
    0x0F, 0xC6, 0x90, 0x27, 0xCE, 0x40, 0x27, 0xF6, 0x40, 0x32, 0x6B, 0x70,
    0x63, 0xE5, 0xCD, 0xD0, 0x7E, 0x7B, 0x0F, 0x0F, 0x0F, 0x0F, 0xE6, 0x0F,
    0xC6, 0x90, 0x27, 0xCE, 0x40, 0x27, 0xF6, 0x40, 0x32, 0x6D, 0x70, 0x7B,
    0xE6, 0x0F, 0xC6, 0x90, 0x27, 0xCE, 0x40, 0x27, 0xF6, 0x40, 0x32, 0x6E,
    0x70, 0x4B, 0xCD, 0xD0, 0x7E, 0x7B, 0x0F, 0x0F, 0x0F, 0x0F, 0xE6, 0x0F,
    0xC6, 0x90, 0x27, 0xCE, 0x40, 0x27, 0xF6, 0x40, 0x32, 0x70, 0x70, 0x7B,
    0xE6, 0x0F, 0xC6, 0x90, 0x27, 0xCE, 0x40, 0x27, 0xF6, 0x40, 0x32, 0x71,
    0x70, 0x43, 0xC5, 0x3E, 0x48, 0x32, 0xE9, 0x70, 0xCD, 0xD0, 0x7E, 0x18,
    0x03, 0xCD, 0xD6, 0x7E, 0x73, 0x23, 0x0B, 0x79, 0x32, 0x87, 0x70, 0x78,
    0x32, 0x86, 0x70, 0xB1, 0x20, 0xEF, 0xCD, 0xD6, 0x7E, 0xD5, 0xCD, 0xDC,
    0x7E, 0xD5, 0xC1, 0xD1, 0xD9, 0x3E, 0x42, 0x32, 0xEA, 0x70, 0xC1, 0xD1,
    0xD5, 0x62, 0x6B, 0x09, 0x22, 0xF9, 0x78, 0x7C, 0x0F, 0x0F, 0x0F, 0x0F,
    0xE6, 0x0F, 0xC6, 0x90, 0x27, 0xCE, 0x40, 0x27, 0xF6, 0x40, 0x32, 0xA6,
    0x70, 0x7C, 0xE6, 0x0F, 0xC6, 0x90, 0x27, 0xCE, 0x40, 0x27, 0xF6, 0x40,
    0x32, 0xA7, 0x70, 0x7D, 0x0F, 0x0F, 0x0F, 0x0F, 0xE6, 0x0F, 0xC6, 0x90,
    0x27, 0xCE, 0x40, 0x27, 0xF6, 0x40, 0x32, 0xA8, 0x70, 0x7D, 0xE6, 0x0F,
    0xC6, 0x90, 0x27, 0xCE, 0x40, 0x27, 0xF6, 0x40, 0x32, 0xA9, 0x70, 0xE1,
    0xCD, 0xF4, 0x7E, 0x38, 0x0C, 0x3E, 0x4F, 0x32, 0xB2, 0x70, 0x3E, 0x4B,
    0x32, 0xB3, 0x70, 0x18, 0x12, 0x3E, 0x42, 0x32, 0xB2, 0x70, 0x3E, 0x41,
    0x32, 0xB3, 0x70, 0x3E, 0x44, 0x32, 0xB4, 0x70, 0x76, 0x18, 0xFD, 0x3A,
    0xD1, 0x70, 0x4F, 0x79, 0x0F, 0x0F, 0x0F, 0x0F, 0xE6, 0x0F, 0xC6, 0x90,
    0x27, 0xCE, 0x40, 0x27, 0xF6, 0x40, 0x32, 0xCE, 0x70, 0x79, 0xE6, 0x0F,
    0xC6, 0x90, 0x27, 0xCE, 0x40, 0x27, 0xF6, 0x40, 0x32, 0xCF, 0x70, 0x3E,
    0x60, 0x32, 0xD1, 0x70, 0x3E, 0x4F, 0x32, 0x86, 0x70, 0x3E, 0x4B, 0x32,
    0x87, 0x70, 0x3E, 0x44, 0x32, 0xEB, 0x70, 0xD5, 0x01, 0x00, 0x00, 0x16,
    0x04, 0x0B, 0x78, 0xB1, 0x20, 0xFB, 0x15, 0x20, 0xF8, 0xD1, 0xF1, 0xFE,
    0xF1, 0x28, 0x1F, 0x22, 0xA4, 0x78, 0xCD, 0xFC, 0x1A, 0xCD, 0x5D, 0x1B,
    0xFB, 0x21, 0xE8, 0x79, 0x36, 0x52, 0x23, 0x36, 0x55, 0x23, 0x36, 0x4E,
    0x23, 0x36, 0x00, 0x21, 0xE7, 0x79, 0xAF, 0xC3, 0x81, 0x1A, 0xFB, 0xE9,
    0x1E, 0x01, 0x3E, 0x03, 0x18, 0x09, 0x1E, 0x01, 0x3E, 0x01, 0x18, 0x03,
    0x1E, 0x01, 0xAF, 0x3C, 0xFD, 0xCB, 0x00, 0x76, 0x20, 0xF9, 0x3C, 0xFD,
    0xCB, 0x00, 0x76, 0x28, 0xF9, 0xBA, 0x3F, 0xCB, 0x13, 0x30, 0xEB, 0xC9,
    0xC5, 0xD5, 0xE5, 0x7B, 0x82, 0x6F, 0x3E, 0x00, 0xCE, 0x00, 0x67, 0x1A,
    0x85, 0x6F, 0x7C, 0xCE, 0x00, 0x67, 0x13, 0x0B, 0x78, 0xB1, 0x20, 0xF3,
    0x3A, 0xF9, 0x78, 0x85, 0x6F, 0x7C, 0xCE, 0x00, 0x67, 0x3A, 0xFA, 0x78,
    0x85, 0x6F, 0x7C, 0xCE, 0x00, 0x67, 0xD9, 0x7B, 0xD9, 0xBD, 0x20, 0x0B,
    0xD9, 0x79, 0xD9, 0xBC, 0x20, 0x05, 0xE1, 0xD1, 0xC1, 0xB7, 0xC9, 0xE1,
    0xD1, 0xC1, 0x37, 0xC9, 0x7E, 0xB7, 0xC8, 0xF6, 0x40, 0x12, 0x23, 0x13,
    0x18, 0xF6, 0x46, 0x41, 0x53, 0x54, 0x4C, 0x4F, 0x41, 0x44, 0x20, 0x44,
    0x45, 0x42, 0x55, 0x47, 0x20, 0x56, 0x33, 0x00, 0x53, 0x59, 0x4E, 0x43,
    0x20, 0x4C, 0x41, 0x53, 0x54, 0x3A, 0x00, 0x53, 0x59, 0x4E, 0x43, 0x20,
    0x45, 0x4E, 0x44, 0x20, 0x3A, 0x00, 0x48, 0x44, 0x52, 0x3A, 0x00, 0x42,
    0x4F, 0x44, 0x59, 0x3A, 0x00, 0x45, 0x4E, 0x44, 0x3A, 0x00, 0x43, 0x4B,
    0x53, 0x55, 0x4D, 0x3A, 0x00, 0x53, 0x48, 0x4F, 0x52, 0x54, 0x3A, 0x00,
    0x54, 0x48, 0x52, 0x3A, 0x00, 0x53, 0x54, 0x41, 0x47, 0x45, 0x3A, 0x20,
    0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x00
};

/* Offsets of 16-bit absolute words that move with the load address (found by assembling at two origins). */
static const uint16_t FAST_DEBUG_RELOCS[30] = {
    22, 28, 31, 37, 40, 46, 49, 55, 58, 64, 67, 73,
    76, 82, 85, 91, 94, 100, 103, 109, 277, 314, 350, 387,
    423, 465, 470, 487, 491, 577
};

#define FAST_DEBUG_STACK_RESERVE    128u
#define FAST_STUB_BUF_MAX           sizeof(FAST_DEBUG_STUB)

/*
 * Chooses where the bootstrap stub (production or debug) lives and relocates a copy of it into 'stub'.
 * Placement rules are the same for both: default RAM at 0x7AE9, moved above the payload when they overlap,
 * or the reserved video-RAM area with --fast-vram. Returns 0 on success, -1 if it cannot be placed.
 */
static int prepare_fast_boot_stub(int debug, uint16_t payload_addr, uint32_t payload_len,
                                  uint8_t *stub, uint32_t *stub_len_out,
                                  uint16_t *stub_addr, uint16_t *stack_top,
                                  int use_video_ram, uint32_t ram_end)
{
    const uint8_t *image = debug ? FAST_DEBUG_STUB : FAST_BOOT_STUB;
    const uint32_t size = debug ? (uint32_t)sizeof(FAST_DEBUG_STUB)
                                : (uint32_t)sizeof(FAST_BOOT_STUB);
    const uint32_t reserve = debug ? FAST_DEBUG_STACK_RESERVE : FAST_BOOT_STACK_RESERVE;
    const uint32_t payload_end = (uint32_t)payload_addr + payload_len;
    const uint32_t footprint = size + reserve;
    uint32_t candidate = use_video_ram ? FAST_BOOT_VRAM_LOADER_START :
                                         FAST_BOOT_STUB_LOAD_ADDR;
    uint32_t stack_low;
    uint32_t i;
    unsigned read_bit_calls = 0;
    unsigned checksum_calls = 0;

    if (payload_end > 0x10000u)
        return -1;

    if (use_video_ram) {
        stack_low = FAST_BOOT_VRAM_STACK_TOP - reserve;
        if (candidate + size > FAST_BOOT_VRAM_STACK_START ||
            memory_range_overlaps(payload_addr, payload_len,
                                  candidate,
                                  candidate + size) ||
            memory_range_overlaps(payload_addr, payload_len,
                                  stack_low, FAST_BOOT_VRAM_STACK_TOP))
            return -1;
        memcpy(stub, image, size);
        if (stub[1] != 0x31u)
            return -1;
        *stack_top = FAST_BOOT_VRAM_STACK_TOP;
    } else {
        if (memory_range_overlaps(payload_addr, payload_len,
                                  candidate, candidate + footprint)) {
            candidate = (payload_end + 15u) & ~15u;
        }

        if (candidate + footprint > ram_end ||
            memory_range_overlaps(payload_addr, payload_len,
                                  candidate, candidate + footprint))
            return -1;

        memcpy(stub, image, size);
        if (stub[1] != 0x31u)
            return -1;
        *stack_top = (uint16_t)(candidate + footprint);
    }
    stub[2] = (uint8_t)(*stack_top & 0xFFu);
    stub[3] = (uint8_t)(*stack_top >> 8);

    if (debug) {
        /* Table-driven relocation: add the load-address delta to every recorded 16-bit word. */
        uint16_t delta = (uint16_t)(candidate - FAST_DEBUG_STUB_BASE);
        for (i = 0; i < sizeof(FAST_DEBUG_RELOCS) / sizeof(FAST_DEBUG_RELOCS[0]); i++) {
            uint16_t site = FAST_DEBUG_RELOCS[i];
            uint16_t word = (uint16_t)((uint16_t)stub[site] | ((uint16_t)stub[site + 1u] << 8));
            word = (uint16_t)(word + delta);
            stub[site]      = (uint8_t)(word & 0xFFu);
            stub[site + 1u] = (uint8_t)(word >> 8);
        }
    } else {
        /* Relative branches relocate as-is; rewrite only absolute internal CALLs. */
        for (i = 0; i + 2u < sizeof(FAST_BOOT_STUB); i++) {
            uint16_t target;

            if (stub[i] != 0xCDu)
                continue;
            target = (uint16_t)((uint16_t)stub[i + 1u] |
                                ((uint16_t)stub[i + 2u] << 8));
            if (target == FAST_BOOT_STUB_LOAD_ADDR + FAST_BOOT_READ_BIT_OFFSET) {
                target = (uint16_t)(candidate + FAST_BOOT_READ_BIT_OFFSET);
                read_bit_calls++;
            } else if (target == FAST_BOOT_STUB_LOAD_ADDR + FAST_BOOT_CHECKSUM_OFFSET) {
                target = (uint16_t)(candidate + FAST_BOOT_CHECKSUM_OFFSET);
                checksum_calls++;
            } else {
                continue;
            }
            stub[i + 1u] = (uint8_t)(target & 0xFFu);
            stub[i + 2u] = (uint8_t)(target >> 8);
        }

        if (read_bit_calls != 8u || checksum_calls != 1u)
            return -1;
    }

    *stub_len_out = size;
    *stub_addr = (uint16_t)candidate;
    return 0;
}

/* -------------------------------------------------------------------------
 * Borland C artifact padding
 * ------------------------------------------------------------------------- */

static const uint8_t BORLAND_ARTIFACT_PADDING[PADDING_RAW_BYTES] = {
    0x40, 0x41, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A,
    0x4B, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53,
    0x53, 0x54, 0x55, 0x56, 0x57, 0x57, 0x58, 0x59, 0x59, 0x5A,
    0x5B, 0x5C, 0x5C, 0x5D, 0x5E, 0x5E, 0x5F, 0x60, 0x60, 0x61,
    0x61, 0x62, 0x63, 0x63, 0x64, 0x65, 0x65, 0x66, 0x67, 0x68,
    0x68, 0x68, 0x69, 0x69, 0x6A, 0x6B, 0x6B, 0x6C, 0x6C, 0x6D,
    0x6D, 0x6D, 0x6D, 0x6E, 0x6E, 0x6F, 0x6F, 0x70, 0x6F, 0x79,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* -------------------------------------------------------------------------
 * WAV header helpers
 * ------------------------------------------------------------------------- */

static void put16le(unsigned char *buf, uint16_t v)
{
    buf[0] = (unsigned char)( v       & 0xFF);
    buf[1] = (unsigned char)((v >> 8) & 0xFF);
}

static void put32le(unsigned char *buf, uint32_t v)
{
    buf[0] = (unsigned char)( v        & 0xFF);
    buf[1] = (unsigned char)((v >>  8) & 0xFF);
    buf[2] = (unsigned char)((v >> 16) & 0xFF);
    buf[3] = (unsigned char)((v >> 24) & 0xFF);
}

/* Build a 44-byte PCM WAV header. */
static void build_wav_header(unsigned char hdr[44], uint32_t data_bytes, int compat_mode)
{
    memset(hdr, 0, 44);
    memcpy(hdr +  0, "RIFF", 4);
    if (compat_mode) {
        put32le(hdr + 4, 0x00146e0c);
    } else if (data_bytes) {
        put32le(hdr + 4, data_bytes + 36);
    }
    memcpy(hdr +  8, "WAVE", 4);
    memcpy(hdr + 12, "fmt ", 4);
    put32le(hdr + 16, 16);
    put16le(hdr + 20, 1);
    put16le(hdr + 22, 1);
    put32le(hdr + 24, (uint32_t)SAMPLE_RATE);
    put32le(hdr + 28, (uint32_t)SAMPLE_RATE);
    put16le(hdr + 32, 1);
    put16le(hdr + 34, 8);
    memcpy(hdr + 36, "data", 4);
    if (compat_mode) {
        put32le(hdr + 40, 0x01140000);
    } else if (data_bytes) {
        put32le(hdr + 40, data_bytes);
    }
}

/* -------------------------------------------------------------------------
 * Tape encoding primitives
 * ------------------------------------------------------------------------- */

static int write_bit(FILE *out, int bit)
{
    int i;
    const unsigned char *src;
    int gain_num = 100 + g_output_gain_percent;

    switch (g_waveform_mode) {
    case WAVEFORM_SQUARE:
        src = bit ? BIT1_WAVE_SQUARE : BIT0_WAVE_SQUARE;
        break;
    case WAVEFORM_SINE:
        src = bit ? BIT1_WAVE_SINE : BIT0_WAVE_SINE;
        break;
    case WAVEFORM_DOS:
    default:
        src = bit ? BIT1_WAVE_DOS : BIT0_WAVE_DOS;
        break;
    }

    for (i = 0; i < SAMPLES_PER_BIT; i++) {
        int centered = (int)src[i] - SIGNAL_CENTER;
        int scaled = SIGNAL_CENTER + (centered * gain_num) / 100;
        if (scaled < 0) scaled = 0;
        if (scaled > 255) scaled = 255;
        if (fputc((unsigned char)scaled, out) == EOF)
            return -1;
    }
    return 0;
}

static int parse_gain_percent(const char *s, int *out)
{
    char *end = NULL;
    long v;
    if (!s || !*s)
        return -1;
    v = strtol(s, &end, 10);
    if (*end != '\0')
        return -1;
    if (v < MIN_GAIN_PERCENT || v > MAX_GAIN_PERCENT)
        return -1;
    *out = (int)v;
    return 0;
}

static int parse_waveform_mode(const char *s, WaveformMode *out)
{
    if (!s || !*s)
        return -1;
    if (strcmp(s, "dos") == 0 || strcmp(s, "default") == 0) {
        *out = WAVEFORM_DOS;
        return 0;
    }
    if (strcmp(s, "square") == 0 || strcmp(s, "sq") == 0) {
        *out = WAVEFORM_SQUARE;
        return 0;
    }
    if (strcmp(s, "sine") == 0 || strcmp(s, "sin") == 0) {
        *out = WAVEFORM_SINE;
        return 0;
    }
    return -1;
}

static int write_vz_byte(FILE *out, unsigned char val)
{
    int i;
    for (i = 7; i >= 0; i--)
        if (write_bit(out, (val >> i) & 1) < 0)
            return -1;
    return 0;
}

static int write_fast_bit(FILE *out, int bit)
{
    /*
     * 2018AD protocol at 22050 Hz:
     * Bit 0 (short): 2 low samples (61), 2 high samples (195) = 4 samples
     * Bit 1 (long) : 4 low samples (61), 4 high samples (195) = 8 samples
     * Respects g_output_gain_percent around SIGNAL_CENTER (127).
     */
    int half_samples = bit ? 4 : 2;
    int gain_num = 100 + g_output_gain_percent;
    int lo_val, hi_val;
    int i;

    int lo_c = (int)61 - SIGNAL_CENTER;
    int hi_c = (int)195 - SIGNAL_CENTER;
    lo_val = SIGNAL_CENTER + (lo_c * gain_num) / 100;
    hi_val = SIGNAL_CENTER + (hi_c * gain_num) / 100;
    if (lo_val < 0) lo_val = 0;
    if (lo_val > 255) lo_val = 255;
    if (hi_val < 0) hi_val = 0;
    if (hi_val > 255) hi_val = 255;

    if (g_fast_invert) {
        /* inverted phase: high half first, then low half */
        int tmp = lo_val;
        lo_val = hi_val;
        hi_val = tmp;
    }
    for (i = 0; i < half_samples; i++) {
        if (fputc((unsigned char)lo_val, out) == EOF) return -1;
    }
    for (i = 0; i < half_samples; i++) {
        if (fputc((unsigned char)hi_val, out) == EOF) return -1;
    }
    return 0;
}

static int write_fast_byte(FILE *out, unsigned char val)
{
    int i;
    for (i = 7; i >= 0; i--) {
        if (write_fast_bit(out, (val >> i) & 1) < 0)
            return -1;
    }
    return 0;
}

static int write_raw(FILE *out, unsigned char fill, uint32_t count)
{
    uint32_t i;
    for (i = 0; i < count; i++)
        if (fputc(fill, out) == EOF)
            return -1;
    return 0;
}

static int write_artifact_padding(FILE *out)
{
    size_t n = fwrite(BORLAND_ARTIFACT_PADDING, 1, PADDING_RAW_BYTES, out);
    return (n == (size_t)PADDING_RAW_BYTES) ? 0 : -1;
}

static void print_usage(void)
{
    fprintf(stderr,
        "vz2wav v%s - Convert VZ-200/VZ-300 tape image to WAV audio\n"
        "Usage: vz2wav [options] <input.vz> <output.wav>\n"
        "\n"
        "Options:\n"
        "  --waveform,-w <dos|square|sine>\n"
        "                Select waveform profile (default: dos).\n"
        "  --square      Shorthand for --waveform square (clean square wave).\n"
        "  --sine,--sin  Shorthand for --waveform sine (clean sine wave).\n"
        "  --fast,-f     Emit 2018AD speedload audio with self-booting stub (~12.5x speed).\n"
        "  --fast-vram   Place the self-booting loader and stack in unused video RAM.\n"
        "  --fast-flat-ram  Allow --fast relocation into flat RAM through 0xFFFF.\n"
        "  --fast-raw    Emit raw 2018AD speedload audio block without bootstrap stub.\n"
        "  --fast-debug  Like --fast, but the loader paints a live diagnostic panel on screen (sync\n"
        "                counts, received header, checksum). Combines with --fast-vram/--fast-flat-ram.\n"
        "  --fast-invert Fast block phase: high half first (DEFAULT; verified on real VZ200 hardware).\n"
        "  --fast-normal Fast block phase: low half first (original phase). Applies to every fast mode.\n"
        "  --compat,-c   Write a malformed WAV matching the original DOS program\n"
        "                (RIFF and data size fields use raw DOS stack garbage).\n"
        "  --artifact,-a Write the exact Borland C uninitialized stack bytes into\n"
        "                the 80-byte padding gap instead of 0x7F silence.\n"
        "  --robust,-r   Use longer settle/leader/sync timing for noisy analog paths.\n"
        "  --gain,-g N   Amplitude delta in percent (range -90..300, default +10).\n"
        "  --version,-V  Print version and exit.\n",
        TOOL_VERSION);
}

/* -------------------------------------------------------------------------
 * main
 * ------------------------------------------------------------------------- */

int main(int argc, char *argv[])
{
    const char    *arg_input  = NULL;
    const char    *arg_output = NULL;
    int            compat_mode   = 0;
    int            artifact_mode = 0;
    int            robust_mode   = 0;
    int            fast_vram_mode = 0;
    int            fast_flat_ram_mode = 0;
    int            fast_mode     = 0;  /* 1 = self-booting two-stage fast WAV, 2 = raw fast block */
    int            fast_debug    = 0;  /* 1 = verbose diagnostic bootstrap stub */

    FILE          *fin  = NULL;
    FILE          *fout = NULL;
    unsigned char *body = NULL;
    int            ret  = 1;
    int            i;

    unsigned char  vz_hdr[VZ_HEADER_SIZE];
    unsigned char  filename[FILENAME_FIELD_LEN + 1];
    uint8_t        file_type, addr_lo, addr_hi;
    long           file_size;
    uint32_t       body_len;
    uint16_t       load_addr, end_addr;
    uint8_t        end_addr_lo, end_addr_hi;
    uint8_t        fast_boot_stub[FAST_STUB_BUF_MAX];
    uint32_t       fast_stub_len = (uint32_t)sizeof(FAST_BOOT_STUB);
    uint16_t       fast_stub_addr = FAST_BOOT_STUB_LOAD_ADDR;
    uint16_t       fast_stub_stack_top = 0;
    uint32_t       fast_stub_required_top = 0;
    uint32_t       checksum;
    uint8_t        cksum_lo, cksum_hi;
    uint32_t       fn_write_len;
    uint32_t       total_samples;
    uint32_t       pre_silence_samples;
    uint32_t       post_silence_samples;
    uint32_t       leader_count;
    uint32_t       sync_count;
    unsigned char  wav_hdr[44];

    /* Argument parsing */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-V") == 0) {
            printf("vz2wav version %s\n", TOOL_VERSION);
            return 0;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage();
            return 0;
        }
        if (strcmp(argv[i], "--compat") == 0 || strcmp(argv[i], "-c") == 0)
            compat_mode = 1;
        else if (strcmp(argv[i], "--artifact") == 0 || strcmp(argv[i], "-a") == 0)
            artifact_mode = 1;
        else if (strcmp(argv[i], "--robust") == 0 || strcmp(argv[i], "-r") == 0)
            robust_mode = 1;
        else if (strcmp(argv[i], "--fast") == 0 || strcmp(argv[i], "-f") == 0)
            fast_mode = 1;
        else if (strcmp(argv[i], "--fast-vram") == 0) {
            fast_mode = 1;
            fast_vram_mode = 1;
        }
        else if (strcmp(argv[i], "--fast-flat-ram") == 0) {
            fast_mode = 1;
            fast_flat_ram_mode = 1;
        }
        else if (strcmp(argv[i], "--fast-raw") == 0) {
            fast_mode = 2;
            fast_vram_mode = 0;
            fast_flat_ram_mode = 0;
        }
        else if (strcmp(argv[i], "--fast-debug") == 0) {
            fast_mode = 1;
            fast_debug = 1;
        }
        else if (strcmp(argv[i], "--fast-invert") == 0)
            g_fast_invert = 1;   /* default; accepted for explicitness */
        else if (strcmp(argv[i], "--fast-normal") == 0)
            g_fast_invert = 0;
        else if (strcmp(argv[i], "--gain") == 0 || strcmp(argv[i], "-g") == 0) {
            if (i + 1 >= argc || parse_gain_percent(argv[++i], &g_output_gain_percent) != 0) {
                fprintf(stderr, "vz2wav: invalid --gain value\n");
                goto usage;
            }
        }
        else if (strncmp(argv[i], "--gain=", 7) == 0) {
            if (parse_gain_percent(argv[i] + 7, &g_output_gain_percent) != 0) {
                fprintf(stderr, "vz2wav: invalid --gain value\n");
                goto usage;
            }
        }
        else if (strcmp(argv[i], "--waveform") == 0 || strcmp(argv[i], "-w") == 0) {
            if (i + 1 >= argc || parse_waveform_mode(argv[++i], &g_waveform_mode) != 0) {
                fprintf(stderr, "vz2wav: invalid --waveform value (must be dos, square, or sine)\n");
                goto usage;
            }
        }
        else if (strncmp(argv[i], "--waveform=", 11) == 0) {
            if (parse_waveform_mode(argv[i] + 11, &g_waveform_mode) != 0) {
                fprintf(stderr, "vz2wav: invalid --waveform value (must be dos, square, or sine)\n");
                goto usage;
            }
        }
        else if (strcmp(argv[i], "--square") == 0) {
            g_waveform_mode = WAVEFORM_SQUARE;
        }
        else if (strcmp(argv[i], "--sine") == 0 || strcmp(argv[i], "--sin") == 0) {
            g_waveform_mode = WAVEFORM_SINE;
        }
        else if (!arg_input)
            arg_input = argv[i];
        else if (!arg_output)
            arg_output = argv[i];
        else {
            fprintf(stderr, "vz2wav: unexpected argument '%s'\n", argv[i]);
            goto usage;
        }
    }
    if (!arg_input || !arg_output) {
usage:
        print_usage();
        return 1;
    }
    if (fast_vram_mode && fast_flat_ram_mode) {
        fprintf(stderr, "vz2wav: --fast-vram and --fast-flat-ram are mutually exclusive\n");
        goto done;
    }

    printf("\nvz2wav - VZ tape image to WAV converter\n");
    printf("Mode: %s%s%s(%s wave)\n\n",
           compat_mode   ? "compat (malformed WAV header) " : "clean  (standards WAV header) ",
           artifact_mode ? "+ artifact padding "            : "",
           robust_mode   ? "+ robust timing "               : "",
           g_waveform_mode == WAVEFORM_SQUARE ? "square" :
           g_waveform_mode == WAVEFORM_SINE   ? "sine" : "DOS");

    pre_silence_samples  = robust_mode ? (uint32_t)ROBUST_PRE_SILENCE_SAMPLES  : (uint32_t)SILENCE_SAMPLES;
    post_silence_samples = robust_mode ? (uint32_t)ROBUST_POST_SILENCE_SAMPLES : (uint32_t)SILENCE_SAMPLES;
    leader_count         = robust_mode ? (uint32_t)ROBUST_LEADER_COUNT         : (uint32_t)LEADER_COUNT;
    sync_count           = robust_mode ? (uint32_t)ROBUST_SYNC_COUNT           : (uint32_t)SYNC_COUNT;

    /* Read VZ file */
    fin = fopen(arg_input, "rb");
    if (!fin) {
        fprintf(stderr, "vz2wav: cannot open '%s'\n", arg_input);
        goto done;
    }
    if (fread(vz_hdr, 1, VZ_HEADER_SIZE, fin) != VZ_HEADER_SIZE) {
        fprintf(stderr, "vz2wav: '%s' is too short (need %d bytes for header)\n",
                arg_input, VZ_HEADER_SIZE);
        goto done;
    }

    /* Magic check (warn only) */
    {
        static const unsigned char M0[] = {0x56,0x5A,0x46,0x30};
        static const unsigned char M1[] = {0x20,0x20,0x00,0x00};
        static const unsigned char M2[] = {0x56,0x5A,0x46,0x4F};
        if (memcmp(vz_hdr, M0, 4) != 0 &&
            memcmp(vz_hdr, M1, 4) != 0 &&
            memcmp(vz_hdr, M2, 4) != 0)
            fprintf(stderr, "vz2wav: warning: unrecognised magic in '%s'\n", arg_input);
    }

    /* Unpack header fields */
    memcpy(filename, vz_hdr + 4, FILENAME_FIELD_LEN);
    filename[FILENAME_FIELD_LEN] = '\0';
    file_type = vz_hdr[21];
    addr_lo   = vz_hdr[22];
    addr_hi   = vz_hdr[23];
    load_addr = (uint16_t)((unsigned)addr_hi << 8 | addr_lo);

    /* Read body size safely */
    if (fseek(fin, 0, SEEK_END) != 0) {
        fprintf(stderr, "vz2wav: cannot seek '%s'\n", arg_input);
        goto done;
    }
    file_size = ftell(fin);
    if (file_size < 0) {
        fprintf(stderr, "vz2wav: cannot seek '%s'\n", arg_input);
        goto done;
    }
    if (file_size <= (long)VZ_HEADER_SIZE) {
        fprintf(stderr, "vz2wav: '%s' has no body data\n", arg_input);
        goto done;
    }

    /* Now safe to convert to uint32_t (VZ body is practically <= 65535) */
    body_len = (uint32_t)(file_size - (long)VZ_HEADER_SIZE);

    body = (unsigned char *)malloc((size_t)body_len);
    if (!body) {
        fprintf(stderr, "vz2wav: out of memory\n");
        goto done;
    }
    if (fseek(fin, VZ_HEADER_SIZE, SEEK_SET) != 0) {
        fprintf(stderr, "vz2wav: cannot seek '%s'\n", arg_input);
        goto done;
    }
    if (fread(body, 1, (size_t)body_len, fin) != (size_t)body_len) {
        fprintf(stderr, "vz2wav: read error on '%s'\n", arg_input);
        goto done;
    }
    fclose(fin); fin = NULL;

    /* Info */
    printf("Input : %s\n", arg_input);
    printf("  Filename  : %.16s\n", (char *)filename);
    printf("  File type : %s (0x%02" PRIX8 ")\n",
           file_type == 0xF0 ? "BASIC" :
           file_type == 0xF1 ? "Machine code" : "Unknown",
           file_type);
    printf("  Load addr : 0x%04" PRIX16 "\n", load_addr);
    printf("  Body size : %" PRIu32 " bytes\n", body_len);

    /* Compute tape header fields */
    end_addr    = (uint16_t)(load_addr + body_len);
    end_addr_lo = (uint8_t)( end_addr       & 0xFF);
    end_addr_hi = (uint8_t)((end_addr >> 8) & 0xFF);

    if (fast_mode == 1) {
        uint32_t payload_end = (uint32_t)load_addr + body_len;
        if (prepare_fast_boot_stub(fast_debug, load_addr, body_len, fast_boot_stub,
                                   &fast_stub_len,
                                   &fast_stub_addr, &fast_stub_stack_top,
                                   fast_vram_mode,
                                   fast_flat_ram_mode ? FAST_BOOT_FLAT_RAM_END :
                                                        FAST_BOOT_RAM_END) != 0) {
            fprintf(stderr,
                    fast_vram_mode
                        ? "vz2wav: --fast-vram payload overlaps the reserved video-RAM "
                          "loader/stack area\n"
                        : fast_flat_ram_mode
                            ? "vz2wav: --fast-flat-ram cannot place bootstrap and stack "
                              "without overlapping the payload within RAM through 0xFFFF\n"
                            : "vz2wav: --fast cannot place bootstrap and stack without "
                              "overlapping the payload within RAM below 0xC000\n");
            goto done;
        }
        fast_stub_required_top = payload_end;
        if (!fast_vram_mode && fast_stub_required_top < fast_stub_stack_top)
            fast_stub_required_top = fast_stub_stack_top;
        if (fast_stub_required_top < FAST_BOOT_RAM_START)
            fast_stub_required_top = FAST_BOOT_RAM_START;
    }

    checksum = (uint32_t)addr_lo + addr_hi + end_addr_lo + end_addr_hi;
    for (i = 0; i < (int)body_len; i++)
        checksum += body[i];
    cksum_lo = (uint8_t)( checksum       & 0xFF);
    cksum_hi = (uint8_t)((checksum >> 8) & 0xFF);

    fn_write_len = 0;
    while (fn_write_len < (uint32_t)FILENAME_FIELD_LEN) {
        fn_write_len++;
        if (filename[fn_write_len - 1] == '\0') break;
    }
    if (fn_write_len == (uint32_t)FILENAME_FIELD_LEN)
        fn_write_len++;

    printf("  End addr  : 0x%04" PRIX16 "\n", end_addr);
    if (fast_mode == 1) {
        printf("  Fast loader%s (%" PRIu32 "KB): 0x%04" PRIX16 "-0x%04" PRIX32
               " (stack top 0x%04" PRIX16 "%s%s)\n",
               fast_debug ? " DEBUG" : "",
               (fast_stub_len + 1023u) / 1024u,
               fast_stub_addr,
               (uint32_t)fast_stub_addr + fast_stub_len - 1u,
               fast_stub_stack_top,
               fast_vram_mode ? ", video RAM" : "",
               (!fast_vram_mode && fast_stub_stack_top == 0) ?
                   ", wraps at 0xFFFF" : "");
        printf("  Minimum RAM (%" PRIu32 "KB): through 0x%04" PRIX32
               " inclusive\n",
               (fast_stub_required_top - FAST_BOOT_RAM_START + 1023u) / 1024u,
               fast_stub_required_top - 1u);
    }
    printf("  Checksum  : 0x%02" PRIX8 "%02" PRIX8 "\n", cksum_hi, cksum_lo);
    if (fast_mode != 0)
        printf("  Fast phase: %s\n", g_fast_invert ? "inverted (default)" : "normal (--fast-normal)");
    printf("\n");

    /*
     * For fast_mode, compute exact sample count of the fast payload.
     * Header bytes: 1 byte file_type, 2 bytes addr, 2 bytes body_len.
     * In 2018AD speedload, bit 0 is 4 samples, bit 1 is 8 samples.
     */
    uint32_t fast_data_len_samples = 0;
    if (fast_mode != 0) {
        uint8_t fast_hdr[5];
        uint32_t b_idx;
        int bit_idx;

        fast_hdr[0] = file_type;
        fast_hdr[1] = addr_lo;
        fast_hdr[2] = addr_hi;
        fast_hdr[3] = (uint8_t)(body_len & 0xFF);
        fast_hdr[4] = (uint8_t)((body_len >> 8) & 0xFF);

        for (b_idx = 0; b_idx < 5; b_idx++) {
            for (bit_idx = 0; bit_idx < 8; bit_idx++) {
                int bit = (fast_hdr[b_idx] >> (7 - bit_idx)) & 1;
                fast_data_len_samples += bit ? 8u : 4u;
            }
        }
        for (b_idx = 0; b_idx < body_len; b_idx++) {
            for (bit_idx = 0; bit_idx < 8; bit_idx++) {
                int bit = (body[b_idx] >> (7 - bit_idx)) & 1;
                fast_data_len_samples += bit ? 8u : 4u;
            }
        }
        for (b_idx = 0; b_idx < 2; b_idx++) {
            uint8_t checksum_byte = b_idx == 0 ? cksum_lo : cksum_hi;
            for (bit_idx = 0; bit_idx < 8; bit_idx++) {
                int bit = (checksum_byte >> (7 - bit_idx)) & 1;
                fast_data_len_samples += bit ? 8u : 4u;
            }
        }
    }

    /* Compute total samples depending on fast_mode */
    if (fast_mode == 2) {
        /* Raw speedload block only */
        total_samples =
            pre_silence_samples
          + (uint32_t)FAST_SYNC_BYTE_COUNT * 8u * 4u   /* 7 bytes of 0x00 */
          + (7u * 4u + 1u * 8u)                        /* 1 byte of 0x01 */
          + (uint32_t)fast_data_len_samples
          + 4u                                         /* trailing leadout bit */
          + post_silence_samples;
    } else if (fast_mode == 1) {
        /*
         * Self-booting hybrid WAV:
         * Stage 1: Standard ROM tape encoding of the relocatable bootstrap stub.
         * Stage 2: 0.5s interstage silence gap
         * Stage 3: Speedload block containing the actual file payload
         */
        uint32_t stage1_body_len = fast_stub_len;
        total_samples =
            pre_silence_samples
          + leader_count           * 8 * SAMPLES_PER_BIT
          + sync_count             * 8 * SAMPLES_PER_BIT
          + (uint32_t)1            * 8 * SAMPLES_PER_BIT   /* type 0xF1 */
          + fn_write_len           * 8 * SAMPLES_PER_BIT   /* filename */
          + (uint32_t)PADDING_RAW_BYTES                    /* gap */
          + (uint32_t)4            * 8 * SAMPLES_PER_BIT   /* addresses */
          + stage1_body_len        * 8 * SAMPLES_PER_BIT   /* stub body */
          + (uint32_t)2            * 8 * SAMPLES_PER_BIT   /* checksum */
          + ((uint32_t)1           * 8 * SAMPLES_PER_BIT)  /* post cksum guard */
          + (uint32_t)FAST_INTERSTAGE_GAP_SAMPLES          /* interstage gap */
          + (uint32_t)FAST_SYNC_BYTE_COUNT * 8u * 4u       /* fast sync 7x 0x00 */
          + (7u * 4u + 1u * 8u)                            /* fast sync 0x01 */
          + (uint32_t)fast_data_len_samples                /* fast header + body */
          + 4u                                             /* trailing leadout bit */
          + post_silence_samples;
    } else {
        /* Standard VZ tape encoding */
        total_samples =
            pre_silence_samples
          + leader_count           * 8 * SAMPLES_PER_BIT
          + sync_count             * 8 * SAMPLES_PER_BIT
          + (uint32_t)1            * 8 * SAMPLES_PER_BIT
          + fn_write_len           * 8 * SAMPLES_PER_BIT
          + (uint32_t)PADDING_RAW_BYTES
          + (uint32_t)4            * 8 * SAMPLES_PER_BIT
          + body_len               * 8 * SAMPLES_PER_BIT
          + (uint32_t)2            * 8 * SAMPLES_PER_BIT
          + (compat_mode ? 0u : ((uint32_t)1 * 8u * (uint32_t)SAMPLES_PER_BIT))
          + post_silence_samples;
    }

    /* Open output */
    fout = fopen(arg_output, "wb");
    if (!fout) {
        fprintf(stderr, "vz2wav: cannot create '%s'\n", arg_output);
        goto done;
    }

    /* WAV header */
    build_wav_header(wav_hdr, compat_mode ? (uint32_t)0 : total_samples, compat_mode);
    if (fwrite(wav_hdr, 1, 44, fout) != (size_t)44) goto write_err;

    /* Pre-silence */
    if (write_raw(fout, SILENCE_BYTE, pre_silence_samples) < 0) goto write_err;

    if (fast_mode == 1) {
        /*
         * Write Stage 1: Standard ROM tape encoding with the fast bootstrap stub
         */
        uint16_t stub_load = fast_stub_addr;
        uint16_t stub_end  = (uint16_t)(stub_load + fast_stub_len);
        uint8_t  stub_load_lo = (uint8_t)(stub_load & 0xFF);
        uint8_t  stub_load_hi = (uint8_t)((stub_load >> 8) & 0xFF);
        uint8_t  stub_end_lo  = (uint8_t)(stub_end & 0xFF);
        uint8_t  stub_end_hi  = (uint8_t)((stub_end >> 8) & 0xFF);
        uint32_t stub_cksum = (uint32_t)stub_load_lo + stub_load_hi + stub_end_lo + stub_end_hi;
        uint32_t s_idx;

        for (s_idx = 0; s_idx < fast_stub_len; s_idx++)
            stub_cksum += fast_boot_stub[s_idx];

        /* Leader */
        for (i = 0; i < (int)leader_count; i++)
            if (write_vz_byte(fout, LEADER_BYTE) < 0) goto write_err;

        /* Sync preamble */
        for (i = 0; i < (int)sync_count; i++)
            if (write_vz_byte(fout, SYNC_BYTE) < 0) goto write_err;

        /* File type: Machine Code (0xF1) so BIOS immediately executes it */
        if (write_vz_byte(fout, 0xF1) < 0) goto write_err;

        /* Filename */
        {
            uint32_t j;
            for (j = 0; j < fn_write_len; j++)
                if (write_vz_byte(fout, filename[j]) < 0) goto write_err;
        }

        /* Padding gap */
        if (artifact_mode) {
            if (write_artifact_padding(fout) < 0) goto write_err;
        } else {
            if (write_raw(fout, SILENCE_BYTE, PADDING_RAW_BYTES) < 0) goto write_err;
        }

        /* Address block */
        if (write_vz_byte(fout, stub_load_lo) < 0) goto write_err;
        if (write_vz_byte(fout, stub_load_hi) < 0) goto write_err;
        if (write_vz_byte(fout, stub_end_lo)  < 0) goto write_err;
        if (write_vz_byte(fout, stub_end_hi)  < 0) goto write_err;

        /* Stub body */
        for (s_idx = 0; s_idx < fast_stub_len; s_idx++)
            if (write_vz_byte(fout, fast_boot_stub[s_idx]) < 0) goto write_err;

        /* Checksum */
        if (write_vz_byte(fout, (uint8_t)(stub_cksum & 0xFF)) < 0) goto write_err;
        if (write_vz_byte(fout, (uint8_t)((stub_cksum >> 8) & 0xFF)) < 0) goto write_err;
        if (write_vz_byte(fout, POST_CKSUM_GUARD) < 0) goto write_err;

        /* Interstage silence gap */
        if (write_raw(fout, SILENCE_BYTE, FAST_INTERSTAGE_GAP_SAMPLES) < 0) goto write_err;
    }

    if (fast_mode != 0) {
        /*
         * Write Speedload Block (2018AD protocol):
         * 1) 7 bytes of 0x00 + 1 byte of 0x01 sync sequence
         * 2) 1 byte file_type (0xF0 for BASIC, 0xF1 for MC)
         * 3) 2 bytes start address (addr_lo, addr_hi)
         * 4) 2 bytes length (body_len_lo, body_len_hi)
         * 5) body bytes
         * 6) 2-byte standard VZ checksum (little-endian)
         */
        uint32_t b_idx;
        for (i = 0; i < FAST_SYNC_BYTE_COUNT; i++)
            if (write_fast_byte(fout, 0x00) < 0) goto write_err;
        if (write_fast_byte(fout, 0x01) < 0) goto write_err;

        if (write_fast_byte(fout, file_type) < 0) goto write_err;
        if (write_fast_byte(fout, addr_lo) < 0) goto write_err;
        if (write_fast_byte(fout, addr_hi) < 0) goto write_err;
        if (write_fast_byte(fout, (uint8_t)(body_len & 0xFF)) < 0) goto write_err;
        if (write_fast_byte(fout, (uint8_t)((body_len >> 8) & 0xFF)) < 0) goto write_err;

        for (b_idx = 0; b_idx < body_len; b_idx++)
            if (write_fast_byte(fout, body[b_idx]) < 0) goto write_err;

        if (write_fast_byte(fout, cksum_lo) < 0 ||
            write_fast_byte(fout, cksum_hi) < 0) goto write_err;

        /* Emit one trailing short bit to complete the edge of the final bit */
        if (write_fast_bit(fout, 0) < 0) goto write_err;
    } else {
        /* Standard VZ audio stream */
        /* Leader */
        for (i = 0; i < (int)leader_count; i++)
            if (write_vz_byte(fout, LEADER_BYTE) < 0) goto write_err;

        /* Sync preamble */
        for (i = 0; i < (int)sync_count; i++)
            if (write_vz_byte(fout, SYNC_BYTE) < 0) goto write_err;

        /* File-type byte */
        if (write_vz_byte(fout, file_type) < 0) goto write_err;

        /* Filename */
        {
            uint32_t j;
            for (j = 0; j < fn_write_len; j++)
                if (write_vz_byte(fout, filename[j]) < 0) goto write_err;
        }

        /* Raw padding */
        if (artifact_mode) {
            if (write_artifact_padding(fout) < 0) goto write_err;
        } else {
            if (write_raw(fout, SILENCE_BYTE, PADDING_RAW_BYTES) < 0) goto write_err;
        }

        /* Address block */
        if (write_vz_byte(fout, addr_lo)     < 0) goto write_err;
        if (write_vz_byte(fout, addr_hi)     < 0) goto write_err;
        if (write_vz_byte(fout, end_addr_lo) < 0) goto write_err;
        if (write_vz_byte(fout, end_addr_hi) < 0) goto write_err;

        /* Body */
        {
            uint32_t j;
            for (j = 0; j < body_len; j++)
                if (write_vz_byte(fout, body[j]) < 0) goto write_err;
        }

        /* Checksum */
        if (write_vz_byte(fout, cksum_lo) < 0) goto write_err;
        if (write_vz_byte(fout, cksum_hi) < 0) goto write_err;
        if (!compat_mode && write_vz_byte(fout, POST_CKSUM_GUARD) < 0) goto write_err;
    }

    /* Post-silence */
    if (write_raw(fout, SILENCE_BYTE, post_silence_samples) < 0) goto write_err;

    printf("Output: %s\n", arg_output);
    printf("  Total audio : %" PRIu32 " samples (%.2f seconds)\n",
           total_samples, (double)total_samples / SAMPLE_RATE);
    printf("  WAV sizes   : %s\n",
           compat_mode ? "raw DOS garbage (compat mode -- matches DOS original)" : "correct");
    printf("  Padding     : %s\n",
           artifact_mode ? "Borland artifact bytes (--artifact)" : "0x7F silence (clean)");
    printf("  Timing      : %s (pre=%" PRIu32 ", leader=%" PRIu32 ", sync=%" PRIu32 ", post=%" PRIu32 ")\n",
           robust_mode ? "robust" : "normal",
           pre_silence_samples, leader_count, sync_count, post_silence_samples);
    printf("  Waveform    : %s\n",
           g_waveform_mode == WAVEFORM_SQUARE ? "square wave (--square)" :
           g_waveform_mode == WAVEFORM_SINE   ? "sine wave (--sine)" : "DOS original (reconstructed)");
    if (fast_mode == 1) {
        printf("  Loader      : fast 2018AD self-booting (ROM stub + ~12.5x speedload)\n");
    } else if (fast_mode == 2) {
        printf("  Loader      : fast 2018AD raw speedload block (~12.5x speedload)\n");
    } else {
        printf("  Loader      : standard VZ tape (~580 baud)\n");
    }
    printf("  Gain        : %+d%% (scale %.2fx)\n",
           g_output_gain_percent, (100.0 + (double)g_output_gain_percent) / 100.0);
    printf("\nDone.\n");

    ret = 0;
    goto done;

write_err:
    fprintf(stderr, "vz2wav: write error on '%s'\n", arg_output);

done:
    if (body) free(body);
    if (fin)  fclose(fin);
    if (fout) fclose(fout);
    return ret;
}
