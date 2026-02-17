/*
 * WAV to VZ Converter - Reverse of vz2wav  
 * Decodes WAV files back to VZ200 cassette format.
 * Based on vz2wav.c structure and timing constants.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

/* VZ file format constants - MUST match vz2wav.c */
#define VZ_MAGIC_SIZE 4
#define VZ_FILENAME_SIZE 17
#define VZ_TYPE_OFFSET 21
#define VZ_START_ADDR_OFFSET 22
#define VZ_HEADER_SIZE 24

/* WAV format constants */
#define SAMPLE_RATE 22050
#define BITS_PER_SAMPLE 8
#define CHANNELS 1

/* Timing constants - MUST match vz2wav.c */
#define HALF_SHORT_CYCLE_NS 287103L
#define SAMPLES_PER_HALF_CYCLE ((SAMPLE_RATE * HALF_SHORT_CYCLE_NS) / 1000000000L)
#define SAMPLES_PER_BIT (6 * SAMPLES_PER_HALF_CYCLE)
#define GAP_TIME_NS 3065000L
#define GAP_SAMPLES ((SAMPLE_RATE * GAP_TIME_NS) / 1000000000L)
#define NULL_GAP_SAMPLES 10
#define GAP_SILENCE (GAP_SAMPLES - NULL_GAP_SAMPLES)
#define INITIAL_SILENCE_SAMPLES (SAMPLE_RATE * 1)

/* Audio level constants */
#define VALUE_HI 195
#define VALUE_LO 61
#define VALUE_SILENCE 127
#define THRESHOLD 128

/* WAV file header */
typedef struct {
    char riff[4];
    uint32_t file_size;
    char wave[4];
    char fmt[4];
    uint32_t fmt_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char data[4];
    uint32_t data_size;
} __attribute__((packed)) WavHeader;

static uint8_t *audio_data = NULL;
static size_t audio_size = 0;
static size_t audio_pos = 0;

static inline uint8_t read_sample(void) {
    if (audio_pos >= audio_size) return 0;
    return audio_data[audio_pos++];
}

static inline void skip_samples(size_t count) {
    audio_pos += count;
    if (audio_pos > audio_size) audio_pos = audio_size;
}

/* Decode a single bit using half-cycle analysis */
static int decode_bit(void) {
    int low_hc3 = 0, total_hc3 = 0;
    
    for (int i = 0; i < SAMPLES_PER_BIT; i++) {
        uint8_t sample = read_sample();
        int half_cycle = i / SAMPLES_PER_HALF_CYCLE;
        
        /* Discriminator is in half-cycle 3 */
        if (half_cycle == 3) {
            total_hc3++;
            if (sample <= THRESHOLD) {
                low_hc3++;
            }
        }
    }
    
    /* HC3 LOW -> bit=1; HC3 HIGH -> bit=0 */
    return (low_hc3 * 2 > total_hc3) ? 1 : 0;
}

/* Decode one byte (8 bits, MSB first) */
static uint8_t decode_byte(void) {
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++) {
        byte = (byte << 1) | (decode_bit() & 1);
    }
    return byte;
}

/* Find synchronization on leader (0x80 bytes) */
static int find_sync(void) {
    int consecutive = 0;
    int max_attempts = 400;
    
    printf("Searching for leader...\n");
    
    while (consecutive < 255 && max_attempts-- > 0) {
        uint8_t byte = decode_byte();
        
        if (byte == 0x80) {
            consecutive++;
            if (consecutive % 50 == 0) {
                printf("  Found %d/255 bytes\n", consecutive);
            }
        } else {
            if (consecutive > 0) {
                printf("  Lost sync at %d (got 0x%02X)\n", consecutive, byte);
            }
            consecutive = 0;
        }
    }
    
    if (consecutive < 255) {
        fprintf(stderr, "Error: Leader not found\n");
        return 1;
    }
    
    printf("  Leader found!\n");
    return 0;
}

/* Verify preamble (5 × 0xFE) */
static int verify_preamble(void) {
    printf("Verifying preamble...\n");
    for (int i = 0; i < 5; i++) {
        uint8_t byte = decode_byte();
        if (byte != 0xFE) {
            fprintf(stderr, "Error: Preamble byte %d = 0x%02X\n", i, byte);
            return 1;
        }
    }
    printf("  Preamble OK\n");
    return 0;
}

