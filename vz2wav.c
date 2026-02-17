/*
 * VZ to WAV Converter - Original Hardware Compatible
 *
 * Generates WAV files compatible with original VZ200 hardware format
 * Based on VZFileSampler.java encoding scheme adapted to 22050 Hz
 *
 * Audio format: 22050 Hz, 8-bit unsigned PCM, mono
 * Bit encoding: 6 half-cycles per bit (~1.72ms per bit, ~38 samples per bit)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

// VZ file format constants
#define VZ_MAGIC_SIZE 4
#define VZ_FILENAME_SIZE 17
#define VZ_TYPE_OFFSET 21
#define VZ_START_ADDR_OFFSET 22
#define VZ_HEADER_SIZE 24

// WAV format constants
#define SAMPLE_RATE 22050
#define BITS_PER_SAMPLE 8
#define BYTES_PER_SAMPLE 1
#define CHANNELS 1

// Timing constants (matching Java emulator)
#define HALF_SHORT_CYCLE_NS 287103L
#define SAMPLES_PER_HALF_CYCLE ((SAMPLE_RATE * HALF_SHORT_CYCLE_NS) / 1000000000L)
#define SAMPLES_PER_BIT (6 * SAMPLES_PER_HALF_CYCLE)
#define SAMPLES_PER_BYTE (8 * SAMPLES_PER_BIT)
#define GAP_TIME_NS 3065000L
#define GAP_SAMPLES ((SAMPLE_RATE * GAP_TIME_NS) / 1000000000L)
#define NULL_GAP_SAMPLES 10   // wierd null gap from dos app.
#define GAP_SILENCE (GAP_SAMPLES-NULL_GAP_SAMPLES)

// Tape format constants
#define LEAD_IN_0x80_COUNT 255
#define LEAD_IN_0xFE_COUNT 5
#define LEAD_OUT_0x00_COUNT 20

// Initial silence before leader (seconds)
#define INITIAL_SILENCE_SEC 1
#define TAIL_SILENCE_SEC 1
#define INITIAL_SILENCE_SAMPLES (SAMPLE_RATE * INITIAL_SILENCE_SEC)
#define TAIL_SILENCE_SAMPLES (SAMPLE_RATE * TAIL_SILENCE_SEC)


// Audio level constants (8-bit unsigned)
#define VALUE_HI 195
#define VALUE_LO 61
#define VALUE_SILENCE 127  // Java inporter is using LOW -- but it passes on conversion, computer actually cares...

// WAV header structure
typedef struct {
    char riff[4];           // "RIFF"
    uint32_t file_size;     // File size - 8
    char wave[4];           // "WAVE"
    char fmt[4];            // "fmt "
    uint32_t fmt_size;      // 16 for PCM
    uint16_t audio_format;  // 1 for PCM
    uint16_t num_channels;  // 1 for mono
    uint32_t sample_rate;   // 44100 Hz
    uint32_t byte_rate;     // sample_rate * num_channels * bytes_per_sample
    uint16_t block_align;   // num_channels * bytes_per_sample
    uint16_t bits_per_sample; // 16 bits
    char data[4];           // "data"
    uint32_t data_size;     // Size of audio data
} __attribute__((packed)) WavHeader;

/*
 * Get the audio value for a specific bit at a specific time within the bit period.
 * This matches the Java emulator's getValueOfBit() method exactly.
 *
 * Bit encoding (6 half-cycles):
 *   Time 0-1: HIGH
 *   Time 1-2: LOW
 *   Time 2-3: HIGH
 *   Time 3-4: LOW if bit=1, HIGH if bit=0
 *   Time 4-5: HIGH if bit=1, LOW if bit=0
 *   Time 5-6: LOW
 */
static uint8_t get_bit_value(int bit, int sample_in_bit)
{
    int half_cycle = sample_in_bit / SAMPLES_PER_HALF_CYCLE;

    switch (half_cycle) {
        case 0: return VALUE_HI;  // Time 0-1
        case 1: return VALUE_LO;  // Time 1-2
        case 2: return VALUE_HI;  // Time 2-3
        case 3: return (bit == 1) ? VALUE_LO : VALUE_HI;  // Time 3-4
        case 4: return (bit == 1) ? VALUE_HI : VALUE_LO;  // Time 4-5
        default: return VALUE_LO; // Time 5-6
    }
}

