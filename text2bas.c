/* TEXT2BAS tokenizer, Linux version */
/* Converts ASCII BASIC to tokenized format for VZ200/300 or Colour Genie */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* set to 1 for Colour Genie tokenizer, 0 for VZ200/300 */
#ifndef CGENIE
#define CGENIE  0
#endif

typedef struct tagLINE {
    unsigned short next;
    unsigned short num;
    unsigned char text[254];
} BLABLA;

BLABLA line = {0, 0, {0, }};

unsigned short lineaddr = 0;
unsigned short linenum = 1;
int flag_tokenize = 1;
int flag_squeeze_blanks = 1;

#if CGENIE
#define EXT ".cas"
#define ADR 0x5801

static char *token[128+26+1] = {
    "END","FOR","RESET","SET","CLS","CMD","RANDOM","NEXT",
    "DATA","INPUT","DIM","READ","LET","GOTO","RUN","IF",
    "RESTORE","GOSUB","RETURN","REM","STOP","ELSE","TRON","TROFF",
    "DEFSTR","DEFINT","DEFSNG","DEFDBL","LINE","EDIT","ERROR","RESUME",
    "OUT","ON","OPEN","FIELD","GET","PUT","CLOSE","LOAD",
    "MERGE","NAME","KILL","LSET","RSET","SAVE","SYSTEM","LPRINT",
    "DEF","POKE","PRINT","CONT","LIST","LLIST","DELETE","AUTO",
    "CLEAR","CLOAD","CSAVE","NEW","TAB(","TO","FN", "USING",
    "VARPTR","USR","ERL","ERR","STRING$","INSTR","CHECK","TIME$",
    "MEM","INKEY$","THEN","NOT","STEP","+","-","*",
    "/","[","AND","OR",">","=","<","SGN",
    "INT","ABS","FRE","INP","POS","SQR","RND","LOG",
    "EXP","COS","SIN","TAN","ATN","PEEK","CVI","CVS",
    "CVD","EOF","LOC","LOF","MKI$","MKS$","MKD$","CINT",
    "CSNG","CDBL","FIX","LEN","STR$","VAL","ASC","CHR$",
    "LEFT$","RIGHT$","MID$","'","","","","",
    "COLOUR","FCOLOU","KEYPAD","JOY","PLOT","FGR","LGR","FCLS",
    "PLAY","CIRCLE","SCALE","SHAPE","NSHAPE","XSHAPE","PAINT","CPOINT",
    "NPLOT","SOUND","CHAR","RENUM","SWAP","FKEY","CALL","VERIFY",
    "BGRD","NBGRD", NULL};
#else
#define EXT ".vz"
#define ADR 0x7ae9

static char *token[128+1] = {
    "END","FOR","RESET","SET","CLS",""/* CMD */,"RANDOM","NEXT",
    "DATA","INPUT","DIM","READ","LET","GOTO","RUN","IF",
    "RESTORE","GOSUB","RETURN","REM","STOP","ELSE","COPY","COLOR",
    "VERIFY","DEFINT","DEFSNG","DEFDBL","CRUN","MODE","SOUND","RESUME",
    "OUT","ON","OPEN","FIELD","GET","PUT","CLOSE","LOAD",
    "MERGE","NAME","KILL","LSET","RSET","SAVE","SYSTEM","LPRINT",
    "DEF","POKE","PRINT","CONT","LIST","LLIST","DELETE","AUTO",
    "CLEAR","CLOAD","CSAVE","NEW","TAB(","TO","FN", "USING",
    "VARPTR","USR","ERL","ERR","STRING$","INSTR","POINT","TIME$",
    "MEM","INKEY$","THEN","NOT","STEP","+","-","*",
    "/","^","AND","OR",">","=","<","SGN",
    "INT","ABS","FRE","INP","POS","SQR","RND","LOG",
    "EXP","COS","SIN","TAN","ATN","PEEK","CVI","CVS",
    "CVD","EOF","LOC","LOF","MKI$","MKS$","MKD$","CINT",
    "CSNG","CDBL","FIX","LEN","STR$","VAL","ASC","CHR$",
    "LEFT$","RIGHT$","MID$","'","","","","",NULL};
#endif

