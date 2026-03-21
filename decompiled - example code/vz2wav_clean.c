/* Cleaned decompiled vz2wav.c - Application code only */
/* VZ200 Cassette to WAV converter */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Write a 0 bit */
word16 _wrBIT_0(struct Eq_22 * ds, Eq_3416 dwArg04, ptr16 & bxOut, ptr16 & diOut)
{
    ci16 si_13 = 0x00;
    word16 dx;
    ptr16 bx;
    ptr16 di;
    while (si_13 < 0x26)
    {
        <unknown> Eq_6186::* fp;
        word16 si_45;
        _fwrite(ds, fp - 3, 0x01, 0x01, dwArg04, out dx, out bx, out si_45, out di);
        si_13 = si_45 + 0x01;
    }
    bxOut = bx;
    diOut = di;
    return dx;
}


/* Write a 1 bit */
word16 _wrBIT_1(struct Eq_22 * ds, Eq_3416 dwArg04, ptr16 & bxOut, ptr16 & diOut)
{
    ci16 si_13 = 0x00;
    word16 dx;
    ptr16 bx;
    ptr16 di;
    while (si_13 < 0x26)
    {
        <unknown> Eq_6213::* fp;
        word16 si_45;
        _fwrite(ds, fp - 3, 0x01, 0x01, dwArg04, out dx, out bx, out si_45, out di);
        si_13 = si_45 + 0x01;
    }
    bxOut = bx;
    diOut = di;
    return dx;
}


/* Write a VZ200 byte as bit sequence */
word16 _wrVZbyte(struct Eq_22 * ds, Eq_3416 dwArg04, byte bArg08, ptr16 & dxOut, ptr16 & bxOut, ptr16 & diOut)
{
    word16 wArg06 = SLICE(dwArg04, word16, 16);
    byte bLoc03_103 = 0x80;
    ci16 si_14;
    ptr16 dx;
    ptr16 bx;
    ptr16 di;
    word16 cx;
    for (si_14 = 0x00; si_14 < 0x08; ++si_14)
    {
        if ((bArg08 & bLoc03_103) == 0x00)
            dx = _wrBIT_0(ds, dwArg04, out bx, out di);
        else
            dx = _wrBIT_1(ds, dwArg04, out bx, out di);
        cx = wArg06;
        bLoc03_103 = (byte) ((uint16) bLoc03_103 >> 0x01);
    }
    dxOut = dx;
    bxOut = bx;
    diOut = di;
    return cx;
}


