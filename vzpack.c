/*
 * vzpack - build .vz containers from binary, Intel HEX, S-record, or BASIC.
 *
 * Design goal is determinism: once input is accepted, output layout is fixed
 * and uses a normalized VZ name and explicit start/type metadata.
 */
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <limits.h>

#ifndef TOOL_VERSION
#define TOOL_VERSION "dev"
#endif

#if defined(_WIN32) && !defined(strncasecmp)
#define strncasecmp _strnicmp
#endif

#define VZ_HEADER_SIZE 24
#define VZ_NAME_LEN 17
#define VZ_BASIC_START 0x7AE9u

/* Exactly one input source is accepted per invocation. */
typedef enum {
    INPUT_NONE = 0,
    INPUT_BIN,
    INPUT_HEX,
    INPUT_SREC,
    INPUT_BAS
} InputKind;

typedef enum {
    TYPE_AUTO = 0,
    TYPE_BASIC,
    TYPE_MC
} VzType;

typedef struct {
    uint8_t *data;
    size_t size;
    uint16_t start;
    int start_set;
} Payload;

/*
 * Sparse-address staging map for HEX/SREC import.
 * We only accept a final contiguous byte range, because .vz stores one
 * flat payload with a single start address.
 */
typedef struct {
    uint8_t *bytes;
    uint8_t *used;
    size_t size;
    uint32_t min_addr;
    uint32_t max_addr;
    int seen_any;
} AddrMap;

#if SIZE_MAX >= 65536u
#define ADDRMAP_CAPACITY 65536u
#else
#define ADDRMAP_CAPACITY 65535u
#endif

typedef struct tagLINE {
    unsigned short next;
    unsigned short num;
    unsigned char text[254];
} BasicLine;

static BasicLine g_line = {0, 0, {0}};
static unsigned short g_lineaddr = 0;
static unsigned short g_linenum = 1;

static char *vz_token[128 + 1] = {
    "END","FOR","RESET","SET","CLS",""/* CMD */,"RANDOM","NEXT",
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

static void die(const char *msg)
{
    fprintf(stderr, "vzpack: %s\n", msg);
    exit(1);
}

static void usage(const char *prog)
{
    fprintf(stderr,
        "vzpack version %s\n"
        "Usage: %s [input] -o output.vz [options]\n"
        "\n"
        "Input (exactly one):\n"
        "  --in-bin,  -b FILE      Raw binary payload\n"
        "  --in-hex,  -x FILE      Intel HEX\n"
        "  --in-srec, -r FILE      Motorola S-record\n"
        "  --in-bas,  -B FILE      ASCII BASIC source\n"
        "\n"
        "Options:\n"
        "  --out,   -o FILE        Output .vz path (required)\n"
        "  --name,  -n NAME        VZ header filename override\n"
        "  --type,  -t TYPE        basic | mc | auto (default auto)\n"
        "  --start, -s ADDR        Start address (required for --in-bin)\n"
        "  --fill,  -f BYTE        Fill holes for HEX/SREC sparse ranges (0..255)\n"
        "  --help,  -h             Show this help\n"
        "  --version, -V           Print version and exit\n",
        TOOL_VERSION, prog);
}

static int parse_u32(const char *s, uint32_t *out)
{
    char *end = NULL;
    unsigned long v;
    int base = 10;
    if (!s || !*s) return -1;
    if (s[0] == '$') {
        s++;
        base = 16;
    } else if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        base = 16;
    }
    v = strtoul(s, &end, base);
    if (*end != '\0') return -1;
    *out = (uint32_t)v;
    return 0;
}

static void normalize_name(char out[VZ_NAME_LEN], const char *src_path_or_name)
{
    const char *name = src_path_or_name;
    const char *slash1 = strrchr(src_path_or_name, '/');
    const char *slash2 = strrchr(src_path_or_name, '\\');
    const char *dot;
    size_t n, i;

    memset(out, 0, VZ_NAME_LEN);

    if (slash1 && (!slash2 || slash1 > slash2))
        name = slash1 + 1;
    else if (slash2)
        name = slash2 + 1;

    dot = strrchr(name, '.');
    if (dot && dot > name)
        n = (size_t)(dot - name);
    else
        n = strlen(name);
    if (n > 16) n = 16;

    for (i = 0; i < n; i++)
        out[i] = (char)toupper((unsigned char)name[i]);
}

