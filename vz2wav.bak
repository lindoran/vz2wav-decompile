/*
 * vz2wav.c  --  Convert a VZ/VZ200/VZ300 tape image file to a WAV audio file.
 *
 * Reconstructed from the decompiled 16-bit DOS binary (vz2wav.exe), with the
 * actual waveform tables and file structure verified against a known-good WAV
 * output produced by the original DOS application.
 *
 * Usage:
 *   vz2wav [--compat] [--artifact] <input.vz> <output.wav>
 *
 *   --compat    Produce a "malformed" WAV file that matches the DOS original
 *               exactly: the RIFF and data chunk size fields are set to the
 *               raw DOS stack garbage values (0x00146e0c and 0x01140000) because
 *               the DOS program never rewound the file to fix them.
 *               Most WAV players handle this gracefully.  Omit --compat for a
 *               standards-compliant WAV file with correct size fields.
 *
 *   --artifact  Write the exact 80-byte uninitialized Borland C stack pattern
 *               into the padding gap between the filename and address block,
 *               instead of 80 x 0x7F.  This matches the padding bytes found
 *               in the known-good DOS binary output byte-for-byte.  The VZ
 *               hardware treats both identically (both are below the comparator
 *               threshold).  Can be combined with --compat.
 *
 * Build (Linux / GCC):
 *   gcc -Wall -o vz2wav vz2wav.c
 *
 * Build (Windows / MinGW):
 *   gcc -Wall -o vz2wav.exe vz2wav.c
 *
 * =========================================================================
 * VZ file format  (from vzdasm.c reference and binary analysis)
 * =========================================================================
 *  Offset  Size  Description
 *  ------  ----  -----------
 *   0       4    Magic: "VZF0" | "  \0\0" | "VZFO"
 *   4      16    Filename (null-padded)
 *  20       1    Reserved
 *  21       1    File type: 0xF0 = BASIC, 0xF1 = machine code
 *  22       2    Load address, little-endian (lo, hi)
 *  24       N    File data body
 *
 * =========================================================================
 * WAV tape encoding  (verified against DOS binary output)
 * =========================================================================
 *
 * Format:   22050 Hz, 8-bit unsigned PCM, mono
 * Silence:  0x7F (mid-scale, not 0x00)
 *
 * Stream layout:
 *  1.  22050 x 0x7F          pre-silence (1 second)
 *  2.  255 x 0x80            leader bytes, VZ-encoded
 *  3.  5 x 0xFE              sync preamble bytes, VZ-encoded
 *  4.  vz_header[21]         file-type byte, VZ-encoded
 *                            (the DOS code wrote an uninitialised stack byte
 *                             here which happened to equal the file-type byte)
 *  5.  filename bytes        null-terminated, up to 17 bytes, VZ-encoded
 *  6.  80 raw bytes          NOT VZ-encoded; written directly to the file.
 *                            DOS original wrote uninitialised Borland C stack
 *                            data (0x40..0x79 + zeros); we write 0x7F here.
 *  7.  addr_lo               load address low byte, VZ-encoded
 *  8.  addr_hi               load address high byte, VZ-encoded
 *  9.  end_addr_lo           (load_addr + body_len) low byte, VZ-encoded
 * 10.  end_addr_hi           (load_addr + body_len) high byte, VZ-encoded
 * 11.  N data bytes          body, VZ-encoded
 * 12.  checksum_lo           16-bit checksum low byte, VZ-encoded
 *                            checksum = addr_lo+addr_hi+end_lo+end_hi+sum(body)
 * 13.  checksum_hi           checksum high byte, VZ-encoded
 * 14.  22050 x 0x7F          post-silence (1 second)
 *
 * Each VZ byte is transmitted MSB-first as 8 bits; each bit = 38 PCM samples.
 *
 * =========================================================================
 * Bit waveforms  (extracted directly from the DOS binary output)
 * =========================================================================
 *
 * The waveforms are SINUSOIDAL, not square waves.  Square-wave reimplementations
 * fail to load reliably on real VZ hardware.  The tables below were recovered
 * by decoding a known-good WAV file produced by the original DOS program and
 * verifying that every single bit in the file uses exactly these patterns.
 *
 * Bit 1 (two sinusoidal half-cycles, ~2400 Hz):
 *   156,178,189,191,195,179,118, 88, 75, 70, 70, 67,110,163,180,190,190,195,
 *   168,108, 84, 73, 69, 69, 68,117,165,181,190,190,195,161,104, 83, 72, 69,
 *    67, 72
 *
 * Bit 0 (one sinusoidal full cycle, ~1200 Hz):
 *   165,181,190,190,195,161,104, 83, 72, 69, 67, 72,129,169,183,190,192,192,
 *   191,191,189,189,186,189,157, 98, 76, 65, 62, 61, 61, 62, 63, 64, 64, 67,
 *    65,106
 *
 * Peak ~ 195 (0xC3), trough ~ 61 (0x3D), midpoint ~ 128 (0x80).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>     /* uint8_t, uint16_t, uint32_t -- same width on all targets */
