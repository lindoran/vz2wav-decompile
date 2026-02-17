/* VZ200 Cassette to WAV Converter - Fixed Implementation */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* VZ200 Constants */
#define SAMPLE_RATE 22050
#define BITS_PER_SAMPLE 8
#define HALF_SHORT_CYCLE_NS 287103L
#define SAMPLES_PER_HALF_CYCLE 6

#define VALUE_HI 195
#define VALUE_LO 61
#define VALUE_SILENCE 127
#define VALUE_NULL 0

#define LEAD_IN_0x80_COUNT 255
#define LEAD_IN_0xFE_COUNT 5
#define LEAD_OUT_0x00_COUNT 20
#define INITIAL_SILENCE_SEC 1
#define TAIL_SILENCE_SEC 1
#define INITIAL_SILENCE_SAMPLES (SAMPLE_RATE * INITIAL_SILENCE_SEC)
#define TAIL_SILENCE_SAMPLES (SAMPLE_RATE * TAIL_SILENCE_SEC)
#define GAP_SILENCE 8820
#define NULL_GAP_SAMPLES 1470
#define SAMPLES_PER_BIT (6 * SAMPLES_PER_HALF_CYCLE)

#define VZ_HEADER_SIZE 24
#define VZ_MAGIC_SIZE 4
#define VZ_FILENAME_SIZE 17

/* WAV File Header Structure */
typedef struct {
    char riff_id[4];          // "RIFF"
    uint32_t file_size;       // File size - 8
    char riff_type[4];        // "WAVE"
    char fmt_id[4];           // "fmt "
    uint32_t fmt_size;        // 16
    uint16_t audio_format;    // 1 = PCM
    uint16_t num_channels;    // 1 = mono
    uint32_t sample_rate;     // 22050
    uint32_t byte_rate;       // sample_rate * channels * bits/8
    uint16_t block_align;     // channels * bits/8
    uint16_t bits_per_sample; // 8
    char data_id[4];          // "data"
    uint32_t data_size;       // data size in bytes
} WavHeader;

/* Determine the audio value for a bit at a specific position within the bit period */
static uint8_t get_bit_value(int bit, int sample_in_bit) {
    int half_cycle = sample_in_bit / SAMPLES_PER_HALF_CYCLE;
    switch (half_cycle) {
        case 0: return VALUE_HI;
        case 1: return VALUE_LO;
        case 2: return VALUE_HI;
        case 3: return (bit == 1) ? VALUE_LO : VALUE_HI;
        case 4: return (bit == 1) ? VALUE_HI : VALUE_LO;
        default: return VALUE_LO;
    }
}

/* Write a single byte as audio (8 bits, MSB first) */
int write_byte_as_audio(FILE *output, uint8_t byte_val) {
    for (int bit_idx = 0; bit_idx < 8; bit_idx++) {
        int bit = (byte_val >> (7 - bit_idx)) & 1;
        for (int sample_in_bit = 0; sample_in_bit < SAMPLES_PER_BIT; sample_in_bit++) {
            uint8_t audio_val = get_bit_value(bit, sample_in_bit);
            if (fputc(audio_val, output) == EOF) {
                return 1;
            }
        }
    }
    return 0;
}

/* Write silence (VALUE_SILENCE samples) */
int write_silence(FILE *output, size_t sample_count) {
    for (size_t i = 0; i < sample_count; i++) {
        if (fputc(VALUE_SILENCE, output) == EOF) {
            return 1;
        }
    }
    return 0;
}

/* Write null/gap (VALUE_NULL samples) */
int write_null(FILE *output, size_t sample_count) {
    for (size_t i = 0; i < sample_count; i++) {
        if (fputc(VALUE_NULL, output) == EOF) {
            return 1;
        }
    }
    return 0;
}

