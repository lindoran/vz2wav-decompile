/*
 * wav2vz.c  --  WAV to .VZ file converter
 *
 * Reconstructed from WAV2VZ_2.EXE (Borland C++ / DOS, ~1991).
 * Reads a raw PCM WAV (22050 Hz, 8-bit, mono) containing an FSK-encoded
 * .VZ tape image (Sharp MZ-series / VZ-200/300) and decodes it to a
 * binary .VZ file.
 *
 * Build:
 *   Linux/macOS : gcc  -std=c99 -O2 -Wall -Wextra -o wav2vz wav2vz.c
 *   MinGW Win32 : gcc  -std=c99 -O2 -Wall -Wextra -o wav2vz.exe wav2vz.c
 *   MinGW Win64 : same -- Win32 ABI honoured via LLP64; 'long' not used
 *
 * Usage:  wav2vz  input.wav  output.vz
 *
 * -------------------------------------------------------------------------
 * Portability audit (GCC/Linux vs MinGW/Win32 and Win64):
 *
 *  sizeof(int)  = 4 on LP64 Linux AND LLP64 Win32/Win64.  Safe.
 *  sizeof(long) = 8 on LP64 Linux, 4 on LLP64 Win32/Win64.
 *                 'long' is deliberately ABSENT from this file.
 *
 *  stdint.h (uint8_t, uint16_t, uint32_t):
 *    Present in MinGW >= GCC 4.x.  No conditional inclusion needed.
 *
 *  printf format specifiers:
 *    All unsigned integer arguments are cast to (unsigned) before passing
 *    to printf.  uint8_t/uint16_t vararg-promote to int; the (unsigned)
 *    cast eliminates any signed-extension ambiguity and silences
 *    -Wformat warnings on strict MinGW builds.
 *    %u  used for unsigned values.
 *    %02X / %04X used for hex; paired with (unsigned) cast.
 *    'long' format modifiers (%lu, %ld, %lX) are NOT used.
 *
 *  fgetc() returns int in [-1..255].  EOF == -1.
 *    We check (c == EOF) as an int comparison BEFORE any cast to uint8_t
 *    or unsigned.  Casting EOF to uint8_t yields 0xFF, which is >=
 *    TAPE_LEADER_THRESH (0x90) -- a silent false positive on truncated
 *    files.  The explicit EOF guard prevents this.
 *
 *  fread() / fwrite() return size_t.
 *    Compared only to the literal 1 in a size_t context; no narrowing.
 *
 *  ungetc():
 *    Called only on files opened in binary mode ("rb").  The Windows CRT
 *    restriction on ungetc() with bytes > 127 applies only to text mode.
 *
 *  #pragma pack:
 *    Supported identically by GCC and MinGW (MSVC syntax, widely adopted).
 *
 *  _Static_assert / STATIC_ASSERT:
 *    C11 _Static_assert preferred; C99 array-size fallback provided.
 * -------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* -----------------------------------------------------------------------
 * Compile-time assertion.
 * C11 _Static_assert is available in GCC >= 4.6 and MinGW >= 4.6.
 * The C99 fallback uses a zero/negative-size array trick; the __LINE__
 * concatenation gives each instantiation a unique symbol name.
 * ----------------------------------------------------------------------- */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
   /* C11: msg must be a string literal -- we stringify the token */
#  define STATIC_ASSERT(cond, msg)  _Static_assert((cond), #msg)
#else
   /* C99 fallback: negative-size array triggers a compile error */
#  define SA_CAT2(a, b)  a##b
#  define SA_CAT(a, b)   SA_CAT2(a, b)
#  define STATIC_ASSERT(cond, msg) \
     typedef char SA_CAT(static_assert_, __LINE__)[(cond) ? 1 : -1]
#endif

/* -----------------------------------------------------------------------
 * FSK / tape decoding constants.
 *
 * The WAV stream encodes each bit as a series of audio half-cycles
 * (alternating high and low amplitude runs) sampled at 22050 Hz, 8-bit.
 * FindCycle() counts the total samples in one high+low pair and maps the
 * count to one of three states:
 *
 *   CYCLE_SHORT : total in [9..14]   -- encodes bit 0
 *   CYCLE_LONG  : total in [17..29]  -- encodes bit 1 / leader
 *   CYCLE_ERROR : anything else      -- indeterminate, skip
 *
 * Boundary values are lifted verbatim from the original ASM:
 *   cmp si, 08h  / jle -> lower exclusive: short total >= 9
 *   cmp si, 0Fh  / jge -> upper exclusive: short total <= 14
 *   cmp si, 10h  / jle -> lower exclusive: long  total >= 17
 *   cmp si, 1Eh  / jge -> upper exclusive: long  total <= 29
 * ----------------------------------------------------------------------- */