/*
 * Write a single byte to the WAV file as audio samples.
 * Each byte = 8 bits, each bit = 6 half-cycles.
 * Bits are transmitted MSB first.
 */
static int write_byte_as_audio(FILE *out, uint8_t byte_val)
{
    for (int bit_index = 0; bit_index < 8; bit_index++) {
        // Extract bit (MSB first: bit 7, 6, 5, 4, 3, 2, 1, 0)
        int bit = (byte_val >> (7 - bit_index)) & 1;

        // Write samples for this bit
        for (int sample = 0; sample < SAMPLES_PER_BIT; sample++) {
            uint8_t value = get_bit_value(bit, sample);

            // Write as 8-bit unsigned
            if (fwrite(&value, 1, 1, out) != 1) {
                return -1;
            }
        }
    }
    return 0;
}

/*
 * Write silence (VALUE_SILENCE) for a specified number of samples
 */
static int write_silence(FILE *out, size_t num_samples)
{
    uint8_t value = VALUE_SILENCE;

    for (size_t i = 0; i < num_samples; i++) {
        if (fwrite(&value, 1, 1, out) != 1) {
            return -1;
        }
    }
    return 0;
}

/*
 * Write NULL (NULL 0x00) for a specified number of samples
 */
static int write_null(FILE *out, size_t num_samples)
{
    uint8_t value = 0;

    for (size_t i = 0; i < num_samples; i++) {
        if (fwrite(&value, 1, 1, out) != 1) {
            return -1;
        }
    }
    return 0;
}

/*
 * Calculate total number of samples needed
 */
static size_t calculate_total_samples(size_t data_length, size_t filename_length)
{
    size_t total = 0;

    // initial Silence 1 sec. default from dos utility.
    total += INITIAL_SILENCE_SAMPLES;

    // Leader: 255 × 0x80
    total += LEAD_IN_0x80_COUNT * SAMPLES_PER_BYTE;

    // Preamble: 5 × 0xFE
    total += LEAD_IN_0xFE_COUNT * SAMPLES_PER_BYTE;

    // File type: 1 byte
    total += SAMPLES_PER_BYTE;

    // Filename: variable length + null terminator
    total += (filename_length + 1) * SAMPLES_PER_BYTE;

    // Gap: silence
    total += GAP_SAMPLES;

    // Addresses: 4 bytes (start_lo, start_hi, end_lo, end_hi)
    total += 4 * SAMPLES_PER_BYTE;

    // Data: variable length
    total += data_length * SAMPLES_PER_BYTE;

    // Checksum: 2 bytes
    total += 2 * SAMPLES_PER_BYTE;

    // Lead-out: 20 × 0x00
    total += LEAD_OUT_0x00_COUNT * SAMPLES_PER_BYTE;

    // tail_silence 1 sec. default from dos utility.
    total += TAIL_SILENCE_SAMPLES;

    return total;
}

/*
 * Write WAV header
 */
static int write_wav_header(FILE *out, size_t num_samples)
{
    WavHeader header;
    size_t data_size = num_samples * BYTES_PER_SAMPLE;

    memcpy(header.riff, "RIFF", 4);
    header.file_size = data_size + 36; // data_size + header_size - 8
    memcpy(header.wave, "WAVE", 4);
    memcpy(header.fmt, "fmt ", 4);
    header.fmt_size = 16;
    header.audio_format = 1; // PCM
    header.num_channels = CHANNELS;
    header.sample_rate = SAMPLE_RATE;
    header.byte_rate = SAMPLE_RATE * CHANNELS * BYTES_PER_SAMPLE;
    header.block_align = CHANNELS * BYTES_PER_SAMPLE;
    header.bits_per_sample = BITS_PER_SAMPLE;
    memcpy(header.data, "data", 4);
    header.data_size = data_size;

    return fwrite(&header, 1, sizeof(header), out) == sizeof(header) ? 0 : -1;
}

/*
 * Main conversion function
 */
