#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * Convert a VZEm format cassette image back to the byte
 * sequence that is written (and expected) by the Video Technology
 * machines type 1.
 */

struct vz {
	unsigned long vz_magic;
	char vz_name[17];
	unsigned char vz_type;
	unsigned short vz_start;
} __attribute__((__packed__));

struct cas {
	unsigned char preamble[128];
	unsigned char leadin[5];
	unsigned char filetype;
	char name[17];
	unsigned short startaddr;
	unsigned short endaddr;
	unsigned char data[1];
} __attribute__((__packed__));

int main(int ac, char **av)
{
	char input_name[256+1];
	FILE *input_file;
	char output_name[256+1];
	FILE *output_file;
	unsigned filesize, checksum;
	struct vz vz;
	struct cas *cas;
    char *p;
    int i;

    if( ac < 2 )
	{
		fprintf(stderr, "usage: vz2cas name[.vz] [output.cas]\n");
		return 1;
	}
	strcpy(input_name, av[1]);
	p = strrchr(input_name, '.');
	if( (p && strrchr(p, '/')) || !p )
		strcat(input_name, ".vz");
	if( ac > 2 )
		strcpy(output_name, av[2]);
	else
	{
		strcpy(output_name, input_name);
		p = strrchr(output_name, '.');
        if( (p && strrchr(p,'/')) || !p )
			strcat(p, ".cas");
		else
			strcpy(p, ".cas");
    }

	input_file = fopen(input_name, "rb");
	if( !input_file )
	{
		fprintf(stderr, "cannot open %s\n", input_name);
		return 1;
    }
	fseek(input_file, 0, SEEK_END);
	filesize = ftell(input_file) - sizeof(struct vz);
    fseek(input_file, 0, SEEK_SET);

	cas = malloc(filesize + sizeof(struct cas) + 1);
	if( !cas )
	{
        fprintf(stderr, "cannot malloc() VTC structure\n");
		return 1;
    }

	output_file = fopen(output_name, "wb");
	if( !output_file )
	{
		fprintf(stderr, "cannot create %s\n", output_name);
		return 1;
    }

	fread(&vz, 1, sizeof(struct vz), input_file);
	memset(cas->preamble, 0x80, sizeof(cas->preamble));
	memset(cas->leadin, 0xfe, sizeof(cas->leadin));
	cas->filetype = vz.vz_type;
	for( i = 0; i < 17; i++ )
	{
		cas->name[i] = vz.vz_name[i];
		if( cas->name[i] == 0 )
			break;
	}
	for( /* */; i < 17; i++ )
		cas->name[i] = 0;
    fread(cas->data, 1, filesize, input_file);

	cas->startaddr = vz.vz_start;
	cas->endaddr = vz.vz_start + filesize;
	checksum = cas->startaddr & 0xff;
	checksum += (cas->startaddr >> 8) & 0xff;
	checksum += cas->endaddr & 0xff;
	checksum += (cas->endaddr >> 8) & 0xff;
	for( i = 0; i < filesize; i++ )
		checksum += cas->data[i];
	checksum &= 0xffff;
	cas->data[i++] = checksum & 0xff;
	cas->data[i++] = (checksum >> 8) & 0xff;

	printf("Filetype     : %02X\n", cas->filetype);
	printf("Filename     : %s\n", cas->name);
	printf("Startaddress : %04X\n", cas->startaddr);
	printf("Endaddress   : %04X\n", cas->endaddr);
	printf("Checksum     : %04X\n", checksum);

	fwrite(cas, 1, sizeof(cas->preamble) + sizeof(cas->leadin) + sizeof(cas->filetype) + strlen(cas->name) + 1, output_file);
	fwrite(&cas->startaddr, 1, 2 * sizeof(unsigned short) + filesize + 2, output_file);
    fclose(input_file);
	fclose(output_file);
	free(cas);
	return 0;
}