#define CYCLE_SHORT_LO   9
#define CYCLE_SHORT_HI  14
#define CYCLE_LONG_LO   17
#define CYCLE_LONG_HI   29

/* Stored and compared as uint8_t throughout -- no implicit sign issues. */
#define CYCLE_SHORT  ((uint8_t)0x00u)
#define CYCLE_LONG   ((uint8_t)0x01u)
#define CYCLE_ERROR  ((uint8_t)0xFFu)

/* Raw WAV / tape sentinels */
#define TAPE_LEADER_THRESH  0x90u   /* raw WAV sample; leader found >= this */
#define TAPE_START_BYTE     0x80u   /* end of leader, start of encoded data */
#define TAPE_PREAMBLE_BYTE  0xFEu   /* preamble sync byte (appears 5 times) */

/* -----------------------------------------------------------------------
 * WAV RIFF header -- packed struct to match the on-disk RIFF layout.
 *
 * Field offsets confirmed against the original ASM buffer checks.
 * The original fread'd 0x48 (72) bytes from offset 0 of the WAV file
 * into a local stack buffer and compared four fields:
 *
 *   [bp-0x44] -> buffer+0x16 -> struct offset 22 : num_channels    (== 1)
 *   [bp-0x42] -> buffer+0x18 -> struct offset 24 : sample_rate_lo  (== 22050)
 *   [bp-0x40] -> buffer+0x1A -> struct offset 26 : sample_rate_hi  (== 0)
 *   [bp-0x38] -> buffer+0x22 -> struct offset 34 : bits_per_sample (== 8)
 * ----------------------------------------------------------------------- */
#pragma pack(push, 1)
typedef struct {
    uint8_t  riff_id[4];        /* "RIFF"                     offset  0 */
    uint32_t riff_size;         /* file size - 8               offset  4 */
    uint8_t  wave_id[4];        /* "WAVE"                     offset  8 */
    uint8_t  fmt_id[4];         /* "fmt "                     offset 12 */
    uint32_t fmt_size;          /* 16 for PCM                  offset 16 */
    uint16_t audio_format;      /* 1 = PCM                     offset 20 */
    uint16_t num_channels;      /* must be 1                   offset 22 */
    uint16_t sample_rate_lo;    /* sample rate low word        offset 24 */
    uint16_t sample_rate_hi;    /* sample rate high word (= 0) offset 26 */
    uint32_t byte_rate;         /* bytes per second            offset 28 */
    uint16_t block_align;       /* bytes per sample frame      offset 32 */
    uint16_t bits_per_sample;   /* must be 8                   offset 34 */
    uint8_t  data_id[4];        /* "data"                     offset 36 */
    uint32_t data_size;         /* audio data byte count       offset 40 */
    uint8_t  pad[0x48u - 44u];  /* padding to 72 bytes         offset 44 */
} WavHeader;
#pragma pack(pop)

STATIC_ASSERT(sizeof(WavHeader) == 0x48u, WavHeader_must_be_72_bytes);


/* -----------------------------------------------------------------------
 * VZ file header -- 24 bytes on disk, confirmed by vzdasm.c.
 *
 * Verified from the VZ disassembler source (vzdasm.c):
 *   header_vzfile1 = { 0x56, 0x5A, 0x46, 0x30 } = "VZF0"  (BASIC)
 *   header_vzfile3 = { 0x56, 0x5A, 0x46, 0x4F } = "VZFO"  (machine code)
 *   data[21] = file_type  (0xF0 = BASIC, 0xF1 = machine code)
 *   data[22] = start_addr lo,  data[23] = start_addr hi
 *   data[24..] = payload
 *
 * On-disk layout:
 *   Bytes  0- 3 : magic "VZF0" (BASIC) or "VZFO" (machine code)
 *   Bytes  4-20 : filename, NUL-terminated, 17 bytes
 *   Byte  21    : file_type (0xF0 or 0xF1)
 *   Bytes 22-23 : start address, little-endian uint16
 *
 * NOTE: end_addr is NOT stored in the file. It is transmitted on tape
 * for the decoder to know the payload size, but the file size itself
 * encodes the length (payload = file_size - 24).
 * ----------------------------------------------------------------------- */