static void payload_free(Payload *p)
{
    free(p->data);
    p->data = NULL;
    p->size = 0;
    p->start = 0;
    p->start_set = 0;
}

static int read_entire_file(const char *path, uint8_t **out_data, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    long sz;
    uint8_t *buf;
    if (!f) return -1;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return -1; }
    sz = ftell(f);
    if (sz < 0) { fclose(f); return -1; }
    if (fseek(f, 0, SEEK_SET) != 0) { fclose(f); return -1; }
    buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return -1; }
    if (sz > 0 && fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
        free(buf);
        fclose(f);
        return -1;
    }
    fclose(f);
    *out_data = buf;
    *out_size = (size_t)sz;
    return 0;
}

static int addrmap_init(AddrMap *m)
{
    m->size = ADDRMAP_CAPACITY;
    m->bytes = (uint8_t *)calloc(m->size, 1);
    m->used = (uint8_t *)calloc(m->size, 1);
    m->min_addr = 0xFFFFFFFFu;
    m->max_addr = 0u;
    m->seen_any = 0;
    if (!m->bytes || !m->used) {
        free(m->bytes);
        free(m->used);
        m->bytes = NULL;
        m->used = NULL;
        return -1;
    }
    return 0;
}

static void addrmap_free(AddrMap *m)
{
    free(m->bytes);
    free(m->used);
    m->bytes = NULL;
    m->used = NULL;
}

static int addrmap_set(AddrMap *m, uint32_t addr, uint8_t val)
{
    if (addr >= m->size) return -1;
    if (m->used[addr] && m->bytes[addr] != val) return -2;
    m->bytes[addr] = val;
    m->used[addr] = 1;
    if (!m->seen_any) {
        m->min_addr = addr;
        m->max_addr = addr;
        m->seen_any = 1;
    } else {
        if (addr < m->min_addr) m->min_addr = addr;
        if (addr > m->max_addr) m->max_addr = addr;
    }
    return 0;
}

static int addrmap_finalize(const AddrMap *m, Payload *p, int allow_fill, uint8_t fill_byte)
{
    uint32_t addr;
    size_t len;
    if (!m->seen_any) return -1;
    len = (size_t)(m->max_addr - m->min_addr + 1u);
    p->data = (uint8_t *)malloc(len);
    if (!p->data) return -3;
    if (allow_fill)
        memset(p->data, fill_byte, len);

    /*
     * Require a single linear image by address range. Out-of-order source
     * records are fine; holes require --fill/-f.
     */
    for (addr = m->min_addr; addr <= m->max_addr; addr++) {
        size_t off = (size_t)(addr - m->min_addr);
        if (m->used[addr]) {
            p->data[off] = m->bytes[addr];
        } else {
            if (!allow_fill) {
                free(p->data);
                p->data = NULL;
                return -2;
            }
        }
    }
    p->size = len;
    p->start = (uint16_t)m->min_addr;
    p->start_set = 1;
    return 0;
}

static int parse_hex_nibble(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    return -1;
}

static int parse_hex_byte(const char *s, uint8_t *out)
{
    int hi = parse_hex_nibble(s[0]);
    int lo = parse_hex_nibble(s[1]);
    if (hi < 0 || lo < 0) return -1;
    *out = (uint8_t)((hi << 4) | lo);
    return 0;
}