#include <inttypes.h>   /* PRIu32, PRIu8, PRIX16 etc -- matching printf format macros */

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
#define PADDING_RAW_BYTES   80          /* 0x50 raw bytes between filename and addr block */
#define VZ_HEADER_SIZE      24
#define FILENAME_FIELD_LEN  16          /* bytes [4..19] of VZ header */
#define SAMPLES_PER_BIT     38

/* -------------------------------------------------------------------------
 * Bit waveform tables  (exact values from the DOS binary)
 * ------------------------------------------------------------------------- */

static const unsigned char BIT1_WAVE[SAMPLES_PER_BIT] = {
    156, 178, 189, 191, 195, 179, 118,  88,  75,  70,
     70,  67, 110, 163, 180, 190, 190, 195, 168, 108,
     84,  73,  69,  69,  68, 117, 165, 181, 190, 190,
    195, 161, 104,  83,  72,  69,  67,  72
};

static const unsigned char BIT0_WAVE[SAMPLES_PER_BIT] = {
    165, 181, 190, 190, 195, 161, 104,  83,  72,  69,
     67,  72, 129, 169, 183, 190, 192, 192, 191, 191,
    189, 189, 186, 189, 157,  98,  76,  65,  62,  61,
     61,  62,  63,  64,  64,  67,  65, 106
};

/* -------------------------------------------------------------------------
 * Borland C artifact padding  (exact bytes from the DOS binary output)
 *
 * The original DOS program wrote 80 bytes of uninitialized Borland C stack
 * data into the gap between the filename NUL and the address block.  These
 * were recovered from the known-good hello.wav DOS output.
 *
 * Structure of the 80 bytes:
 *   Bytes  0..69  -- a ramp of values 0x40..0x79 (Borland RTL table data
 *                    left on the stack from earlier library calls)
 *   Bytes 70..79  -- 0x00 (a zero-initialized local variable in the frame)
 *
 * The VZ hardware comparator threshold is ~0x80; all of these values are
 * below threshold and are read as logic LOW, identical to 0x7F silence.
 * Use --artifact only if you need bit-exact reproduction of the DOS output.
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

/* Build a 44-byte PCM WAV header.  Pass data_bytes=0 for compat mode
 * (size fields set to raw DOS stack garbage values, matching the DOS original). */
static void build_wav_header(unsigned char hdr[44], uint32_t data_bytes, int compat_mode)
{
    memset(hdr, 0, 44);
    memcpy(hdr +  0, "RIFF", 4);
    if (compat_mode) {
        put32le(hdr + 4, 0x00146e0c);  /* RIFF size: raw DOS stack garbage */
    } else if (data_bytes) {
        put32le(hdr + 4, data_bytes + 36);
    }
    memcpy(hdr +  8, "WAVE", 4);
    memcpy(hdr + 12, "fmt ", 4);
    put32le(hdr + 16, 16);                      /* fmt sub-chunk size */
    put16le(hdr + 20, 1);                       /* PCM */
    put16le(hdr + 22, 1);                       /* mono */
    put32le(hdr + 24, (uint32_t)SAMPLE_RATE);
    put32le(hdr + 28, (uint32_t)SAMPLE_RATE);   /* byte rate = rate * 1 ch * 1 byte/sample */
    put16le(hdr + 32, 1);                       /* block align */
    put16le(hdr + 34, 8);                       /* bits per sample */
    memcpy(hdr + 36, "data", 4);
    if (compat_mode) {
        put32le(hdr + 40, 0x01140000);  /* data size: raw DOS stack garbage */
    } else if (data_bytes) {
        put32le(hdr + 40, data_bytes);
    }
}