#pragma pack(push, 1)
typedef struct {
    uint8_t  magic[4];          /* "VZF0" (BASIC) or "VZFO" (machine code) */
    char     filename[17];      /* NUL-terminated, up to 16 usable chars   */
    uint8_t  file_type;         /* 0xF0 = BASIC, 0xF1 = machine code       */
    uint16_t start_addr;        /* little-endian load/exec start address   */
} VzHeader;
#pragma pack(pop)

STATIC_ASSERT(sizeof(VzHeader) == 24u, VzHeader_must_be_24_bytes);

/* -----------------------------------------------------------------------
 * Global file handles.
 * Kept global so fatal() can close them from any call depth.
 * ----------------------------------------------------------------------- */
static FILE *g_wav = NULL;
static FILE *g_vz  = NULL;

/* -----------------------------------------------------------------------
 * fatal() -- print error, close files, exit.
 * ----------------------------------------------------------------------- */
static void fatal(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
    if (g_wav) { fclose(g_wav); g_wav = NULL; }
    if (g_vz)  { fclose(g_vz);  g_vz  = NULL; }
    exit(1);
}

/* -----------------------------------------------------------------------
 * safe_fgetc() -- fgetc() with EOF -> fatal().
 *
 * Returns int in [0..255].  Return type is int (not uint8_t) so the
 * caller can pass the value directly to ungetc() without a cast.
 * ungetc() is specified to take an int argument; passing a uint8_t
 * value that has been sign-extended through a char would be wrong on
 * platforms where char is signed.
 * ----------------------------------------------------------------------- */
static int safe_fgetc(void)
{
    int c = fgetc(g_wav);
    if (c == EOF)
        fatal("\nFATAL ERROR - unexpected end of WAV file");
    return c;   /* guaranteed [0..255] */
}

/* -----------------------------------------------------------------------
 * FindCycle() -- measure and classify one FSK half-cycle.
 *
 * Faithful to the original two-counter loop structure in _FindCycle
 * (0AE3:000F).  register di = hi_count, [bp-2] = lo_count in the asm.
 *
 * All comparisons use (unsigned)c to avoid -Wsign-compare; fgetc()
 * returns int but we have already excluded EOF above so the value is
 * always in [0..255], making the (unsigned) cast safe and portable.
 * ----------------------------------------------------------------------- */
static uint8_t FindCycle(void)
{
    int c;
    int hi_count;
    int lo_count;
    int total;

    /* Advance to the next high sample (> 0x7F) */
    do {
        c = safe_fgetc();
    } while ((unsigned)c <= 0x7Fu);

    /* Count consecutive high samples; c holds the first one already */
    hi_count = 1;
    for (;;) {
        c = safe_fgetc();
        if ((unsigned)c <= 0x7Fu) break;
        hi_count++;
    }

    /* Count consecutive low samples; c holds the first one already */
    lo_count = 1;
    for (;;) {
        c = safe_fgetc();
        if ((unsigned)c > 0x7Fu) break;
        lo_count++;
    }

    /* Push back the first high sample of the next cycle */
    ungetc(c, g_wav);

    /* Classify */
    total = hi_count + lo_count;

    if (total > CYCLE_SHORT_LO && total <= CYCLE_SHORT_HI) return CYCLE_SHORT;
    if (total > CYCLE_LONG_LO  && total <= CYCLE_LONG_HI)  return CYCLE_LONG;
    return CYCLE_ERROR;
}

/* -----------------------------------------------------------------------
 * ReadVZBit() -- decode one FSK bit (_ReadVZBit, 0AE3:0112).
 *
 *   FindCycle() -> SHORT + LONG          => bit 0
 *   FindCycle() -> SHORT + SHORT + SHORT => bit 1
 *   anything else                        => CYCLE_ERROR
 * ----------------------------------------------------------------------- */
static uint8_t ReadVZBit(void)
{
    uint8_t c1, c2, c3;

    c1 = FindCycle();
    if (c1 != CYCLE_SHORT) return CYCLE_ERROR;

    c2 = FindCycle();
    if (c2 == CYCLE_LONG)  return 0u;   /* bit 0 */
    if (c2 != CYCLE_SHORT) return CYCLE_ERROR;

    c3 = FindCycle();
    if (c3 == CYCLE_SHORT) return 1u;   /* bit 1 */

    return CYCLE_ERROR;
}

/* -----------------------------------------------------------------------
 * ReadVZbyte() -- assemble 8 bit-slots MSB-first (_ReadVZbyte, 0AE3:0174).
 *
 * CYCLE_ERROR slots are skipped without shifting the accumulator,
 * matching the original "cmp al, 0FFh / jz skip_shift" logic.
 *
 * Shifts and ORs are done in (unsigned) width to avoid -Wconversion
 * on the narrowing truncation back to uint8_t.
 * ----------------------------------------------------------------------- */
