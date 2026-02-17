/* VZ200 Cassette to WAV Converter - Fixed Implementation */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

/* Configuration structure */
typedef struct {
    FILE *input_file;
    FILE *output_file;
    unsigned char file_data[65536];
    unsigned int file_size;
} CONVERTER_STATE;

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
{
    CONVERTER_STATE state;
    VZ_FILE_HEADER header;
    unsigned char checksum_low, checksum_high;
    unsigned int start_addr, end_addr;
    unsigned int file_length;
    
    /* Open input file */
    state.input_file = fopen(input_filename, "rb");
    if (state.input_file == NULL)
    {
        fprintf(stderr, "Error: Cannot open input file %s\n", input_filename);
        return 1;
    }
    
    printf("Opened input file: %s\n", input_filename);
    
    /* Read VZ file header */
    if (fread(&header, sizeof(header), 1, state.input_file) != 1)
    {
        fprintf(stderr, "Error: Cannot read VZ file header\n");
        fclose(state.input_file);
        return 1;
    }
    
    printf("Header raw bytes: %02x %02x %02x %02x | %.13s... | %02x %02x %02x %02x\n",
           header.desc1, header.desc2, header.type, header.reserved,
           header.filename,
           header.start_addr_low, header.start_addr_high, 
           header.end_addr_low, header.end_addr_high);
    
    /* Extract address information (little-endian) */
    start_addr = (header.start_addr_high << 8) | header.start_addr_low;
    end_addr = (header.end_addr_high << 8) | header.end_addr_low;
    
    /* Check if addresses look valid; if not, try interpreting differently */
    if (end_addr < start_addr && start_addr == 0)
    {
        /* Try big-endian */
        start_addr = (header.start_addr_low << 8) | header.start_addr_high;
        end_addr = (header.end_addr_low << 8) | header.end_addr_high;
    }
    
    file_length = end_addr - start_addr;
    if (file_length <= 0 || file_length > 65536)
    {
        fprintf(stderr, "Error: Invalid file length: 0x%04x\n", file_length);
        fclose(state.input_file);
        return 1;
    }
    
    printf("File type: 0x%02x\n", header.type);
    printf("Filename: %.13s\n", header.filename);
    printf("Start address: 0x%04x\n", start_addr);
    printf("End address: 0x%04x\n", end_addr);
    printf("File length: 0x%04x bytes\n", file_length);
    
    /* Read file data from VZ file */
    state.file_size = 0;
    while (state.file_size < file_length && state.file_size < sizeof(state.file_data))
    {
        int ch = fgetc(state.input_file);
        if (ch == EOF)
            break;
        
        state.file_data[state.file_size++] = (unsigned char)ch;
    }
    
    printf("Read %d bytes from file\n", state.file_size);
    
    /* Open output audio file */
    state.output_file = fopen(output_filename, "wb");
    if (state.output_file == NULL)
    {
        fprintf(stderr, "Error: Cannot open output file %s\n", output_filename);
        fclose(state.input_file);
        return 1;
    }
    
    printf("Opened output file: %s\n", output_filename);
    
    /* Write audio cassette format */
    
    /* 1. Write WAV header (44 bytes) */
    write_wav_header(&state);
    
    /* 2. Write initial silence (22050 samples) */
    write_silence(&state, 22050);
    
    /* 3. Write leader (0x80 byte repeated 255 times) */
    for (int i = 0; i < 0xFF; i++)
    {
        write_vz_byte(&state, 0x80);
    }
    
    /* 4. Write preamble (0xFE byte repeated 5 times) */
    for (int i = 0; i < 5; i++)
    {
        write_vz_byte(&state, 0xFE);
    }
    
    /* 5. Write file type byte */
    write_vz_byte(&state, header.type);
    
    /* 6. Write filename (null-terminated string, max 17 bytes) */
    for (int i = 0; i < 17; i++)
    {
        write_vz_byte(&state, header.filename[i]);
        if (header.filename[i] == 0)
            break;
    }
    
    /* 7. Write null delay (80 samples) */
    write_silence(&state, 80);
    
    /* 8. Write file data - start address bytes */
    write_vz_byte(&state, header.start_addr_low);
    write_vz_byte(&state, header.start_addr_high);
    write_vz_byte(&state, header.end_addr_low);
    write_vz_byte(&state, header.end_addr_high);
    
    /* 9. Write file data bytes with checksum calculation */
    unsigned int checksum = header.start_addr_low + header.start_addr_high + 
                            header.end_addr_low + header.end_addr_high;
    
    for (unsigned int i = 0; i < state.file_size; i++)
    {
        write_vz_byte(&state, state.file_data[i]);
        checksum += state.file_data[i];
    }
    
    /* 10. Write checksum bytes */
    write_vz_byte(&state, (unsigned char)(checksum & 0xFF));
    write_vz_byte(&state, (unsigned char)((checksum >> 8) & 0xFF));
    
    /* 11. Write trailing silence (22050 samples) */
    write_silence(&state, 22050);
    
    printf("Conversion complete!\n");
    
    /* Close files */
    fclose(state.output_file);
    fclose(state.input_file);
    
    return 0;
}

/* Main entry point */
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("VZ200 Cassette to WAV Converter\n");
        printf("Usage: %s <input.vz> <output.wav>\n", argv[0]);
        return 1;
    }
    
    return convert_vz_to_wav(argv[1], argv[2]);
}