/* -------------------------------------------------------------------------
 * Tape encoding primitives
 * ------------------------------------------------------------------------- */

static int write_bit(FILE *out, int bit)
{
    size_t n = fwrite(bit ? BIT1_WAVE : BIT0_WAVE, 1, SAMPLES_PER_BIT, out);
    return (n == (size_t)SAMPLES_PER_BIT) ? 0 : -1;
}

/* Write one byte MSB-first as 8 VZ tape bits. */
static int write_vz_byte(FILE *out, unsigned char val)
{
    int i;
    for (i = 7; i >= 0; i--)
        if (write_bit(out, (val >> i) & 1) < 0)
            return -1;
    return 0;
}

/* Write count raw bytes (not VZ-encoded). */
static int write_raw(FILE *out, unsigned char fill, uint32_t count)
{
    uint32_t i;
    for (i = 0; i < count; i++)
        if (fputc(fill, out) == EOF)
            return -1;
    return 0;
}

/* Write the exact Borland C artifact padding bytes (--artifact mode). */
static int write_artifact_padding(FILE *out)
{
    size_t n = fwrite(BORLAND_ARTIFACT_PADDING, 1, PADDING_RAW_BYTES, out);
    return (n == (size_t)PADDING_RAW_BYTES) ? 0 : -1;
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

    FILE          *fin  = NULL;
    FILE          *fout = NULL;
    unsigned char *body = NULL;
    int            ret  = 1;
    int            i;

    unsigned char  vz_hdr[VZ_HEADER_SIZE];
    unsigned char  filename[FILENAME_FIELD_LEN + 1];
    uint8_t        file_type, addr_lo, addr_hi;
    long           file_size;           /* ftell() returns long -- OS boundary */
    uint32_t       body_len;            /* VZ body <= 65535 bytes; fits in 32 bits */
    uint16_t       load_addr, end_addr; /* Z80 16-bit addresses */
    uint8_t        end_addr_lo, end_addr_hi;
    uint32_t       checksum;            /* worst case: 65535*255 + overhead < 2^32 */
    uint8_t        cksum_lo, cksum_hi;
    uint32_t       fn_write_len;
    uint32_t       total_samples;       /* worst case ~20M; fits in 32 bits */
    unsigned char  wav_hdr[44];

    /* -- Argument parsing ------------------------------------------------ */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--compat") == 0)
            compat_mode = 1;
        else if (strcmp(argv[i], "--artifact") == 0)
            artifact_mode = 1;
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
        fprintf(stderr,
            "vz2wav - Convert VZ-200/VZ-300 tape image to WAV audio\n"
            "Usage: vz2wav [--compat] [--artifact] <input.vz> <output.wav>\n"
            "\n"
            "  --compat     Write a malformed WAV matching the original DOS program\n"
            "               (RIFF and data size fields use raw DOS stack garbage).  Players open it\n"
            "               anyway.  Omit for a standards-compliant WAV file.\n"
            "  --artifact   Write the exact Borland C uninitialized stack bytes into\n"
            "               the 80-byte padding gap instead of 0x7F silence.  Both\n"
            "               are below the VZ comparator threshold and load the same.\n"
            "               Can be combined with --compat.\n");
        return 1;
    }

    /* -- Banner ---------------------------------------------------------- */
    printf("\nvz2wav - VZ tape image to WAV converter\n");
    printf("Mode: %s%s\n\n",
           compat_mode   ? "compat (malformed WAV header) " : "clean  (standards WAV header) ",
           artifact_mode ? "+ artifact padding"             : "");

    /* -- Read VZ file ---------------------------------------------------- */
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
        static const unsigned char M0[] = {0x56,0x5A,0x46,0x30}; /* VZF0 */
        static const unsigned char M1[] = {0x20,0x20,0x00,0x00}; /* "  \0\0" */
        static const unsigned char M2[] = {0x56,0x5A,0x46,0x4F}; /* VZFO */
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

    /* Read body */
    fseek(fin, 0, SEEK_END);
    file_size = ftell(fin);
    if (file_size < 0) {
        fprintf(stderr, "vz2wav: cannot seek '%s'\n", arg_input);
        goto done;
    }
    if (file_size <= (long)VZ_HEADER_SIZE) {
        fprintf(stderr, "vz2wav: '%s' has no body data\n", arg_input);
        goto done;
    }
    /* Range check: body must fit in uint32_t (and practically must be <= 65535) */
    if ((file_size - VZ_HEADER_SIZE) > (long)UINT32_MAX) {
        fprintf(stderr, "vz2wav: '%s' body is impossibly large\n", arg_input);
        goto done;
    }
    body_len = (uint32_t)(file_size - VZ_HEADER_SIZE);
    body = (unsigned char *)malloc((size_t)body_len);
    if (!body) { fprintf(stderr, "vz2wav: out of memory\n"); goto done; }
    fseek(fin, VZ_HEADER_SIZE, SEEK_SET);
    if (fread(body, 1, (size_t)body_len, fin) != (size_t)body_len) {
        fprintf(stderr, "vz2wav: read error on '%s'\n", arg_input);
        goto done;
    }
    fclose(fin); fin = NULL;

    /* -- Info ------------------------------------------------------------ */
    printf("Input : %s\n", arg_input);
    printf("  Filename  : %.16s\n", (char *)filename);
    printf("  File type : %s (0x%02" PRIX8 ")\n",
           file_type == 0xF0 ? "BASIC" :
           file_type == 0xF1 ? "Machine code" : "Unknown",
           file_type);
    printf("  Load addr : 0x%04" PRIX16 "\n", load_addr);
    printf("  Body size : %" PRIu32 " bytes\n", body_len);

    /* -- Compute tape header fields -------------------------------------- */

    /* End address = load address + body length */
    end_addr    = (uint16_t)(load_addr + body_len);
    end_addr_lo = (uint8_t)( end_addr       & 0xFF);
    end_addr_hi = (uint8_t)((end_addr >> 8) & 0xFF);

    /* 16-bit checksum of addr block + body */
    checksum = (uint32_t)addr_lo + addr_hi + end_addr_lo + end_addr_hi;
    for (i = 0; i < (int)body_len; i++)
        checksum += body[i];
    cksum_lo = (uint8_t)( checksum       & 0xFF);
    cksum_hi = (uint8_t)((checksum >> 8) & 0xFF);

    /* Filename write length: chars up to and including the first NUL */
    fn_write_len = 0;
    while (fn_write_len < (uint32_t)FILENAME_FIELD_LEN) {
        fn_write_len++;
        if (filename[fn_write_len - 1] == '\0') break;
    }
    /* filename[FILENAME_FIELD_LEN] is always '\0' (set above), so this is safe */
    if (fn_write_len == (uint32_t)FILENAME_FIELD_LEN)
        fn_write_len++; /* write the guaranteed NUL at index [16] */

    printf("  End addr  : 0x%04" PRIX16 "\n", end_addr);
    printf("  Checksum  : 0x%02" PRIX8 "%02" PRIX8 "\n\n", cksum_hi, cksum_lo);

    /* -- Total sample count (needed for clean WAV header) --------------- */
    total_samples =
        (uint32_t)SILENCE_SAMPLES
      + (uint32_t)LEADER_COUNT * 8 * SAMPLES_PER_BIT
      + (uint32_t)SYNC_COUNT   * 8 * SAMPLES_PER_BIT
      + (uint32_t)1            * 8 * SAMPLES_PER_BIT   /* file-type byte */
      + fn_write_len           * 8 * SAMPLES_PER_BIT   /* filename + NUL */
      + (uint32_t)PADDING_RAW_BYTES
      + (uint32_t)4            * 8 * SAMPLES_PER_BIT   /* addr + end_addr */
      + body_len               * 8 * SAMPLES_PER_BIT   /* body */
      + (uint32_t)2            * 8 * SAMPLES_PER_BIT   /* checksum */
      + (uint32_t)SILENCE_SAMPLES;

    /* -- Open output ----------------------------------------------------- */
    fout = fopen(arg_output, "wb");
    if (!fout) {
        fprintf(stderr, "vz2wav: cannot create '%s'\n", arg_output);
        goto done;
    }

    /* -- WAV header ------------------------------------------------------ */
    /* compat: size fields = 0 (like the DOS original, which never fixed them) */
    /* clean:  correct sizes computed above                                   */
    build_wav_header(wav_hdr, compat_mode ? (uint32_t)0 : total_samples, compat_mode);
    if (fwrite(wav_hdr, 1, 44, fout) != (size_t)44) goto write_err;

    /* -- Pre-silence ----------------------------------------------------- */
    if (write_raw(fout, SILENCE_BYTE, SILENCE_SAMPLES) < 0) goto write_err;

    /* -- Leader: 255 x 0x80 ---------------------------------------------- */
    for (i = 0; i < LEADER_COUNT; i++)
        if (write_vz_byte(fout, LEADER_BYTE) < 0) goto write_err;

    /* -- Sync preamble: 5 x 0xFE ----------------------------------------- */
    for (i = 0; i < SYNC_COUNT; i++)
        if (write_vz_byte(fout, SYNC_BYTE) < 0) goto write_err;

    /* -- File-type byte (acts as the sync byte in the VZ protocol) ------- */
    /* The DOS program wrote an uninitialised stack variable here (bLoc15).  */
    /* The Borland C stack frame happened to hold vz_hdr[21] at that offset. */
    if (write_vz_byte(fout, file_type) < 0) goto write_err;

    /* -- Filename (null-terminated, VZ-encoded) -------------------------- */
    {
        uint32_t j;
        for (j = 0; j < fn_write_len; j++)
            if (write_vz_byte(fout, filename[j]) < 0) goto write_err;
    }

    /* -- Raw padding: 80 bytes ------------------------------------------- */
    /* The DOS program wrote 80 bytes of uninitialized Borland C stack data  */
    /* (0x40..0x79 ramp + 10 x 0x00) into this gap.  In clean mode we write  */
    /* 0x7F silence instead -- both are below the VZ comparator threshold    */
    /* and the hardware treats them identically.  Use --artifact to write the */
    /* exact original bytes for bit-for-bit reproduction of the DOS output.  */
    if (artifact_mode) {
        if (write_artifact_padding(fout) < 0) goto write_err;
    } else {
        if (write_raw(fout, SILENCE_BYTE, PADDING_RAW_BYTES) < 0) goto write_err;
    }

    /* -- Address block: addr_lo, addr_hi, end_addr_lo, end_addr_hi -------- */
    if (write_vz_byte(fout, addr_lo)     < 0) goto write_err;
    if (write_vz_byte(fout, addr_hi)     < 0) goto write_err;
    if (write_vz_byte(fout, end_addr_lo) < 0) goto write_err;
    if (write_vz_byte(fout, end_addr_hi) < 0) goto write_err;

    /* -- Body data ------------------------------------------------------- */
    {
        uint32_t j;
        for (j = 0; j < body_len; j++)
            if (write_vz_byte(fout, body[j]) < 0) goto write_err;
    }

    /* -- Checksum: lo then hi -------------------------------------------- */
    if (write_vz_byte(fout, cksum_lo) < 0) goto write_err;
    if (write_vz_byte(fout, cksum_hi) < 0) goto write_err;

    /* -- Post-silence ---------------------------------------------------- */
    if (write_raw(fout, SILENCE_BYTE, SILENCE_SAMPLES) < 0) goto write_err;

    /* -- Summary --------------------------------------------------------- */
    printf("Output: %s\n", arg_output);
    printf("  Total audio : %" PRIu32 " samples (%.2f seconds)\n",
           total_samples, (double)total_samples / SAMPLE_RATE);
    printf("  WAV sizes   : %s\n",
           compat_mode ? "raw DOS garbage (compat mode -- matches DOS original)" : "correct");
    printf("  Padding     : %s\n",
           artifact_mode ? "Borland artifact bytes (--artifact)" : "0x7F silence (clean)");
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