static uint8_t ReadVZbyte(void)
{
    int     i;
    uint8_t result = 0u;
    uint8_t bit;

    for (i = 0; i < 8; i++) {
        bit = ReadVZBit();
        if (bit != CYCLE_ERROR)
            result = (uint8_t)(((unsigned)result << 1u) | (unsigned)bit);
    }
    return result;
}

/* -----------------------------------------------------------------------
 * read_u16_le() -- two tape bytes -> little-endian uint16_t.
 *
 * Arithmetic is done in 'unsigned' (>= 32-bit on every supported
 * platform) before truncation to uint16_t, avoiding implementation-
 * defined behaviour on narrow shift operations.
 * ----------------------------------------------------------------------- */
static uint16_t read_u16_le(void)
{
    unsigned lo = (unsigned)ReadVZbyte();
    unsigned hi = (unsigned)ReadVZbyte();
    return (uint16_t)(lo | (hi << 8u));
}

/* -----------------------------------------------------------------------
 * main()
 * ----------------------------------------------------------------------- */
int main(int argc, char *argv[])
{
    WavHeader  wav_hdr;
    VzHeader   vz_hdr;
    uint8_t    b;               /* decoded tape byte                        */
    int        c;               /* raw fgetc result -- MUST stay int        */
    int        i;
    uint8_t    file_type;
    uint8_t    filename_buf[17];
    uint16_t   start_addr, end_addr, data_size;
    uint16_t   checksum_calc, checksum_tape;

    /* ------------------------------------------------------------------ */
    /* Banner (strings from 0B42 data segment of the original EXE)         */
    /* ------------------------------------------------------------------ */
    printf("\t\tWAV2VZ - ");
    printf("WAV file to .VZ converter\n\n");
    printf("Usage: WAV2VZ wavfile.wav vzfile.vz\n\n");

    /* ------------------------------------------------------------------ */
    /* Argument check                                                       */
    /* ------------------------------------------------------------------ */
    if (argc != 3) {
        printf("error -- must specify input & output file\n");
        exit(1);
    }

    /* ------------------------------------------------------------------ */
    /* Open WAV input                                                       */
    /* ------------------------------------------------------------------ */
    printf("Opening WAV file......");
    fflush(stdout);

    g_wav = fopen(argv[1], "rb");
    if (!g_wav) {
        printf("error -- file doesn't exist\n");
        exit(1);
    }
    printf("OK!\n");

    /* ------------------------------------------------------------------ */
    /* Validate WAV header                                                  */
    /* ------------------------------------------------------------------ */
    if (fread(&wav_hdr, sizeof(WavHeader), 1u, g_wav) != 1u) {
        printf("error - could not read WAV header\n");
        fclose(g_wav);
        exit(1);
    }

    if (wav_hdr.sample_rate_hi  != 0u    ||
        wav_hdr.sample_rate_lo  != 22050u ||
        wav_hdr.bits_per_sample != 8u    ||
        wav_hdr.num_channels    != 1u)
    {
        printf("error - WAV file must be 22050hz 8 bit mono\n");
        fclose(g_wav);
        exit(1);
    }

    /* ------------------------------------------------------------------ */
    /* Open VZ output                                                       */
    /* ------------------------------------------------------------------ */
    g_vz = fopen(argv[2], "wb");
    if (!g_vz) {
        printf("error -- couldn't create output file\n");
        fclose(g_wav);
        exit(1);
    }

    /* ------------------------------------------------------------------ */
    /* Search for leader tone in raw WAV bytes                              */
    /*                                                                      */
    /* c is kept as int and EOF is checked before the (unsigned) cast.     */
    /* Casting EOF(-1) to unsigned gives 0xFFFFFFFF >= 0x90, which would   */
    /* silently exit the loop on a truncated file -- the int check stops   */
    /* this.                                                                */
    /* ------------------------------------------------------------------ */
    printf("Searching for signal..");
    fflush(stdout);

    do {
        c = fgetc(g_wav);
        if (c == EOF)
            fatal("error -- unexpected end of file while searching for signal");
    } while ((unsigned)c < TAPE_LEADER_THRESH);

    printf("OK!\n");

    /* ------------------------------------------------------------------ */
    /* Sync to leader: decode bytes until TAPE_START_BYTE (0x80)           */
    /* ------------------------------------------------------------------ */
    printf("Synching to leader.....");
    fflush(stdout);

    do { b = ReadVZbyte(); } while (b != (uint8_t)TAPE_START_BYTE);

    printf("OK!\n");

    /* ------------------------------------------------------------------ */
    /* Locate preamble: skip 0x80 bytes, then verify 0xFE * 5              */
    /* ------------------------------------------------------------------ */
    printf("Finding preamble......");
    fflush(stdout);

    do { b = ReadVZbyte(); } while (b == (uint8_t)TAPE_START_BYTE);

    if (b != (uint8_t)TAPE_PREAMBLE_BYTE) {
        printf("error finding preamble\n");
        fclose(g_wav); fclose(g_vz);
        exit(1);
    }

    for (i = 0; i < 4; i++) {
        if (ReadVZbyte() != (uint8_t)TAPE_PREAMBLE_BYTE) {
            printf("error reading preamble\n");
            fclose(g_wav); fclose(g_vz);
            exit(1);
        }
    }

    printf("OK!\n");
    printf("Reading tape header....\n\n");

    /* ------------------------------------------------------------------ */
    /* Read tape header fields                                              */
    /* ------------------------------------------------------------------ */

    file_type = ReadVZbyte();
    /* (unsigned) cast: uint8_t promotes through varargs to int; making it
     * explicitly unsigned ensures %02X prints the correct 2-digit hex on
     * all platforms including MinGW where MSVC-style printf is used.      */
    printf("File type         : %02X\n", (unsigned)file_type);

    memset(filename_buf, 0, sizeof(filename_buf));
    for (i = 0; i < 17; i++) {
        filename_buf[i] = ReadVZbyte();
        if (filename_buf[i] == 0u) break;
    }
    filename_buf[16] = 0u;                          /* unconditional NUL   */
    printf("Filename          : %s\n", (const char *)filename_buf);

    start_addr = read_u16_le();
    printf("Start Address     : %04X\n", (unsigned)start_addr);

    end_addr   = read_u16_le();
    printf("End Address       : %04X\n", (unsigned)end_addr);

    /* uint16_t subtraction wraps mod 65536, matching original 16-bit DOS */
    data_size  = (uint16_t)(end_addr - start_addr);
    printf("Size in bytes     : %u\n\n", (unsigned)data_size);

    /* ------------------------------------------------------------------ */
    /* Build and write VZ file header (24 bytes)                           */
    /*                                                                      */
    /* Magic (vzdasm.c): "VZF0" for BASIC (0xF0), "VZFO" for m/c (0xF1)  */
    /* Field order: magic[4] + filename[17] + file_type + start_addr[2]   */
    /* end_addr is NOT stored in the file -- size is implicit in file len. */
    /* ------------------------------------------------------------------ */
    printf("Creating VZ file......");
    fflush(stdout);

    memset(&vz_hdr, 0, sizeof(vz_hdr));
    vz_hdr.magic[0] = (uint8_t)'V';
    vz_hdr.magic[1] = (uint8_t)'Z';
    vz_hdr.magic[2] = (uint8_t)'F';
    vz_hdr.magic[3] = (file_type == 0xF0u) ? (uint8_t)'0' : (uint8_t)'O';
    memcpy(vz_hdr.filename, filename_buf, sizeof(filename_buf));
    vz_hdr.file_type  = file_type;
    vz_hdr.start_addr = start_addr;

    if (fwrite(&vz_hdr, sizeof(VzHeader), 1u, g_vz) != 1u)
        fatal("error writing VZ header");

    /* ------------------------------------------------------------------ */
    /* Decode and write data payload; accumulate checksum                  */
    /*                                                                      */
    /* Loop index i (int) holds values up to data_size (uint16_t <= 65535):*/
    /* no overflow.  checksum_calc wraps at 65536 (uint16_t arithmetic)   */
    /* matching the original 16-bit DOS accumulator.                       */
    /* ------------------------------------------------------------------ */
    checksum_calc = 0u;
    for (i = 0; i < (int)data_size; i++) {
        b = ReadVZbyte();
        checksum_calc = (uint16_t)(checksum_calc + (uint16_t)b);
        if (fwrite(&b, 1u, 1u, g_vz) != 1u)
            fatal("error writing data byte");
    }

    printf("OK!\n");
    printf("Comparing checksum....");
    fflush(stdout);

    /* ------------------------------------------------------------------ */
    /* Read and verify tape checksum (little-endian uint16)                */
    /* ------------------------------------------------------------------ */
    checksum_tape = read_u16_le();

    if (checksum_calc != checksum_tape)
        printf("error --  try resampling tape\n");
    else
        printf("OK!\n");

    printf("\n*** Operation completed ***\n");

    fclose(g_wav);
    fclose(g_vz);
    return 0;
}