void tokenize(int i)
{
    int t;
    int len;

    for (t = 0x80; token[t-0x80]; t++)
    {
        len = strlen(token[t-0x80]);
        if (len && strncasecmp((char*)(line.text+i), token[t-0x80], len) == 0)
        {
            if (t > 0xFF)
            {
                strcpy((char*)(line.text + i + 2), (char*)(line.text + i + len));
                line.text[i+0] = 0xFF;
                line.text[i+1] = t - 0x80;
                line.next -= len - 2;
            }
            else
            {
                strcpy((char*)(line.text + i + 1), (char*)(line.text + i + len));
                line.text[i+0] = t;
                line.next -= len - 1;
            }
        }
    }
}

void squeeze_blanks(int i)
{
    int j;

    if (line.text[i] != ' ')
        return;

    for (j = i+1; j < line.next; j++)
    {
        if (line.text[j] != ' ')
            break;
    }
    if (j > i+1)
    {
        strcpy((char*)(line.text + i + 1), (char*)(line.text + j));
        line.next -= j - i - 1;
    }
}

void outline(FILE * out)
{
    int i;
    int len;
    char str = 0;

    if (line.next)
    {
        if (line.next == 1)  /* avoid empty lines */
        {
            line.text[0] = 'R';
            line.text[1] = 'E';
            line.text[2] = 'M';
            line.text[3] = 0;
            line.next = 4;
        }
        for (i = 0; i < line.next; i++)
        {
            if (str && (line.text[i] != str))
                continue;
            if (line.text[i] == str)
            {
                str = 0;
            }
            else if (line.text[i] == 0x22)
            {
                str = 0x22;
            }
            else if (line.text[i] == 0x27)
            {
                str = 0x27;
            }
            else if (flag_tokenize)
                tokenize(i);
            if (flag_squeeze_blanks)
                squeeze_blanks(i);
        }

        len = 4 + line.next;
        lineaddr += len;
        line.next = lineaddr;
        line.num = linenum++;
        fwrite(&line, 1, len, out);
        line.next = 0;
    }
}

void outbyte(FILE * out, char c)
{
    if (c == 0 && line.next > 0 && line.text[line.next-1] == 0)
        return;

    line.text[line.next] = c;

    if (++line.next == 254)
        outline(out);
}

void print_usage(const char *progname)
{
    printf("Usage: %s <input.bas> [output%s] [options]\n", progname, EXT);
    printf("\nOptions:\n");
    printf("  -n, --no-tokenize       Don't tokenize BASIC keywords\n");
    printf("  -s, --no-squeeze        Don't squeeze multiple blanks\n");
    printf("  -h, --help              Show this help message\n");
    printf("\nConverts ASCII BASIC to tokenized format for %s\n", 
#if CGENIE
           "Colour Genie"
#else
           "VZ200/300"
#endif
    );
}

