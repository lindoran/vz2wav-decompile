/*
 * vzexport - unpack a .vz container into common external representations.
 *
 * The tool is intentionally strict about I/O errors but permissive about
 * output selection: callers can request one or many export formats in one run.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#ifndef TOOL_VERSION
#define TOOL_VERSION "dev"
#endif

/* 24-byte VZ header, followed by raw payload bytes. */
#define VZ_HEADER_SIZE 24

/*
 * VZ BASIC token table used for detokenization.
 * Index is token_byte - 0x80.
 */
static const char *vz_tokens[128 + 1] = {
    "END","FOR","RESET","SET","CLS","","RANDOM","NEXT",
    "DATA","INPUT","DIM","READ","LET","GOTO","RUN","IF",
    "RESTORE","GOSUB","RETURN","REM","STOP","ELSE","COPY","COLOR",
    "VERIFY","DEFINT","DEFSNG","DEFDBL","CRUN","MODE","SOUND","RESUME",
    "OUT","ON","OPEN","FIELD","GET","PUT","CLOSE","LOAD",
    "MERGE","NAME","KILL","LSET","RSET","SAVE","SYSTEM","LPRINT",
    "DEF","POKE","PRINT","CONT","LIST","LLIST","DELETE","AUTO",
    "CLEAR","CLOAD","CSAVE","NEW","TAB(","TO","FN","USING",
    "VARPTR","USR","ERL","ERR","STRING$","INSTR","POINT","TIME$",
    "MEM","INKEY$","THEN","NOT","STEP","+","-","*",
    "/","^","AND","OR",">","=","<","SGN",
    "INT","ABS","FRE","INP","POS","SQR","RND","LOG",
    "EXP","COS","SIN","TAN","ATN","PEEK","CVI","CVS",
    "CVD","EOF","LOC","LOF","MKI$","MKS$","MKD$","CINT",
    "CSNG","CDBL","FIX","LEN","STR$","VAL","ASC","CHR$",
    "LEFT$","RIGHT$","MID$","'","","","","",NULL
};

typedef struct {
    uint8_t magic[4];
    char filename[17];
    uint8_t file_type;
    uint16_t start_addr;
} VzHeader;

static void die(const char *msg)
{
    fprintf(stderr, "vzexport: %s\n", msg);
    exit(1);
}

static uint16_t le16(const uint8_t *p)
{
    return (uint16_t)p[0] | (uint16_t)((uint16_t)p[1] << 8);
}

static int write_bin(const char *path, const uint8_t *data, size_t len)
{
    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    if (len > 0 && fwrite(data, 1, len, f) != len) {
        fclose(f);
        return -1;
    }
    fclose(f);
    return 0;
}

static int write_ihex_record(FILE *f, uint8_t type, uint16_t addr, const uint8_t *data, uint8_t len)
{
    uint8_t sum = (uint8_t)(len + (uint8_t)(addr >> 8) + (uint8_t)(addr & 0xFF) + type);
    uint8_t i;

    if (fprintf(f, ":%02X%04X%02X", (unsigned)len, (unsigned)addr, (unsigned)type) < 0)
        return -1;

    for (i = 0; i < len; i++) {
        sum = (uint8_t)(sum + data[i]);
        if (fprintf(f, "%02X", (unsigned)data[i]) < 0)
            return -1;
    }

    sum = (uint8_t)(0u - sum);
    if (fprintf(f, "%02X\n", (unsigned)sum) < 0)
        return -1;
    return 0;
}

static int write_ihex(const char *path, const uint8_t *data, size_t len, uint32_t base_addr)
{
    FILE *f = fopen(path, "wb");
    size_t off = 0;
    uint32_t ext = 0xFFFFFFFFu;

    if (!f) return -1;

    /*
     * Emit 16-byte data records and only change type-04 extended address
     * records when the upper 16 bits actually move.
     */
    while (off < len) {
        uint8_t rec_len = (uint8_t)((len - off) > 16 ? 16 : (len - off));
        uint32_t addr32 = base_addr + (uint32_t)off;
        uint32_t need_ext = addr32 >> 16;
        uint16_t addr16 = (uint16_t)(addr32 & 0xFFFFu);

        if (need_ext != ext) {
            uint8_t ext_data[2];
            ext_data[0] = (uint8_t)((need_ext >> 8) & 0xFFu);
            ext_data[1] = (uint8_t)(need_ext & 0xFFu);
            if (write_ihex_record(f, 0x04u, 0x0000u, ext_data, 2) < 0) {
                fclose(f);
                return -1;
            }
            ext = need_ext;
        }

        if (write_ihex_record(f, 0x00u, addr16, data + off, rec_len) < 0) {
            fclose(f);
            return -1;
        }
        off += rec_len;
    }

    if (write_ihex_record(f, 0x01u, 0x0000u, NULL, 0) < 0) {
        fclose(f);
        return -1;
    }

    fclose(f);
    return 0;
}