/* Main converter */
int wav_to_vz(const char *input_file, const char *output_file) {
    FILE *in = NULL, *out = NULL;
    WavHeader header;
    int result = 1;
    
    /* Open and read WAV */
    in = fopen(input_file, "rb");
    if (!in) {
        fprintf(stderr, "Error: Cannot open '%s'\n", input_file);
        return 1;
    }
    
    if (fread(&header, sizeof(header), 1, in) != 1) {
        fprintf(stderr, "Error: Cannot read WAV header\n");
        fclose(in);
        return 1;
    }
    
    if (memcmp(header.riff, "RIFF", 4) != 0 || memcmp(header.wave, "WAVE", 4) !=  0) {
        fprintf(stderr, "Error: Invalid WAV file\n");
        fclose(in);
        return 1;
    }
    
    printf("WAV File Information:\n");
    printf("  Sample Rate: %d Hz\n", header.sample_rate);
    printf("  Channels: %d\n", header.num_channels);
    printf("  Bits per Sample: %d\n", header.bits_per_sample);
    printf("  Data Size: %d bytes\n\n", header.data_size);
    
    if (header.sample_rate != SAMPLE_RATE || header.bits_per_sample != BITS_PER_SAMPLE || 
        header.num_channels != CHANNELS) {
        fprintf(stderr, "Error: Expected 22050 Hz, 8-bit, mono\n");
        fclose(in);
        return 1;
    }
    
    /* Load audio */
    audio_size = header.data_size;
    audio_data = malloc(audio_size);
    if (!audio_data) {
        fprintf(stderr, "Error: Memory alloc failed\n");
        fclose(in);
        return 1;
    }
    
    if (fread(audio_data, 1, audio_size, in) != audio_size) {
        fprintf(stderr, "Error: Cannot read audio\n");
        free(audio_data);
        fclose(in);
        return 1;
    }
    
    fclose(in);
    audio_pos = 0;
    
    printf("Decoding audio...\n\n");
    
    /* Skip initial silence */
    printf("Skipping initial silence\n");
    skip_samples(INITIAL_SILENCE_SAMPLES);
    
    /* Find sync and verify preamble */
    if (find_sync() != 0 || verify_preamble() != 0) {
        goto cleanup;
    }
    
    /* Read file type byte */
    uint8_t file_type = decode_byte();
    printf("File Type: 0x%02X\n", file_type);
    
    /* Read filename (null-terminated, variable length) */
    uint8_t filename[VZ_FILENAME_SIZE + 1];
    int fn_len = 0;
    for (int i = 0; i < VZ_FILENAME_SIZE + 1; i++) {
        uint8_t ch = decode_byte();
        filename[i] = ch;
        if (ch == 0) {
            fn_len = i;
            break;
        }
        if (isprint(ch)) fn_len = i + 1;
    }
    filename[VZ_FILENAME_SIZE] = '\0';
    printf("Filename: %s\n", (char *)filename);
    
    /* Skip gap */
    printf("Skipping gap\n");
    skip_samples(GAP_SILENCE + NULL_GAP_SAMPLES);
    
    /* Read start address (2 bytes) */
    uint8_t start_lo = decode_byte();
    uint8_t start_hi = decode_byte();
    uint16_t vz_start = start_lo | (start_hi << 8);
    printf("Start Address: 0x%04X\n", vz_start);
    
    /* Read end address (2 bytes) */
    uint8_t end_lo = decode_byte();
    uint8_t end_hi = decode_byte();
    uint16_t vz_end = end_lo | (end_hi << 8);
    printf("End Address: 0x%04X\n", vz_end);
    
    size_t data_len = vz_end - vz_start;
    printf("Data Length: %zu bytes\n\n", data_len);
    
    /* Read data */
    printf("Reading data...\n");
    uint8_t *vz_data = malloc(data_len);
    if (!vz_data) {
        fprintf(stderr, "Error: Memory alloc failed\n");
        goto cleanup;
    }
    
    uint16_t checksum = start_lo + start_hi + end_lo + end_hi;
    
    for (size_t i = 0; i < data_len; i++) {
        vz_data[i] = decode_byte();
        checksum += vz_data[i];
        if ((i + 1) % 1000 == 0) printf("  %zu/%zu\n", i + 1, data_len);
    }
    printf("  Data complete!\n");
    
    /* Read and verify checksum */
    uint8_t cs_lo = decode_byte();
    uint8_t cs_hi = decode_byte();
    uint16_t file_cs = cs_lo | (cs_hi << 8);
    checksum &= 0xFFFF;
    printf("Checksum: 0x%04X (file: 0x%04X)\n", checksum, file_cs);
    
    if (checksum != file_cs) {
        fprintf(stderr, "Warning: Checksum mismatch\n");
    }
    
    /* Write VZ file */
    printf("\nWriting VZ file...\n");
    out = fopen(output_file, "wb");
    if (!out) {
        fprintf(stderr, "Error: Cannot create '%s'\n", output_file);
        free(vz_data);
        goto cleanup;
    }
    
    /* Build VZ header (24 bytes) - MUST match vz2wav.c structure exactly */
    uint8_t vz_header[VZ_HEADER_SIZE];
    memset(vz_header, 0, VZ_HEADER_SIZE);
    
    /* Bytes 0-3: Magic (4 bytes) */
    memset(&vz_header[0], 0x20, 2);  /* Two space characters */
    vz_header[2] = 0x00;
    vz_header[3] = 0x00;
    
    /* Bytes 4-20: Filename padded to 17 bytes with zeros */
    memcpy(&vz_header[VZ_MAGIC_SIZE], filename, (fn_len < VZ_FILENAME_SIZE) ? fn_len : VZ_FILENAME_SIZE);
    
    /* Byte 21: File type */
    vz_header[VZ_TYPE_OFFSET] = file_type;
    
    /* Bytes 22-23: Start address (little-endian) */
    vz_header[VZ_START_ADDR_OFFSET] = start_lo;
    vz_header[VZ_START_ADDR_OFFSET + 1] = start_hi;
    
    /* Write header */
    if (fwrite(vz_header, 1, VZ_HEADER_SIZE, out) != VZ_HEADER_SIZE) {
        fprintf(stderr, "Error: Cannot write header\n");
        fclose(out);
        free(vz_data);
        goto cleanup;
    }
    
    /* Write data */
    if (fwrite(vz_data, 1, data_len, out) != data_len) {
        fprintf(stderr, "Error: Cannot write data\n");
        fclose(out);
        free(vz_data);
        goto cleanup;
    }
    
    fclose(out);
    free(vz_data);
    
    printf("VZ file written successfully!\n");
    result = 0;

cleanup:
    if (audio_data) free(audio_data);
    return result;
}

int main(int argc, char *argv[]) {
    printf("WAV to VZ Converter\n");
    printf("==================\n\n");
    
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.wav> <output.vz>\n", argv[0]);
        return 1;
    }
    
    printf("Input:  %s\n", argv[1]);
    printf("Output: %s\n\n", argv[2]);
    
    int ret = wav_to_vz(argv[1], argv[2]);
    return ret ? 1 : 0;
}