static int load_from_ihex(const char *path, Payload *p, int allow_fill, uint8_t fill_byte)
{
    FILE *f = fopen(path, "rb");
    char line[1024];
    AddrMap map;
    uint32_t upper = 0;
    int eof_seen = 0;
    if (!f) return -1;
    if (addrmap_init(&map) != 0) { fclose(f); return -1; }

    /*
     * Accept data, EOF, and common addressing records (02/04).
     * Unknown record types are ignored rather than treated as fatal.
     */
    while (fgets(line, sizeof(line), f)) {
        size_t len = strcspn(line, "\r\n");
        uint8_t reclen, rectype, cksum, calc;
        uint8_t b;
        uint16_t addr16;
        uint32_t addr;
        size_t i;
        if (len == 0) continue;
        if (line[0] != ':') { addrmap_free(&map); fclose(f); return -2; }
        if (len < 11) { addrmap_free(&map); fclose(f); return -2; }

        if (parse_hex_byte(line + 1, &reclen) != 0 ||
            parse_hex_byte(line + 3, &b) != 0) { addrmap_free(&map); fclose(f); return -2; }
        addr16 = (uint16_t)b << 8;
        if (parse_hex_byte(line + 5, &b) != 0) { addrmap_free(&map); fclose(f); return -2; }
        addr16 = (uint16_t)(addr16 | b);
        if (parse_hex_byte(line + 7, &rectype) != 0) { addrmap_free(&map); fclose(f); return -2; }

        if (len != (size_t)(11 + reclen * 2)) { addrmap_free(&map); fclose(f); return -2; }
        if (parse_hex_byte(line + 9 + reclen * 2, &cksum) != 0) { addrmap_free(&map); fclose(f); return -2; }

        calc = (uint8_t)(reclen + (uint8_t)(addr16 >> 8) + (uint8_t)(addr16 & 0xFF) + rectype);
        for (i = 0; i < reclen; i++) {
            if (parse_hex_byte(line + 9 + i * 2, &b) != 0) { addrmap_free(&map); fclose(f); return -2; }
            calc = (uint8_t)(calc + b);
        }
        calc = (uint8_t)(0u - calc);
        if (calc != cksum) { addrmap_free(&map); fclose(f); return -3; }

        if (rectype == 0x00u) {
            for (i = 0; i < reclen; i++) {
                if (parse_hex_byte(line + 9 + i * 2, &b) != 0) { addrmap_free(&map); fclose(f); return -2; }
                addr = upper + (uint32_t)addr16 + (uint32_t)i;
                if (addrmap_set(&map, addr, b) != 0) { addrmap_free(&map); fclose(f); return -4; }
            }
        } else if (rectype == 0x01u) {
            eof_seen = 1;
            break;
        } else if (rectype == 0x02u) {
            uint8_t hi, lo;
            if (reclen != 2) { addrmap_free(&map); fclose(f); return -2; }
            if (parse_hex_byte(line + 9, &hi) != 0 || parse_hex_byte(line + 11, &lo) != 0) { addrmap_free(&map); fclose(f); return -2; }
            upper = (uint32_t)(((uint32_t)hi << 8) | lo) << 4;
        } else if (rectype == 0x04u) {
            uint8_t hi, lo;
            if (reclen != 2) { addrmap_free(&map); fclose(f); return -2; }
            if (parse_hex_byte(line + 9, &hi) != 0 || parse_hex_byte(line + 11, &lo) != 0) { addrmap_free(&map); fclose(f); return -2; }
            upper = (uint32_t)(((uint32_t)hi << 8) | lo) << 16;
        } else {
            /*
             * Keep import tolerant for producer-specific metadata records
             * that do not contribute payload bytes.
             */
        }
    }

    fclose(f);
    if (!eof_seen) {
        addrmap_free(&map);
        return -2;
    }

    if (addrmap_finalize(&map, p, allow_fill, fill_byte) != 0) {
        addrmap_free(&map);
        return -5;
    }
    addrmap_free(&map);
    return 0;
}

static int srec_hexbyte(const char *s, uint8_t *out)
{
    return parse_hex_byte(s, out);
}