/* Calculate total number of samples needed for WAV file */
static size_t calculate_total_samples(size_t data_length, size_t filename_length) {
    size_t samples = 0;
    
    // Initial silence
    samples += INITIAL_SILENCE_SAMPLES;
    
    // Leader (255 × 0x80): 255 bytes × 8 bits × samples per bit
    samples += LEAD_IN_0x80_COUNT * 8 * SAMPLES_PER_BIT;
    
    // Preamble (5 × 0xFE): 5 bytes × 8 bits × samples per bit
    samples += LEAD_IN_0xFE_COUNT * 8 * SAMPLES_PER_BIT;
    
    // File type (1 byte)
    samples += 1 * 8 * SAMPLES_PER_BIT;
    
    // Filename (including null terminator)
    samples += (filename_length + 1) * 8 * SAMPLES_PER_BIT;
    
    // Gap (silence)
    samples += GAP_SILENCE;
    
    // Null gap
    samples += NULL_GAP_SAMPLES;
    
    // Addresses (4 bytes)
    samples += 4 * 8 * SAMPLES_PER_BIT;
    
    // Data
    samples += data_length * 8 * SAMPLES_PER_BIT;
    
    // Checksum (2 bytes)
    samples += 2 * 8 * SAMPLES_PER_BIT;
    
    // Lead-out (20 × 0x00)
    samples += LEAD_OUT_0x00_COUNT * 8 * SAMPLES_PER_BIT;
    
    // Tail silence
    samples += TAIL_SILENCE_SAMPLES;
    
    return samples;
}

/* Write WAV file header */
static int write_wav_header(FILE *output, size_t total_samples) {
    WavHeader header;
    uint32_t data_size = total_samples;  // Each sample is 1 byte (8-bit)
    uint32_t file_size = 36 + data_size;
    
    memcpy(header.riff_id, "RIFF", 4);
    header.file_size = file_size;
    memcpy(header.riff_type, "WAVE", 4);
    memcpy(header.fmt_id, "fmt ", 4);
    header.fmt_size = 16;
    header.audio_format = 1;  // PCM
    header.num_channels = 1;  // Mono
    header.sample_rate = SAMPLE_RATE;
    header.byte_rate = SAMPLE_RATE * 1 * 1;  // channels * bits/8
    header.block_align = 1;
    header.bits_per_sample = BITS_PER_SAMPLE;
    memcpy(header.data_id, "data", 4);
    header.data_size = data_size;
    
    if (fwrite(&header, sizeof(header), 1, output) != 1) {
        return 1;
    }
    
    return 0;
}