static int write_srec(const char *path, const uint8_t *data, size_t len, uint32_t base_addr)
{
    FILE *f = fopen(path, "wb");
    size_t off = 0;
    int use_s2 = ((base_addr + (uint32_t)len) > 0x10000u);

    if (!f) return -1;

    /*
     * Pick S1/S9 for 16-bit space, S2/S8 when payload spills above 64K.
     * This keeps the output compact while still handling larger bases.
     */
    while (off < len) {
        uint8_t rec_len = (uint8_t)((len - off) > 16 ? 16 : (len - off));
        uint32_t addr = base_addr + (uint32_t)off;
        uint8_t count;
        uint8_t cksum;
        size_t i;

        if (use_s2) {
            count = (uint8_t)(rec_len + 4u);
            cksum = count;
            cksum = (uint8_t)(cksum + ((addr >> 16) & 0xFFu));
            cksum = (uint8_t)(cksum + ((addr >> 8) & 0xFFu));
            cksum = (uint8_t)(cksum + (addr & 0xFFu));
            if (fprintf(f, "S2%02X%06lX", (unsigned)count, (unsigned long)(addr & 0xFFFFFFu)) < 0) {
                fclose(f);
                return -1;
            }
        } else {
            count = (uint8_t)(rec_len + 3u);
            cksum = count;
            cksum = (uint8_t)(cksum + ((addr >> 8) & 0xFFu));
            cksum = (uint8_t)(cksum + (addr & 0xFFu));
            if (fprintf(f, "S1%02X%04X", (unsigned)count, (unsigned)(addr & 0xFFFFu)) < 0) {
                fclose(f);
                return -1;
            }
        }

        for (i = 0; i < rec_len; i++) {
            cksum = (uint8_t)(cksum + data[off + i]);
            if (fprintf(f, "%02X", (unsigned)data[off + i]) < 0) {
                fclose(f);
                return -1;
            }
        }

        cksum = (uint8_t)(~cksum);
        if (fprintf(f, "%02X\n", (unsigned)cksum) < 0) {
            fclose(f);
            return -1;
        }

        off += rec_len;
    }

    if (use_s2) {
        uint32_t addr = base_addr & 0xFFFFFFu;
        uint8_t count = 4u;
        uint8_t cksum = (uint8_t)(count + ((addr >> 16) & 0xFFu) + ((addr >> 8) & 0xFFu) + (addr & 0xFFu));
        cksum = (uint8_t)(~cksum);
        if (fprintf(f, "S804%06lX%02X\n", (unsigned long)addr, (unsigned)cksum) < 0) {
            fclose(f);
            return -1;
        }
    } else {
        uint16_t addr = (uint16_t)(base_addr & 0xFFFFu);
        uint8_t count = 3u;
        uint8_t cksum = (uint8_t)(count + ((addr >> 8) & 0xFFu) + (addr & 0xFFu));
        cksum = (uint8_t)(~cksum);
        if (fprintf(f, "S903%04X%02X\n", (unsigned)addr, (unsigned)cksum) < 0) {
            fclose(f);
            return -1;
        }
    }

    fclose(f);
    return 0;
}

static void emit_bas_byte(FILE *out, uint8_t b)
{
    if (b >= 0x80u) {
        const char *t = vz_tokens[b - 0x80u];
        if (t && *t) {
            fputs(t, out);
            return;
        }
    }

    if (b >= 32u && b <= 126u) {
        fputc((int)b, out);
    } else {
        fprintf(out, "\\x%02X", (unsigned)b);
    }
}

static int write_basic_text(const char *path, const uint8_t *data, size_t len, uint16_t start_addr)
{
    FILE *out = fopen(path, "wb");
    size_t pos = 0;

    if (!out) return -1;

    /*
     * BASIC lines are linked by absolute "next line" addresses.
     * Follow link pointers when sane; otherwise fall back to linear scan
     * so partially damaged payloads still produce useful text.
     */
    while (pos + 4 <= len) {
        uint16_t next_addr = le16(data + pos);
        uint16_t line_num = le16(data + pos + 2);
        size_t p = pos + 4;

        if (next_addr == 0u)
            break;

        fprintf(out, "%u ", (unsigned)line_num);
        while (p < len && data[p] != 0u) {
            emit_bas_byte(out, data[p]);
            p++;
        }
        fputc('\n', out);

        if (p >= len)
            break;

        if (next_addr > start_addr) {
            size_t next_pos = (size_t)(next_addr - start_addr);
            if (next_pos > pos && next_pos <= len) {
                pos = next_pos;
                continue;
            }
        }

        pos = p + 1;
    }

    fclose(out);
    return 0;
}