static int load_from_srec(const char *path, Payload *p, int allow_fill, uint8_t fill_byte)
{
    FILE *f = fopen(path, "rb");
    char line[1024];
    AddrMap map;
    int eof_seen = 0;
    if (!f) return -1;
    if (addrmap_init(&map) != 0) { fclose(f); return -1; }

    /*
     * Load S1/S2/S3 data records into the sparse map and require one of
     * S7/S8/S9 termination records before accepting the file.
     */
    while (fgets(line, sizeof(line), f)) {
        size_t len = strcspn(line, "\r\n");
        uint8_t count, cksum = 0, read_cksum;
        int addr_bytes;
        uint32_t addr = 0;
        size_t i, pos;

        if (len == 0) continue;
        if (line[0] != 'S' || len < 4) { addrmap_free(&map); fclose(f); return -2; }
        if (srec_hexbyte(line + 2, &count) != 0) { addrmap_free(&map); fclose(f); return -2; }

        if (line[1] == '1') addr_bytes = 2;
        else if (line[1] == '2') addr_bytes = 3;
        else if (line[1] == '3') addr_bytes = 4;
        else if (line[1] == '7' || line[1] == '8' || line[1] == '9') {
            eof_seen = 1;
            continue;
        } else if (line[1] == '0' || line[1] == '5' || line[1] == '6') {
            continue;
        } else {
            addrmap_free(&map);
            fclose(f);
            return -2;
        }

        if (len != (size_t)(4 + count * 2)) { addrmap_free(&map); fclose(f); return -2; }

        cksum = count;
        pos = 4;
        for (i = 0; i < (size_t)addr_bytes; i++) {
            uint8_t b;
            if (srec_hexbyte(line + pos, &b) != 0) { addrmap_free(&map); fclose(f); return -2; }
            cksum = (uint8_t)(cksum + b);
            addr = (addr << 8) | b;
            pos += 2;
        }

        if (count < (uint8_t)(addr_bytes + 1)) { addrmap_free(&map); fclose(f); return -2; }
        {
            uint8_t data_len = (uint8_t)(count - addr_bytes - 1);
            for (i = 0; i < data_len; i++) {
                uint8_t b;
                if (srec_hexbyte(line + pos, &b) != 0) { addrmap_free(&map); fclose(f); return -2; }
                cksum = (uint8_t)(cksum + b);
                if (addrmap_set(&map, addr + (uint32_t)i, b) != 0) { addrmap_free(&map); fclose(f); return -4; }
                pos += 2;
            }
        }

        if (srec_hexbyte(line + pos, &read_cksum) != 0) { addrmap_free(&map); fclose(f); return -2; }
        cksum = (uint8_t)(~cksum);
        if (cksum != read_cksum) { addrmap_free(&map); fclose(f); return -3; }
    }

    fclose(f);
    if (!eof_seen) {
        addrmap_free(&map);
        return -2;
    }

    if (addrmap_finalize(&map, p, allow_fill, fill_byte) != 0) {
        addrmap_free(&map);
        return -5;
    }
    addrmap_free(&map);
    return 0;
}

static void basic_tokenize_at(int i)
{
    int t;
    int len;
    size_t move_len;
    /* Greedy table walk preserves original text2bas behavior. */
    for (t = 0x80; vz_token[t - 0x80]; t++) {
        len = (int)strlen(vz_token[t - 0x80]);
        if (len && strncasecmp((char *)(g_line.text + i), vz_token[t - 0x80], (size_t)len) == 0) {
            move_len = strlen((char *)(g_line.text + i + len)) + 1;
            memmove((char *)(g_line.text + i + 1), (char *)(g_line.text + i + len), move_len);
            g_line.text[i] = (unsigned char)t;
            g_line.next = (unsigned short)(g_line.next - (unsigned short)(len - 1));
        }
    }
}

static void basic_squeeze_blanks(int i)
{
    int j;
    size_t move_len;
    if (g_line.text[i] != ' ') return;
    for (j = i + 1; j < g_line.next; j++) {
        if (g_line.text[j] != ' ')
            break;
    }
    if (j > i + 1) {
        move_len = strlen((char *)(g_line.text + j)) + 1;
        memmove((char *)(g_line.text + i + 1), (char *)(g_line.text + j), move_len);
        g_line.next = (unsigned short)(g_line.next - (unsigned short)(j - i - 1));
    }
}