/* Main converter function */
int convert_vz_to_wav(const char *input_filename, const char *output_filename)
{
    FILE *in = NULL;
    FILE *out = NULL;
    unsigned char *vz_data = NULL;
    int result = 1;
    
    /* Open input file */
    in = fopen(input_filename, "rb");
    if (!in) {
        fprintf(stderr, "Error: Cannot open input file '%s'\n", input_filename);
        goto cleanup;
    }
    
    /* Read VZ file header (24 bytes) */
    unsigned char vz_header[VZ_HEADER_SIZE];
    if (fread(vz_header, VZ_HEADER_SIZE, 1, in) != 1) {
        fprintf(stderr, "Error: Cannot read VZ file header\n");
        goto cleanup;
    }
    
    /* Parse header */
    unsigned char vz_type = vz_header[2];
    uint16_t vz_start_addr = vz_header[4] | (vz_header[5] << 8);
    
    /* Extract filename (bytes 4-20, but check actual format) */
    char vz_filename[VZ_FILENAME_SIZE + 1];
    memcpy(vz_filename, &vz_header[4], VZ_FILENAME_SIZE);
    vz_filename[VZ_FILENAME_SIZE] = '\0';
    size_t filename_length = strlen(vz_filename);

    printf("VZ File Information:\n");
    printf("  Filename: %s\n", vz_filename);
    printf("  Type: 0x%02X\n", vz_type);
    printf("  Start Address: 0x%04X\n", vz_start_addr);

    /* Read data */
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

    /* Calculate checksum (addresses + data) */
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

    /* Open output file */
    out = fopen(output_filename, "wb");
    if (!out) {
        fprintf(stderr, "Error: Cannot create output file '%s'\n", output_filename);
        goto cleanup;
    }

    /* Calculate total samples */
    size_t total_samples = calculate_total_samples(data_length, filename_length);
    printf("Generating WAV file:\n");
    printf("  Sample Rate: %d Hz\n", SAMPLE_RATE);
    printf("  Bit Depth: %d-bit\n", BITS_PER_SAMPLE);
    printf("  Total Samples: %zu\n", total_samples);
    printf("  Duration: %.2f seconds\n\n", (double)total_samples / SAMPLE_RATE);

    /* Write WAV header */
    if (write_wav_header(out, total_samples) != 0) {
        fprintf(stderr, "Error: Cannot write WAV header\n");
        goto cleanup;
    }

    printf("Writing audio data:\n");

    /* Initial silence (1 second) */
    printf("  - Initial silence (%d s)\n", INITIAL_SILENCE_SEC);
    if (write_silence(out, INITIAL_SILENCE_SAMPLES) != 0) {
        fprintf(stderr, "Error: Cannot write initial silence\n");
        goto cleanup;
    }

    /* Write leader: 255 × 0x80 */
    printf("  - Leader (255 × 0x80)\n");
    for (int i = 0; i < LEAD_IN_0x80_COUNT; i++) {
        if (write_byte_as_audio(out, 0x80) != 0) {
            fprintf(stderr, "Error: Cannot write leader\n");
            goto cleanup;
        }
    }

    /* Write preamble: 5 × 0xFE */
    printf("  - Preamble (5 × 0xFE)\n");
    for (int i = 0; i < LEAD_IN_0xFE_COUNT; i++) {
        if (write_byte_as_audio(out, 0xFE) != 0) {
            fprintf(stderr, "Error: Cannot write preamble\n");
            goto cleanup;
        }
    }

    /* Write file type */
    printf("  - File type (0x%02X)\n", vz_type);
    if (write_byte_as_audio(out, vz_type) != 0) {
        fprintf(stderr, "Error: Cannot write file type\n");
        goto cleanup;
    }

    /* Write filename */
    printf("  - Filename (%s)\n", vz_filename);
    for (size_t i = 0; i <= filename_length; i++) {
        uint8_t ch = (i < filename_length) ? vz_filename[i] : 0;
        if (write_byte_as_audio(out, ch) != 0) {
            fprintf(stderr, "Error: Cannot write filename\n");
            goto cleanup;
        }
    }

    /* Write gap (silence) */
    printf("  - Gap (silence)\n");
    if (write_silence(out, GAP_SILENCE) != 0) {
        fprintf(stderr, "Error: Cannot write gap\n");
        goto cleanup;
    }

    /* Write null gap */
    printf("  - Null GAP\n");
    if (write_null(out, NULL_GAP_SAMPLES) != 0) {
        fprintf(stderr, "Error: Cannot write null gap\n");
        goto cleanup;
    }

    /* Write addresses */
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

    /* Write data */
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

    /* Write checksum */
    printf("  - Checksum (0x%04X)\n", checksum);
    if (write_byte_as_audio(out, checksum & 0xFF) != 0 ||
        write_byte_as_audio(out, (checksum >> 8) & 0xFF) != 0) {
        fprintf(stderr, "Error: Cannot write checksum\n");
        goto cleanup;
    }

    /* Write lead-out: 20 × 0x00 */
    printf("  - Lead-out (20 × 0x00)\n");
    for (int i = 0; i < LEAD_OUT_0x00_COUNT; i++) {
        if (write_byte_as_audio(out, 0x00) != 0) {
            fprintf(stderr, "Error: Cannot write lead-out\n");
            goto cleanup;
        }
    }

    /* Tail silence (1 second) */
    printf("  - Tail Silence (%d s)\n", TAIL_SILENCE_SEC);
    if (write_silence(out, TAIL_SILENCE_SAMPLES) != 0) {
        fprintf(stderr, "Error: Cannot write tail silence\n");
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

/* Main entry point */
int main(int argc, char *argv[])
{
    printf("VZ to WAV Converter (Fixed Implementation)\n");
    printf("=========================================\n\n");

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.vz> <output.wav>\n", argv[0]);
        fprintf(stderr, "\nGenerates 22050 Hz, 8-bit unsigned PCM WAV files\n");
        fprintf(stderr, "compatible with original VZ200 hardware\n");
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