int main(int ac, char **av)
{
    char inpfilename[256];
    char outfilename[256];
    char *p;
    FILE *inp, *out;
    int num = 1;
    char str = 0;
    char c;
    int arg_idx = 1;

    memset(inpfilename, 0, sizeof(inpfilename));
    memset(outfilename, 0, sizeof(outfilename));

    /* Parse command line arguments */
    while (arg_idx < ac)
    {
        if (strcmp(av[arg_idx], "-h") == 0 || strcmp(av[arg_idx], "--help") == 0)
        {
            print_usage(av[0]);
            return 0;
        }
        else if (strcmp(av[arg_idx], "-n") == 0 || strcmp(av[arg_idx], "--no-tokenize") == 0)
        {
            flag_tokenize = 0;
            arg_idx++;
        }
        else if (strcmp(av[arg_idx], "-s") == 0 || strcmp(av[arg_idx], "--no-squeeze") == 0)
        {
            flag_squeeze_blanks = 0;
            arg_idx++;
        }
        else if (inpfilename[0] == 0)
        {
            strcpy(inpfilename, av[arg_idx]);
            arg_idx++;
        }
        else if (outfilename[0] == 0)
        {
            strcpy(outfilename, av[arg_idx]);
            arg_idx++;
        }
        else
        {
            fprintf(stderr, "Unknown argument: %s\n", av[arg_idx]);
            print_usage(av[0]);
            return 1;
        }
    }

    if (inpfilename[0] == 0)
    {
        fprintf(stderr, "Error: No input file specified\n\n");
        print_usage(av[0]);
        return 1;
    }

    /* Generate output filename if not specified */
    if (outfilename[0] == 0)
    {
        strcpy(outfilename, inpfilename);
        p = strrchr(outfilename, '.');
        if (p)
            strcpy(p, EXT);
        else
            strcat(outfilename, EXT);
    }

    /* Ensure output has correct extension */
    p = strrchr(outfilename, '.');
    if (!p)
        strcat(outfilename, EXT);

    /* Open input file */
    inp = fopen(inpfilename, "rb");
    if (!inp)
    {
        fprintf(stderr, "Error: Could not open input file '%s'\n", inpfilename);
        return 1;
    }

    /* Open output file */
    out = fopen(outfilename, "wb");
    if (!out)
    {
        fprintf(stderr, "Error: Could not create output file '%s'\n", outfilename);
        fclose(inp);
        return 1;
    }

#if CGENIE
    /* Colour Genie header */
    p = strrchr(inpfilename, '/');
    if (!p)
        p = inpfilename - 1;
    fputc(0x66, out);
    /* Basic name is first char of filename */
    fputc(toupper(p[1]), out);
#else
    /* VZ magic header */
    fwrite("VZF0", 1, 4, out);
    /* write input filename (basename only, uppercase, null-padded to 17 bytes) */
    p = strrchr(inpfilename, '/');
    if (p)
        p++;
    else
        p = inpfilename;
    
    char basename[18];
    memset(basename, 0, sizeof(basename));
    strncpy(basename, p, 16);  /* Leave room for null terminator */
    basename[16] = '\0';  /* Ensure null termination */
    
    /* Convert to uppercase and truncate at extension */
    for (int i = 0; i < 17 && basename[i]; i++)
    {
        if (basename[i] == '.')
        {
            basename[i] = 0;
            break;
        }
        basename[i] = toupper(basename[i]);
    }
    fwrite(basename, 1, 17, out);
    
    /* VZ magic value 0xf0 */
    fputc(0xf0, out);
    /* VZ basic load address */
    fputc(ADR & 0xff, out);
    fputc(ADR >> 8, out);
#endif

    lineaddr = ADR;
    
    /* Process input file */
    while (!feof(inp))
    {
        c = fgetc(inp);
        if (feof(inp))
            break;
            
        if (str)  /* inside string/comment ? */
        {
            if (c == 0x0d || c == 0x0a)  /* new line ? (handle both CR and LF) */
            {
                /* Skip LF if it follows CR */
                if (c == 0x0d)
                {
                    int next = fgetc(inp);
                    if (next != 0x0a && next != EOF)
                        ungetc(next, inp);
                }
                outbyte(out, 0x00);
                outline(out);
                num = 1;
                str = 0;  /* End string/comment at line end */
            }
            else
            {
#if CGENIE
                /* put any char */
                outbyte(out, c);
#else
                /* upper case letters only */
                outbyte(out, toupper(c));
#endif
            }
            if (c == str)  /* end of string/comment ? */
                str = 0;
        }
        else  /* normal text */
        {
            if (num)  /* expecting line number ? */
            {
                if (isdigit(c))  /* digit follows ?? */
                {
                    linenum = c - '0';
                    while (!feof(inp) && isdigit(c = fgetc(inp)))
                    {
                        linenum *= 10;
                        linenum += c - '0';
                    }
                    if (c == ' ')  /* skip one space after the line number */
                        c = fgetc(inp);
                }
                num = 0;
            }
            switch (c)
            {
            case 0x0a:  /* LF - Unix line ending */
            case 0x0d:  /* CR - Mac/DOS line ending */
            case 0x1a:  /* End of text marker */
                /* Skip LF if it follows CR (handle CRLF) */
                if (c == 0x0d)
                {
                    int next = fgetc(inp);
                    if (next != 0x0a && next != EOF)
                        ungetc(next, inp);
                }
                outbyte(out, 0x00);
                outline(out);
                num = 1;
                break;
            case 9:  /* expand tabs */
                outbyte(out, 0x20);
                while (line.next & 7)
                    outbyte(out, 0x20);
                break;
            case 0x22:  /* inside string ? */
            case 0x27:
                str = c;
                outbyte(out, c);
                break;
            case ';':  /* comment line ? */
                str = 0x0d;  /* skip until next return */
                outbyte(out, c);
                break;
            default:
#if CGENIE
                outbyte(out, c);
#else
                outbyte(out, toupper(c));
#endif
                break;
            }
        }
    }

    /* Write end marker */
    line.next = 0;
    fwrite(&line, 1, 2, out);

    fclose(inp);
    fclose(out);

    printf("Successfully converted '%s' to '%s'\n", inpfilename, outfilename);
    printf("Tokenization: %s\n", flag_tokenize ? "enabled" : "disabled");
    printf("Blank squeezing: %s\n", flag_squeeze_blanks ? "enabled" : "disabled");

    return 0;
}