static void print_info(const VzHeader *hdr, size_t payload_len)
{
    char name[18];
    memcpy(name, hdr->filename, 17);
    name[17] = '\0';

    printf("Magic       : %c%c%c%c\n",
           hdr->magic[0], hdr->magic[1], hdr->magic[2], hdr->magic[3]);
    printf("Type        : %s (0x%02X)\n",
           hdr->file_type == 0xF0u ? "BASIC" :
           hdr->file_type == 0xF1u ? "Machine code" : "Unknown",
           (unsigned)hdr->file_type);
    printf("Filename    : %s\n", name);
    printf("Start addr  : 0x%04X\n", (unsigned)hdr->start_addr);
    printf("End addr    : 0x%04X\n", (unsigned)(hdr->start_addr + (uint16_t)payload_len));
    printf("Payload     : %lu bytes\n", (unsigned long)payload_len);
}

static void usage(const char *prog)
{
    fprintf(stderr,
        "vzexport version %s\n"
        "Usage: %s input.vz [options]\n"
        "\n"
        "Options:\n"
        "  --info, -i              Print VZ metadata\n"
        "  --out-bin,  -b FILE     Write raw payload bytes\n"
        "  --out-hex,  -x FILE     Write Intel HEX\n"
        "  --out-srec, -s FILE     Write Motorola S-record\n"
        "  --out-bas,  -B FILE     Detokenize BASIC to ASCII (BASIC only)\n"
        "  --version, -V           Print version and exit\n",
        TOOL_VERSION, prog);
}

int main(int argc, char **argv)
{
    const char *in_path = NULL;
    const char *out_bin = NULL;
    const char *out_hex = NULL;
    const char *out_srec = NULL;
    const char *out_bas = NULL;
    int want_info = 0;
    FILE *in = NULL;
    uint8_t *buf = NULL;
    size_t fsize;
    VzHeader hdr;
    const uint8_t *payload;
    size_t payload_len;
    int i;

    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    /*
     * Keep CLI parsing "compiler-like": options can appear before/after
     * the input file as long as exactly one positional input is provided.
     */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-V") == 0) {
            printf("vzexport version %s\n", TOOL_VERSION);
            return 0;
        } else
        if (strcmp(argv[i], "--info") == 0 || strcmp(argv[i], "-i") == 0) {
            want_info = 1;
        } else if ((strcmp(argv[i], "--out-bin") == 0 || strcmp(argv[i], "-b") == 0) && i + 1 < argc) {
            out_bin = argv[++i];
        } else if ((strcmp(argv[i], "--out-hex") == 0 || strcmp(argv[i], "-x") == 0) && i + 1 < argc) {
            out_hex = argv[++i];
        } else if ((strcmp(argv[i], "--out-srec") == 0 || strcmp(argv[i], "-s") == 0) && i + 1 < argc) {
            out_srec = argv[++i];
        } else if ((strcmp(argv[i], "--out-bas") == 0 || strcmp(argv[i], "-B") == 0) && i + 1 < argc) {
            out_bas = argv[++i];
        } else if (argv[i][0] == '-') {
            usage(argv[0]);
            return 1;
        } else if (!in_path) {
            in_path = argv[i];
        } else {
            usage(argv[0]);
            return 1;
        }
    }

    if (!in_path) {
        usage(argv[0]);
        return 1;
    }

    in = fopen(in_path, "rb");
    if (!in)
        die("cannot open input file");

    if (fseek(in, 0, SEEK_END) != 0)
        die("cannot seek input file");
    {
        long sz = ftell(in);
        if (sz < 0)
            die("cannot tell input file size");
        fsize = (size_t)sz;
    }
    if (fseek(in, 0, SEEK_SET) != 0)
        die("cannot seek input file");

    if (fsize < VZ_HEADER_SIZE)
        die("input is too short to be a VZ file");

    buf = (uint8_t *)malloc(fsize);
    if (!buf)
        die("out of memory");

    if (fread(buf, 1, fsize, in) != fsize)
        die("failed to read input file");
    fclose(in);
    in = NULL;

    memcpy(hdr.magic, buf, 4);
    memcpy(hdr.filename, buf + 4, 17);
    hdr.file_type = buf[21];
    hdr.start_addr = le16(buf + 22);

    payload = buf + VZ_HEADER_SIZE;
    payload_len = fsize - VZ_HEADER_SIZE;

    if (want_info || (!out_bin && !out_hex && !out_srec && !out_bas))
        print_info(&hdr, payload_len);

    if (out_bin && write_bin(out_bin, payload, payload_len) != 0)
        die("failed writing binary output");
    if (out_hex && write_ihex(out_hex, payload, payload_len, (uint32_t)hdr.start_addr) != 0)
        die("failed writing Intel HEX output");
    if (out_srec && write_srec(out_srec, payload, payload_len, (uint32_t)hdr.start_addr) != 0)
        die("failed writing SREC output");
    if (out_bas) {
        if (hdr.file_type != 0xF0u)
            die("--out-bas requested but file is not BASIC (type 0xF0)");
        if (write_basic_text(out_bas, payload, payload_len, hdr.start_addr) != 0)
            die("failed writing BASIC text output");
    }

    free(buf);
    return 0;
}