static void basic_outline(FILE *out)
{
    int i;
    int len;
    char str = 0;

    if (!g_line.next) return;

    if (g_line.next == 1) {
        g_line.text[0] = 'R';
        g_line.text[1] = 'E';
        g_line.text[2] = 'M';
        g_line.text[3] = 0;
        g_line.next = 4;
    }

    for (i = 0; i < g_line.next; i++) {
        if (str && (g_line.text[i] != (unsigned char)str))
            continue;
        if (g_line.text[i] == (unsigned char)str) {
            str = 0;
        } else if (g_line.text[i] == 0x22) {
            str = 0x22;
        } else if (g_line.text[i] == 0x27) {
            str = 0x27;
        } else {
            basic_tokenize_at(i);
        }
        basic_squeeze_blanks(i);
    }

    len = 4 + g_line.next;
    g_lineaddr = (unsigned short)(g_lineaddr + (unsigned short)len);
    g_line.next = g_lineaddr;
    g_line.num = g_linenum++;
    fwrite(&g_line, 1, (size_t)len, out);
    g_line.next = 0;
}

static void basic_outbyte(FILE *out, char c)
{
    if (c == 0 && g_line.next > 0 && g_line.text[g_line.next - 1] == 0)
        return;
    g_line.text[g_line.next] = (unsigned char)c;
    if (++g_line.next == 254)
        basic_outline(out);
}

static int load_from_bas(const char *path, Payload *p)
{
    FILE *inp = fopen(path, "rb");
    FILE *tmp = NULL;
    int c;
    int num = 1;
    char str = 0;
    uint8_t *buf = NULL;
    size_t sz = 0;

    if (!inp) return -1;
    tmp = tmpfile();
    if (!tmp) { fclose(inp); return -1; }

    memset(&g_line, 0, sizeof(g_line));
    g_lineaddr = VZ_BASIC_START;
    g_linenum = 1;

    /*
     * Tokenization flow mirrors the existing text2bas behavior so packed
     * BASIC payloads stay compatible with current toolchain expectations.
     */
    while (!feof(inp)) {
        c = fgetc(inp);
        if (feof(inp)) break;

        if (str) {
            if (c == 0x0d || c == 0x0a) {
                if (c == 0x0d) {
                    int next = fgetc(inp);
                    if (next != 0x0a && next != EOF)
                        ungetc(next, inp);
                }
                basic_outbyte(tmp, 0);
                basic_outline(tmp);
                num = 1;
                str = 0;
            } else {
                basic_outbyte(tmp, (char)toupper((unsigned char)c));
            }
            if (c == str) str = 0;
        } else {
            if (num) {
                if (isdigit(c)) {
                    g_linenum = (unsigned short)(c - '0');
                    while (!feof(inp) && isdigit(c = fgetc(inp))) {
                        g_linenum = (unsigned short)(g_linenum * 10 + (unsigned short)(c - '0'));
                    }
                    if (c == ' ')
                        c = fgetc(inp);
                }
                num = 0;
            }
            switch (c) {
                case 0x0a:
                case 0x0d:
                case 0x1a:
                    if (c == 0x0d) {
                        int next = fgetc(inp);
                        if (next != 0x0a && next != EOF)
                            ungetc(next, inp);
                    }
                    basic_outbyte(tmp, 0);
                    basic_outline(tmp);
                    num = 1;
                    break;
                case 9:
                    basic_outbyte(tmp, 0x20);
                    while (g_line.next & 7)
                        basic_outbyte(tmp, 0x20);
                    break;
                case 0x22:
                case 0x27:
                    str = (char)c;
                    basic_outbyte(tmp, (char)c);
                    break;
                case ';':
                    str = 0x0d;
                    basic_outbyte(tmp, (char)c);
                    break;
                default:
                    basic_outbyte(tmp, (char)toupper((unsigned char)c));
                    break;
            }
        }
    }

    if (g_line.next > 0) {
        basic_outbyte(tmp, 0x00);
        basic_outline(tmp);
    }

    g_line.next = 0;
    fwrite(&g_line, 1, 2, tmp);

    if (fseek(tmp, 0, SEEK_END) != 0) { fclose(inp); fclose(tmp); return -1; }
    {
        long lsz = ftell(tmp);
        if (lsz < 0) { fclose(inp); fclose(tmp); return -1; }
        sz = (size_t)lsz;
    }
    if (fseek(tmp, 0, SEEK_SET) != 0) { fclose(inp); fclose(tmp); return -1; }

    buf = (uint8_t *)malloc(sz);
    if (!buf) { fclose(inp); fclose(tmp); return -1; }
    if (sz > 0 && fread(buf, 1, sz, tmp) != sz) {
        free(buf);
        fclose(inp);
        fclose(tmp);
        return -1;
    }

    fclose(inp);
    fclose(tmp);

    p->data = buf;
    p->size = sz;
    p->start = VZ_BASIC_START;
    p->start_set = 1;
    return 0;
}