int convert_vz_to_wav(const char *input_file, const char *output_file)
{
    FILE *in = NULL;
    FILE *out = NULL;
    uint8_t *vz_data = NULL;
    int result = -1;

    // Open input file
    in = fopen(input_file, "rb");
    if (!in) {
        fprintf(stderr, "Error: Cannot open input file '%s'\n", input_file);
        goto cleanup;
    }

    // Read VZ header
    uint8_t vz_header[VZ_HEADER_SIZE];
    if (fread(vz_header, 1, VZ_HEADER_SIZE, in) != VZ_HEADER_SIZE) {
        fprintf(stderr, "Error: Cannot read VZ header\n");
        goto cleanup;
    }

    // Parse VZ header
    uint8_t vz_type = vz_header[VZ_TYPE_OFFSET];
    uint16_t vz_start_addr = vz_header[VZ_START_ADDR_OFFSET] |
                             (vz_header[VZ_START_ADDR_OFFSET + 1] << 8);

    // Extract filename (bytes 4-20)
    char vz_filename[VZ_FILENAME_SIZE + 1];
    memcpy(vz_filename, &vz_header[VZ_MAGIC_SIZE], VZ_FILENAME_SIZE);
    vz_filename[VZ_FILENAME_SIZE] = '\0';
    size_t filename_length = strlen(vz_filename);

    printf("VZ File Information:\n");
    printf("  Filename: %s\n", vz_filename);
    printf("  Type: 0x%02X\n", vz_type);
    printf("  Start Address: 0x%04X\n", vz_start_addr);

    // Read data
    fseek(in, 0, SEEK_END);
    long file_size = ftell(in);
    size_t data_length = file_size - VZ_HEADER_SIZE;
    fseek(in, VZ_HEADER_SIZE, SEEK_SET);

    vz_data = malloc(data_length);
    if (!vz_data) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        goto cleanup;
    }

    if (fread(vz_data, 1, data_length, in) != data_length) {
        fprintf(stderr, "Error: Cannot read VZ data\n");
        goto cleanup;
    }

    printf("  Data Length: %zu bytes\n", data_length);

    uint16_t vz_end_addr = vz_start_addr + data_length;
    printf("  End Address: 0x%04X\n\n", vz_end_addr);

    // Calculate checksum (addresses + data)
    uint16_t checksum = 0;
    checksum += vz_start_addr & 0xFF;
    checksum += (vz_start_addr >> 8) & 0xFF;
    checksum += vz_end_addr & 0xFF;
    checksum += (vz_end_addr >> 8) & 0xFF;
    for (size_t i = 0; i < data_length; i++) {
        checksum += vz_data[i];
    }
    checksum &= 0xFFFF;
    printf("  Checksum: 0x%04X\n\n", checksum);

    // Open output file
    out = fopen(output_file, "wb");
    if (!out) {
        fprintf(stderr, "Error: Cannot create output file '%s'\n", output_file);
        goto cleanup;
    }

    // Calculate total samples
    size_t total_samples = calculate_total_samples(data_length, filename_length);
    printf("Generating WAV file:\n");
    printf("  Sample Rate: %d Hz\n", SAMPLE_RATE);
    printf("  Bit Depth: %d-bit signed\n", BITS_PER_SAMPLE);
    printf("  Total Samples: %zu\n", total_samples);
    printf("  Duration: %.2f seconds\n\n", (double)total_samples / SAMPLE_RATE);

    // Write WAV header
    if (write_wav_header(out, total_samples) != 0) {
        fprintf(stderr, "Error: Cannot write WAV header\n");
        goto cleanup;
    }

    printf("Writing audio data:\n");

    // Initial silence (1 second)
    printf("  - Initial silence (%d s)\n", INITIAL_SILENCE_SEC);
    if (write_silence(out, INITIAL_SILENCE_SAMPLES) != 0) {
        fprintf(stderr, "Error: Cannot write initial silence\n");
        goto cleanup;
    }

    // Write leader: 255 × 0x80
    printf("  - Leader (255 × 0x80)\n");
    for (int i = 0; i < LEAD_IN_0x80_COUNT; i++) {
        if (write_byte_as_audio(out, 0x80) != 0) {
            fprintf(stderr, "Error: Cannot write leader\n");
            goto cleanup;
        }
    }

    // Write preamble: 5 × 0xFE
    printf("  - Preamble (5 × 0xFE)\n");
    for (int i = 0; i < LEAD_IN_0xFE_COUNT; i++) {
        if (write_byte_as_audio(out, 0xFE) != 0) {
            fprintf(stderr, "Error: Cannot write preamble\n");
            goto cleanup;
        }
    }

    // Write file type
    printf("  - File type (0x%02X)\n", vz_type);
    if (write_byte_as_audio(out, vz_type) != 0) {
        fprintf(stderr, "Error: Cannot write file type\n");
        goto cleanup;
    }

    // Write filename
    printf("  - Filename (%s)\n", vz_filename);
    for (size_t i = 0; i <= filename_length; i++) {
        uint8_t ch = (i < filename_length) ? vz_filename[i] : 0;
        if (write_byte_as_audio(out, ch) != 0) {
            fprintf(stderr, "Error: Cannot write filename\n");
            goto cleanup;
        }
    }

    // Write gap (silence)
    printf("  - Gap (silence)\n");
    if (write_silence(out, GAP_SILENCE) != 0) {
        fprintf(stderr, "Error: Cannot write gap\n");
        goto cleanup;
    }

    // write null gap (NULL)
    printf("  - Null GAP\n");
    if (write_null(out, NULL_GAP_SAMPLES) !=0) {
        fprintf(stderr, "Error: Cannot write gap\n");
        goto cleanup;
    }

    // Write addresses
    printf("  - Start address (0x%04X)\n", vz_start_addr);
    if (write_byte_as_audio(out, vz_start_addr & 0xFF) != 0 ||
        write_byte_as_audio(out, (vz_start_addr >> 8) & 0xFF) != 0) {
        fprintf(stderr, "Error: Cannot write start address\n");
        goto cleanup;
    }

    printf("  - End address (0x%04X)\n", vz_end_addr);
    if (write_byte_as_audio(out, vz_end_addr & 0xFF) != 0 ||
        write_byte_as_audio(out, (vz_end_addr >> 8) & 0xFF) != 0) {
        fprintf(stderr, "Error: Cannot write end address\n");
        goto cleanup;
    }

    // Write data
    printf("  - Data (%zu bytes)\n", data_length);
    for (size_t i = 0; i < data_length; i++) {
        if (write_byte_as_audio(out, vz_data[i]) != 0) {
            fprintf(stderr, "Error: Cannot write data at byte %zu\n", i);
            goto cleanup;
        }
        if (i % 100 == 0) {
            printf("\r    Progress: %zu/%zu bytes", i, data_length);
            fflush(stdout);
        }
    }
    printf("\r    Progress: %zu/%zu bytes\n", data_length, data_length);

    // Write checksum
    printf("  - Checksum (0x%04X)\n", checksum);
    if (write_byte_as_audio(out, checksum & 0xFF) != 0 ||
        write_byte_as_audio(out, (checksum >> 8) & 0xFF) != 0) {
        fprintf(stderr, "Error: Cannot write checksum\n");
        goto cleanup;
    }

    // Write lead-out: 20 × 0x00
    printf("  - Lead-out (20 × 0x00)\n");
    for (int i = 0; i < LEAD_OUT_0x00_COUNT; i++) {
        if (write_byte_as_audio(out, 0x00) != 0) {
            fprintf(stderr, "Error: Cannot write lead-out\n");
            goto cleanup;
        }
    }
    // Initial silence (1 second)
    printf("  - Tail Silence (%d s)\n", TAIL_SILENCE_SEC);
    if (write_silence(out, TAIL_SILENCE_SAMPLES) != 0) {
        fprintf(stderr, "Error: Cannot write initial silence\n");
        goto cleanup;
    }

    printf("\nConversion complete!\n");
    result = 0;

cleanup:
    if (vz_data) free(vz_data);
    if (in) fclose(in);
    if (out) fclose(out);

    return result;
}

int main(int argc, char *argv[])
{
    printf("VZ to WAV Converter (Original Hardware Compatible)\n");
    printf("===================================================\n\n");

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.vz> <output.wav>\n", argv[0]);
        fprintf(stderr, "\nGenerates 22050 Hz, 8-bit unsigned PCM WAV files\n");
        fprintf(stderr, "compatible with original VZ200 hardware and wav2vz\n");
        return 1;
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];

    printf("Input:  %s\n", input_file);
    printf("Output: %s\n\n", output_file);

    if (convert_vz_to_wav(input_file, output_file) != 0) {
        return 1;
    }

    return 0;
}