/* Main application - converts VZ file to audio cassette format */
void _main(struct Eq_22 * ds, word16 wArg04, struct Eq_6269 * ptrArg06)
{
    word16 di_1547;
    word16 bx_1021;
    ui16 dx_1023;
    byte bLoc14;
    struct Eq_22 * ds_437;
    word16 bx_435;
    struct Eq_22 * cx_436;
    byte bLoc13;
    struct Eq_22 * ds_149;
    word16 bx_147;
    struct Eq_22 * cx_148;
    ui16 dx_101;
    word16 bx_19;
    
    /* Clear screen and print header */
    _printf(_clrscr(ds, out dx_101, out bx_19), dx_101, bx_19, ds, (char *) &ds->w0091 + 0x00CB);
    _printf(ds, dx_101, bx_19, ds, (char *) &ds->w0091 + 0x00D7);
    _printf(ds, dx_101, bx_19, ds, &ds->w0091 + 121);
    
    struct Eq_22 * cx_106 = ds;
    if (wArg04 != 0x03)
    {
        _printf(ds, dx_101, bx_19, ds, &ds->w0091 + 0x008C);
        dx_101 = _exit(ds, 0x01);
        cx_106 = 0x01;
    }
    
    /* Open input VZ file */
    _printf(cx_106, dx_101, bx_19, ds, (char *) &ds->w0091 + 323);
    Eq_3416 dx_ax_1507 = _fopen(ds, ptrArg06->ptr0004, (char *) &ds->w0091 + 345, out cx_148, out bx_147, out ds_149);
    ui16 dx_150 = SLICE(dx_ax_1507, word16, 16);
    word16 ax_146 = (word16) dx_ax_1507;
    
    if (dx_ax_1507 == 0x00)
    {
        _printf(cx_148, dx_150, bx_147, ds_149, &ds_149->w0091 + 0x00AE);
        dx_150 = _exit(ds_149, 0x01);
        cx_148 = 0x01;
    }
    
    /* Read VZ file header */
    _printf(cx_148, dx_150, bx_147, ds_149, &ds_149->t020A);
    <unknown> Eq_6333::* fp;
    word16 di_1509;
    _fread(ds_149, fp - 42, 0x18, 0x01, dx_ax_1507, out di_1509);
    
    /* Read VZ file data */
    byte ch_318;
    int16 di_276;
    while (true)
    {
        ch_318 = SLICE((uint16) _fread(ds_149, fp - 0x0C, 0x01, 0x01, dx_ax_1507, out di_276), byte, 8);
        if ((dx_ax_1507.u1->w0002 & 0x20) != 0x00)
            break;
        byte bLoc0C;
        ((char *) fp + 32726)[di_276] = bLoc0C;
    }
    
    /* Print file info */
    uint16 dx_299 = (uint16) bLoc13;
    struct Eq_22 * cx_319 = SEQ(ch_318, 0x08);
    _printf(cx_319, dx_299 << 0x08, ax_146, ds_149, (char *) &ds_149->t020A + 6);
    _printf(cx_319, dx_299 << 0x08, ax_146, ds_149, (char *) &ds_149->t020A + 26);
    _printf(cx_319, dx_299 << 0x08, ax_146, ds_149, (char *) &ds_149->t020A + 48);
    _printf(cx_319, dx_299 << 0x08, ax_146, ds_149, (char *) &ds_149->t020A + 70);
    _printf(cx_319, dx_299 << 0x08, ax_146, ds_149, (char *) &ds_149->t020A + 92);
    _printf(cx_319, dx_299 << 0x08, ax_146, ds_149, (char *) &ds_149->t020A + 113);
    
    /* Open output audio file */
    Eq_3416 dx_ax_1508 = _fopen(ds_149, ptrArg06->ptr0008, (char *) &ds_149->t020A + 0x0091, out cx_436, out bx_435, out ds_437);
    uint16 ax_305 = (uint16) bLoc14 + (dx_299 << 0x08) + di_276;
    ui16 dx_438 = SLICE(dx_ax_1508, word16, 16);
    byte bLoc12_1355 = (byte) ax_305;
    
    if (dx_ax_1508 == 0x00)
    {
        _printf(cx_436, dx_438, bx_435, ds_437, &ds_437->t029E);
        _exit(ds_437, 0x01);
    }
    
    /* Write WAV header (44 bytes) */
    cup16 si_480 = 0x00;
    while (si_480 < 44)
    {
        word16 dx_1510;
        word16 bx_1511;
        word16 si_512;
        word16 di_1512;
        _fwrite(ds_437, fp - 11, 0x01, 0x01, dx_ax_1508, out dx_1510, out bx_1511, out si_512, out di_1512);
        si_480 = si_512 + 0x01;
    }
    
    /* Write silence (22050 samples) */
    cup16 si_527 = 0x00;
    while (si_527 < 22050)
    {
        word16 dx_1513;
        word16 bx_1514;
        word16 si_557;
        word16 di_1515;
        _fwrite(ds_437, fp - 11, 0x01, 0x01, dx_ax_1508, out dx_1513, out bx_1514, out si_557, out di_1515);
        si_527 = si_557 + 0x01;
    }
    
    /* Write leader (0x80 bytes, 255 times) */
    cup16 si_571;
    for (si_571 = 0x00; si_571 < 0xFF; ++si_571)
    {
        word16 dx_1516;
        word16 bx_1517;
        word16 di_1518;
        _wrVZbyte(ds_437, dx_ax_1508, 0x80, out dx_1516, out bx_1517, out di_1518);
    }
    
    /* Write preamble (0xFE, 5 times) */
    cup16 si_608;
    for (si_608 = 0x00; si_608 < 0x05; ++si_608)
    {
        word16 dx_1519;
        word16 bx_1520;
        word16 di_1521;
        _wrVZbyte(ds_437, dx_ax_1508, ~0x01, out dx_1519, out bx_1520, out di_1521);
    }
    
    /* Write file type byte */
    byte bLoc15;
    word16 dx_1522;
    word16 bx_1523;
    word16 di_1524;
    _wrVZbyte(ds_437, dx_ax_1508, bLoc15, out dx_1522, out bx_1523, out di_1524);
    
    /* Write filename */
    Eq_6528 si_669;
    si_669.u0 = 0x00;
    struct Eq_6333 * ss;
    while (si_669 < 0x11)
    {
        byte al_679 = Mem678[ss:fp - 38 + si_669:byte];
        word16 dx_1525;
        word16 bx_1526;
        word16 di_1527;
        _wrVZbyte(ds_437, dx_ax_1508, al_679, out dx_1525, out bx_1526, out di_1527);
        if (al_679 == 0x00)
            break;
        si_669 = (word16) si_669 + 1;
    }
    
    /* Write null delay (80 samples) */
    cup16 si_710 = 0x00;
    while (si_710 < 0x50)
    {
        word16 dx_1528;
        word16 bx_1529;
        word16 si_742;
        word16 di_1530;
        _fwrite(ds_437, fp - 11, 0x01, 0x01, dx_ax_1508, out dx_1528, out bx_1529, out si_742, out di_1530);
        si_710 = si_742 + 0x01;
    }
    
    /* Write file data - start and end address bytes */
    word16 dx_1531;
    word16 bx_1532;
    word16 di_1533;
    _wrVZbyte(ds_437, dx_ax_1508, bLoc14, out dx_1531, out bx_1532, out di_1533);
    
    word16 dx_1534;
    word16 bx_1535;
    word16 di_1536;
    _wrVZbyte(ds_437, dx_ax_1508, bLoc13, out dx_1534, out bx_1535, out di_1536);
    
    word16 dx_1537;
    word16 bx_1538;
    word16 di_1539;
    _wrVZbyte(ds_437, dx_ax_1508, bLoc12_1355, out dx_1537, out bx_1538, out di_1539);
    
    byte al_852 = (byte) ((ax_305 & 0xFF00) >> 0x08);
    word16 dx_1540;
    word16 bx_1541;
    Eq_6606 di_873;
    _wrVZbyte(ds_437, dx_ax_1508, al_852, out dx_1540, out bx_1541, out di_873);
    
    /* Write file data bytes with checksum calculation */
    Eq_6613 wLoc0E_1374 = bLoc14 + bLoc13 + (uint16) bLoc12_1355 + (uint16) al_852;
    Eq_6606 si_882;
    si_882.u0 = 0x00;
    byte bLoc0E_1390;
    while (true)
    {
        bLoc0E_1390 = (byte) wLoc0E_1374;
        if (si_882 >= di_873)
            break;
        byte al_893 = Mem892[ss:fp + 32726 + si_882:byte];
        word16 dx_1542;
        word16 bx_1543;
        _wrVZbyte(ds_437, dx_ax_1508, al_893, out dx_1542, out bx_1543, out di_873);
        wLoc0E_1374 = (word16) wLoc0E_1374 + (uint16) al_893;
        si_882 = (word16) si_882 + 1;
    }
    
    /* Write checksum bytes */
    word16 dx_1544;
    word16 bx_1545;
    word16 di_1546;
    _wrVZbyte(ds_437, dx_ax_1508, bLoc0E_1390, out dx_1544, out bx_1545, out di_1546);
    
    struct Eq_22 * cx_1022 = _wrVZbyte(ds_437, dx_ax_1508, (byte) ((wLoc0E_1374 & 0xFF00) >> 0x08), out dx_1023, out bx_1021, out di_1547);
    
    /* Write trailing silence (22050 samples) */
    cup16 si_1032 = 0x00;
    while (si_1032 < 22050)
    {
        word16 si_1020;
        word16 di_1548;
        cx_1022 = _fwrite(ds_437, fp - 11, 0x01, 0x01, dx_ax_1508, out dx_1023, out bx_1021, out si_1020, out di_1548);
        si_1032 = si_1020 + 0x01;
    }
    
    /* Print completion message */
    _printf(cx_1022, dx_1023, bx_1021, ds_437, (char *) &ds_437->t029E + 25);
    _printf(ds_437, dx_1023, bx_1021, ds_437, (char *) &ds_437->t029E + 31);
    
    /* Close files */
    struct Eq_22 * ds_1082;
    _fclose(ds_437, dx_ax_1508, out ds_1082);
    selector ds_1549;
    _fclose(ds_1082, dx_ax_1507, out ds_1549);
}