static int write_vz_file(const char *out_path, const char name[VZ_NAME_LEN], VzType t, uint16_t start, const uint8_t *payload, size_t payload_len)
{
    FILE *f = fopen(out_path, "wb");
    uint8_t hdr[VZ_HEADER_SIZE];
    if (!f) return -1;

    memset(hdr, 0, sizeof(hdr));
    /* VZF0 for BASIC, VZFO for machine code, then flat payload bytes. */
    hdr[0] = 'V';
    hdr[1] = 'Z';
    hdr[2] = 'F';
    hdr[3] = (t == TYPE_BASIC) ? '0' : 'O';
    memcpy(hdr + 4, name, VZ_NAME_LEN);
    hdr[21] = (t == TYPE_BASIC) ? 0xF0u : 0xF1u;
    hdr[22] = (uint8_t)(start & 0xFFu);
    hdr[23] = (uint8_t)(start >> 8);

    if (fwrite(hdr, 1, sizeof(hdr), f) != sizeof(hdr)) { fclose(f); return -1; }
    if (payload_len > 0 && fwrite(payload, 1, payload_len, f) != payload_len) { fclose(f); return -1; }

    fclose(f);
    return 0;
}

int main(int argc, char **argv)
{
    InputKind in_kind = INPUT_NONE;
    const char *in_path = NULL;
    const char *out_path = NULL;
    const char *name_override = NULL;
    VzType forced_type = TYPE_AUTO;
    uint32_t start_override = 0;
    uint32_t fill_override = 0;
    int start_set = 0;
    int fill_set = 0;
    Payload p = {0};
    int i;
    char name[VZ_NAME_LEN];
    VzType final_type;
    uint16_t final_start;

    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    /*
     * Option order is free-form; only output path and one input source
     * are mandatory.
     */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-V") == 0) {
            printf("vzpack version %s\n", TOOL_VERSION);
            return 0;
        } else
        if ((strcmp(argv[i], "--in-bin") == 0 || strcmp(argv[i], "-b") == 0) && i + 1 < argc) {
            if (in_kind != INPUT_NONE) die("multiple input sources specified");
            in_kind = INPUT_BIN;
            in_path = argv[++i];
        } else if ((strcmp(argv[i], "--in-hex") == 0 || strcmp(argv[i], "-x") == 0) && i + 1 < argc) {
            if (in_kind != INPUT_NONE) die("multiple input sources specified");
            in_kind = INPUT_HEX;
            in_path = argv[++i];
        } else if ((strcmp(argv[i], "--in-srec") == 0 || strcmp(argv[i], "-r") == 0) && i + 1 < argc) {
            if (in_kind != INPUT_NONE) die("multiple input sources specified");
            in_kind = INPUT_SREC;
            in_path = argv[++i];
        } else if ((strcmp(argv[i], "--in-bas") == 0 || strcmp(argv[i], "-B") == 0) && i + 1 < argc) {
            if (in_kind != INPUT_NONE) die("multiple input sources specified");
            in_kind = INPUT_BAS;
            in_path = argv[++i];
        } else if ((strcmp(argv[i], "--out") == 0 || strcmp(argv[i], "-o") == 0) && i + 1 < argc) {
            out_path = argv[++i];
        } else if ((strcmp(argv[i], "--name") == 0 || strcmp(argv[i], "-n") == 0) && i + 1 < argc) {
            name_override = argv[++i];
        } else if ((strcmp(argv[i], "--type") == 0 || strcmp(argv[i], "-t") == 0) && i + 1 < argc) {
            const char *v = argv[++i];
            if (strcmp(v, "basic") == 0) forced_type = TYPE_BASIC;
            else if (strcmp(v, "mc") == 0 || strcmp(v, "machine") == 0) forced_type = TYPE_MC;
            else if (strcmp(v, "auto") == 0) forced_type = TYPE_AUTO;
            else die("invalid --type (use basic|mc|auto)");
        } else if ((strcmp(argv[i], "--start") == 0 || strcmp(argv[i], "-s") == 0) && i + 1 < argc) {
            if (parse_u32(argv[++i], &start_override) != 0 || start_override > 0xFFFFu)
                die("invalid --start value");
            start_set = 1;
        } else if ((strcmp(argv[i], "--fill") == 0 || strcmp(argv[i], "-f") == 0) && i + 1 < argc) {
            if (parse_u32(argv[++i], &fill_override) != 0 || fill_override > 0xFFu)
                die("invalid --fill value (expected 0..255)");
            fill_set = 1;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            return 0;
        } else {
            usage(argv[0]);
            return 1;
        }
    }

    if (!out_path) die("missing output path (use -o/--out)");
    if (in_kind == INPUT_NONE || !in_path) die("missing input source");

    if (in_kind == INPUT_BIN) {
        if (read_entire_file(in_path, &p.data, &p.size) != 0)
            die("failed reading binary input");
        if (!start_set)
            die("--start is required for --in-bin");
        p.start = (uint16_t)start_override;
        p.start_set = 1;
    } else if (in_kind == INPUT_HEX) {
        if (load_from_ihex(in_path, &p, fill_set, (uint8_t)fill_override) != 0)
            die("failed loading Intel HEX (invalid, sparse, or conflicting records)");
    } else if (in_kind == INPUT_SREC) {
        if (load_from_srec(in_path, &p, fill_set, (uint8_t)fill_override) != 0)
            die("failed loading S-record (invalid, sparse, or conflicting records)");
    } else if (in_kind == INPUT_BAS) {
        if (load_from_bas(in_path, &p) != 0)
            die("failed tokenizing BASIC input");
    } else {
        die("internal input selection error");
    }

    if (p.size > 65535u) {
        payload_free(&p);
        die("payload too large for VZ container");
    }

    if (start_set)
        p.start = (uint16_t)start_override;

    final_type = forced_type;
    if (final_type == TYPE_AUTO)
        final_type = (in_kind == INPUT_BAS) ? TYPE_BASIC : TYPE_MC;

    if (name_override)
        normalize_name(name, name_override);
    else
        normalize_name(name, in_path);

    final_start = p.start;

    if (write_vz_file(out_path, name, final_type, final_start, p.data, p.size) != 0) {
        payload_free(&p);
        die("failed writing output .vz");
    }

    printf("Packed %s -> %s\n", in_path, out_path);
    printf("Type      : %s\n", final_type == TYPE_BASIC ? "BASIC (0xF0)" : "Machine code (0xF1)");
    printf("Name      : %.16s\n", name);
    printf("Start     : 0x%04X\n", (unsigned)final_start);
    printf("Payload   : %lu bytes\n", (unsigned long)p.size);
    if ((in_kind == INPUT_HEX || in_kind == INPUT_SREC) && fill_set)
        printf("Fill byte : 0x%02X\n", (unsigned)(fill_override & 0xFFu));

    payload_free(&p);
    return 0;
}
