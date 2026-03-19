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
    const unsigned char *src = bit ? BIT1_WAVE : BIT0_WAVE;
    int gain_num = 100 + g_output_gain_percent;

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

static int write_vz_byte(FILE *out, unsigned char val)
{
    int i;
    for (i = 7; i >= 0; i--)
        if (write_bit(out, (val >> i) & 1) < 0)
            return -1;
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
        "Usage: vz2wav [--compat|-c] [--artifact|-a] [--robust|-r] [--gain|-g <percent>] <input.vz> <output.wav>\n"
        "\n"
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

    printf("\nvz2wav - VZ tape image to WAV converter\n");
    printf("Mode: %s%s%s\n\n",
           compat_mode   ? "compat (malformed WAV header) " : "clean  (standards WAV header) ",
           artifact_mode ? "+ artifact padding "            : "",
           robust_mode   ? "+ robust timing"                : "");

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
    printf("  Checksum  : 0x%02" PRIX8 "%02" PRIX8 "\n\n", cksum_hi, cksum_lo);

    /* Total samples */
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
    /*
     * Guard byte after checksum: leaves a clean high/low transition after
     * the final checksum bit so decoders that classify cycles using a
     * look-ahead edge can still recover the checksum reliably.
     */
    if (!compat_mode && write_vz_byte(fout, POST_CKSUM_GUARD) < 0) goto write_err;

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
