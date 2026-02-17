void __cleanup()
{
    selector seg0800;
    fn0800_01EA(&Eq_10::t05F2, &Eq_10::t05F2, seg0800->ptr0260);
}


void __checknull()
{
}


void __terminate(Eq_16 bArg04)
{
    msdos_terminate(bArg04);
}


word16 __restorezero(struct Eq_22 * ds)
{
    msdos_set_interrupt_vector(0x00, ds->ptr005B);
    msdos_set_interrupt_vector(0x04, ds->ptr005F);
    msdos_set_interrupt_vector(0x05, ds->ptr0063);
    <anonymous> * ds_dx_36 = ds->ptr0067;
    msdos_set_interrupt_vector(0x06, ds_dx_36);
    return (word16) ds_dx_36;
}


void fn0800_01EA(struct Eq_8 Eq_10::* si, struct Eq_8 Eq_10::* di, struct Eq_10 * es)
{
    while (true)
    {
        cu8 ah_100;
        if (si != &Eq_10::t05E0)
            ah_100 = 0x00;
        else
            ah_100 = ~0x00;
        struct Eq_8 Eq_10::* dx_14 = di;
        struct Eq_8 Eq_10::* bx_16;
        for (bx_16 = si; bx_16 != di; ++bx_16)
        {
            if ((es->*bx_16).b0000 != ~0x00)
            {
                bool v18_102;
                if (si != &Eq_10::t05E0)
                    v18_102 = ah_100 > (es->*bx_16).b0001;
                else
                    v18_102 = (es->*bx_16).b0001 > ah_100;
                if (!v18_102)
                {
                    ah_100 = (es->*bx_16).b0001;
                    dx_14 = bx_16;
                }
            }
        }
        if (dx_14 == di)
            break;
        (es->*dx_14).b0000 = ~0x00;
        if ((es->*dx_14).b0000 != 0x00)
            (es->*dx_14).t0002.u0();
        else
        {
            selector seg0800;
            (seg0800->*((es->*dx_14).t0002).u1)();
        }
    }
}


word16 fn0800_02CF(struct Eq_22 * ds, Eq_16 wArg02, word16 wArg04, word16 wArg06)
{
    mp16 fp;
    if (wArg06 == 0x00)
    {
        while (ds->w02DA != 0x00)
        {
            --ds->w02DA;
            ui16 bx_24 = ds->w02DA;
            (*((char *) &ds->t05D8 + 26))[bx_24]();
        }
        __cleanup();
        ds->t02DC();
    }
    word16 dx_56 = __restorezero(ds);
    __checknull();
    mp16 sp_68 = fp - 4;
    if (wArg04 != 0x00)
        return dx_56;
    if (wArg06 == 0x00)
    {
        ds->t02E0();
        ds->ptr02E4();
    }
    union Eq_16 Eq_145::* sp_89 = sp_68 - 2;
    selector ss;
    (ss->*sp_89).u1 = (word16) wArg02;
    __terminate((ss->*sp_89).u0);
}


word16 _exit(struct Eq_22 * ds, Eq_16 wArg04)
{
    return fn0800_02CF(ds, wArg04, 0x00, 0x00);
}


ui32 fn0800_0361(Eq_194 ax, cu8 cl, cui16 dx)
{
    if (cl >= 0x10)
        return SEQ(ax << cl - 0x10, 0x00);
    return SEQ(dx << cl | ax >> -cl + 0x10, ax << cl);
}


Eq_194 fn0800_0382(uint32 cx_bx, Eq_194 ax, Eq_194 dx, ptr16 & cxOut, union Eq_216 & dxOut, union Eq_217 & bxOut)
{
    ci16 cx = SLICE(cx_bx, word16, 16);
    Eq_217 bx = (word16) cx_bx;
    byte dh_33 = SLICE(dx, byte, 8);
    byte dl_44 = (byte) dx;
    byte cl_29 = (byte) cx;
    if (cx >= 0x00)
    {
        cu16 ax_22 = ax + bx;
        byte al_38 = (byte) ax_22;
        if (ax_22 < 0x00)
        {
            dh_33 = SLICE((word32) dx + 0x00001000, byte, 8);
            dl_44 = (byte) dx;
        }
        cxOut = SEQ(al_38, 0x04);
        dxOut = SEQ(dh_33 + (cl_29 << 0x04), dl_44) + (ax_22 >> 0x04);
        bxOut = bx;
        return SEQ(SLICE(ax_22 >> 0x04, byte, 8), al_38) & 0x0F;
    }
    else
    {
        ui32 cx_bx_92 = ~cx_bx;
        word16 bx_58 = (word16) cx_bx_92 + 0x01;
        byte cl_69 = SLICE(cx_bx_92 + 0x01, byte, 16);
        cu16 ax_63 = ax - bx_58;
        byte al_78 = (byte) ax_63;
        if (ax_63 < 0x00)
            dx -= 0x1000;
        cxOut = SEQ(al_78, 0x04);
        Eq_217 bx_75 = SEQ(cl_69 << 0x04, 0x00);
        dxOut = dx - bx_75 + (ax_63 >> 0x04);
        bxOut = bx_75;
        return SEQ(SLICE(ax_63 >> 0x04, byte, 8), al_78) & 0x0F;
    }
}


Eq_194 fn0800_0382(uint32 cx_bx, Eq_194 ax, Eq_194 dx, ptr16 & cxOut, union Eq_216 & dxOut, union Eq_217 & bxOut)
{
    ci16 cx = SLICE(cx_bx, word16, 16);
    Eq_217 bx = (word16) cx_bx;
    byte dh_33 = SLICE(dx, byte, 8);
    byte dl_44 = (byte) dx;
    byte cl_29 = (byte) cx;
    if (cx >= 0x00)
    {
        cu16 ax_22 = ax + bx;
        byte al_38 = (byte) ax_22;
        if (ax_22 < 0x00)
        {
            dh_33 = SLICE((word32) dx + 0x00001000, byte, 8);
            dl_44 = (byte) dx;
        }
        cxOut = SEQ(al_38, 0x04);
        dxOut = SEQ(dh_33 + (cl_29 << 0x04), dl_44) + (ax_22 >> 0x04);
        bxOut = bx;
        return SEQ(SLICE(ax_22 >> 0x04, byte, 8), al_38) & 0x0F;
    }
    else
    {
        ui32 cx_bx_92 = ~cx_bx;
        word16 bx_58 = (word16) cx_bx_92 + 0x01;
        byte cl_69 = SLICE(cx_bx_92 + 0x01, byte, 16);
        cu16 ax_63 = ax - bx_58;
        byte al_78 = (byte) ax_63;
        if (ax_63 < 0x00)
            dx -= 0x1000;
        cxOut = SEQ(al_78, 0x04);
        Eq_217 bx_75 = SEQ(cl_69 << 0x04, 0x00);
        dxOut = dx - bx_75 + (ax_63 >> 0x04);
        bxOut = bx_75;
        return SEQ(SLICE(ax_63 >> 0x04, byte, 8), al_78) & 0x0F;
    }
}


word16 __IOERROR(struct Eq_22 * ds, Eq_194 wArg02)
{
    Eq_194 si_13 = wArg02;
    int16 si_19;
    if (wArg02 >= 0x00)
    {
        if (wArg02 <= 88)
            goto l0800_03F5;
    }
    else
    {
        si_19 = -wArg02;
        if (si_19 <= 0x23)
        {
            ds->t04A6.u0 = ~0x00;
l0800_040F:
            ds->w007F = si_19;
            return ~0x00;
        }
    }
    si_13.u0 = 0x57;
l0800_03F5:
    ds->t04A6 = si_13;
    si_19 = (int16) (ds->*((word32) si_13 + 1192));
    goto l0800_040F;
}


cui16 _isatty(int16 wArg04)
{
    cui16 dx_11;
    msdos_ioctl_get_device_info(wArg04, out dx_11);
    return dx_11 & 0x80;
}


void __LONGTOA(Eq_328 bArg02, Eq_329 bArg04, Eq_330 wArg06, Eq_331 ptrArg08, Eq_332 dwArg0C)
{
    Eq_356 fp;
    uint16 wArg0C = (word16) dwArg0C;
    Eq_335 wArg0E = SLICE(dwArg0C, word16, 16);
    struct Eq_337 * es_36 = SLICE(ptrArg08, selector, 16);
    byte Eq_337::* di_124 = (word16) ptrArg08;
    cu8 bl_23 = (byte) wArg06;
    if (wArg06 > 0x24 || bl_23 < 0x02)
    {
l0800_04AE:
        es_36->*di_124 = 0x00;
        return;
    }
    uint16 ax_26 = wArg0C;
    Eq_335 cx_28 = wArg0E;
    if (wArg0E < 0x00 && bArg04 != 0x00)
    {
        *ptrArg08.u1 = 0x2D;
        ui32 cx_ax_166 = -dwArg0C;
        ++di_124;
        ax_26 = (word16) cx_ax_166;
        cx_28 = SLICE(cx_ax_166, word16, 16);
    }
    byte Eq_378::* si_107 = fp - 36;
    selector ss;
    if (cx_28 != 0x00)
    {
        Eq_335 ax_58;
        do
        {
            uint32 dx_ax_54 = (uint32) cx_28;
            uint32 dx_ax_62 = SEQ(dx_ax_54 % wArg06, ax_26);
            ss->*si_107 = (byte) (dx_ax_62 % wArg06);
            ax_58 = dx_ax_54 /u wArg06;
            cx_28 = ax_58;
            ax_26 = dx_ax_62 /u wArg06;
            ++si_107;
        } while (ax_58 != 0x00);
l0800_0490:
        if (ax_26 == 0x00)
        {
            int16 cx_104 = (int16) (si_107 + (36 - fp));
            do
            {
                --si_107;
                cu8 al_111 = ss->*si_107;
                byte al_116;
                if (al_111 < 0x0A)
                    al_116 = al_111 + 0x30;
                else
                    al_116 = (byte) bArg02.u0 + (al_111 - 0x0A);
                es_36->*di_124 = al_116;
                ++di_124;
                --cx_104;
            } while (cx_104 != 0x00);
            goto l0800_04AE;
        }
    }
    uint32 dx_ax_82 = (uint32) ax_26;
    ss->*si_107 = (byte) (dx_ax_82 % wArg06);
    ax_26 = dx_ax_82 /u wArg06;
    ++si_107;
    goto l0800_0490;
}


void __UTOA(Eq_331 dwArg02, word16 wArg06)
{
    __LONGTOA(0x61, 0x00, 0x0A, dwArg02, (uint32) wArg06);
}


Eq_194 _lseek(struct Eq_22 * ds, Eq_194 wArg04, int32 * dwArg06, byte bArg0A, ptr16 & cxOut, ptr16 & dxOut)
{
    int32 * dx_ax_22;
    ptr16 wArg08 = SLICE(dwArg06, word16, 16);
    (*((char *) &ds->t0478 + 2))[wArg04] &= ~0x0200;
    Eq_194 ax_25 = (word16) dx_ax_22;
    ptr16 dx_51 = SLICE(dx_ax_22, word16, 16);
    if (msdos_set_file_position(wArg04, dwArg06, bArg0A, out dx_ax_22))
    {
        int32 * dx_ax_33 = (int32 *) __IOERROR(ds, ax_25);
        ax_25 = (word16) dx_ax_33;
        dx_51 = SLICE(dx_ax_33, word16, 16);
    }
    cxOut = wArg08;
    dxOut = dx_51;
    return ax_25;
}


Eq_484 __MKNAME(struct Eq_22 * ds, word16 wArg02, cui16 wArg04, cui16 wArg06, Eq_484 dwArg08)
{
    Eq_484 dwArg08_144 = dwArg08;
    if (dwArg08 == 0x00)
        dwArg08_144.u1 = (char *) &ds->t05D8 + 0x009A;
    Eq_194 dx_ax_147;
    if ((wArg04 | wArg06) == 0x00)
        dx_ax_147.u4 = (char *) &ds->t04A6 + 92;
    else
        dx_ax_147 = SEQ(wArg06, wArg04);
    __UTOA(__stpcpy(dwArg08_144, dx_ax_147), wArg02);
    _strcat(dwArg08_144, (char *) &ds->t04A6 + 96);
    return dwArg08_144;
}


uint16 fn0800_05BD(uint16 ax, Eq_518 cx, Eq_518 dx, Eq_194 bx, union Eq_518 & cxOut, ptr16 & dxOut)
{
    Eq_518 ax_15 = dx;
    if (dx != 0x00)
        ax_15 = bx *16 dx;
    if (cx != 0x00)
    {
        cx = ax_15;
        ax_15 = (word16) ax_15.u0 + ax * cx;
    }
    cxOut = cx;
    uint32 dx_ax_43 = bx *32 ax;
    dxOut = (word16) ax_15.u0 + SLICE(dx_ax_43, word16, 16);
    return (word16) dx_ax_43;
}


Eq_536 fn0800_05D4(Eq_194 ax, cup16 cx, Eq_194 dx, cu16 bx)
{
    return <invalid>;
}


Eq_194 __read(struct Eq_22 * ds, Eq_194 wArg04, Eq_194 ptrArg06, Eq_194 wArg0A, union Eq_554 & chOut)
{
    Eq_194 wLoc04_53;
    Eq_194 ax_24;
    if (((*((char *) &ds->t0478 + 2))[wArg04] & 0x02) != 0x00)
        wLoc04_53.u0 = 0x05;
    else
    {
        if (!msdos_read_file(wArg04, wArg0A, ptrArg06, out ax_24))
            goto l0800_0622;
        wLoc04_53 = ax_24;
    }
    ax_24 = __IOERROR(ds, wLoc04_53);
l0800_0622:
    chOut.u1 = <invalid>;
    return ax_24;
}


void _unlink(struct Eq_22 * ds, char * ptrArg04)
{
    Eq_194 ax_16;
    if (msdos_delete_file(ptrArg04, out ax_16))
        __IOERROR(ds, ax_16);
}


word16 fn0800_070A(byte * es_di, byte dh, byte dl)
{
    struct Eq_593 * es = SLICE(es_di, selector, 16);
    word16 di_10 = fn0800_0711(es_di, dh);
    return fn0800_0711(di_10, dl);
}


word16 fn0800_0711(byte * es_di, byte al)
{
    struct Eq_605 * es = SLICE(es_di, selector, 16);
    word16 ax_5 = __aam(al);
    byte al_6 = (byte) ax_5;
    word16 di_15 = fn0800_071A(es_di, SLICE(ax_5, byte, 8));
    return fn0800_071A(di_15, al_6);
}


word16 fn0800_071A(byte * es_di, byte al)
{
    word16 di = (word16) es_di;
    *es_di = al + 0xD0 + (byte) __daa(al + 0x90, &(al + 0x90));
    return di + 1;
}


void __VPRINTER(struct Eq_22 * cx, ui16 dx, word16 bx, struct Eq_22 * ds, mp16 wArg02, Eq_649 ptrArg04)
{
    byte ah_1230;
    byte ah_1224;
    ci16 bx_821;
    byte bLoc16_1534;
    word16 bx_791;
    word16 cx_787;
    word16 cx_619;
    Eq_705 wLoc10_1520;
    byte Eq_1458::* di_576;
    cui16 wLoc04_1482;
    struct Eq_1113 * es_181;
    byte Eq_1113::* di_179;
    byte Eq_1113::* di_566;
    word16 cx_523;
    word16 dx_522;
    byte * es_di_2104;
    ptr32 es_di_2111;
    <unknown> Eq_22::* di_319;
    word16 sp_1284;
    word16 di_389;
    byte Eq_662::* si_97;
    Eq_328 bx_2101;
    word16 bx_2107;
    word16 bx_2114;
    cui16 wLoc04_1538;
    Eq_937 wLoc0C_1352;
    word16 ax_962;
    cu8 ch_845;
    byte Eq_662::* si_35;
    segptr32 ptrArg04_1326;
    word16 wLoc06_1325;
    Eq_652 fp;
    byte bLoc16_1808 = 0x50;
    mp16 sp_20 = fp - 0x9E;
    mp16 wLoc06_1324 = fp - 0x98;
l0800_078F:
    word16 di_29 = wLoc06_1325;
l0800_0792:
    struct Eq_816 * psegArg06_1805 = SLICE(ptrArg04_1326, selector, 16);
    struct Eq_662 * es_1125 = SLICE(ptrArg04_1326, selector, 16);
    word16 si_1127 = (word16) ptrArg04_1326;
    ci8 bLoc16_1583;
    while (true)
    {
        byte al_37 = es_1125->*si_35;
        byte Eq_662::* si_38 = si_35 + 1;
        byte al_39 = al_37;
        if (al_37 == 0x00)
            break;
        byte Eq_22::* di_53;
        struct Eq_22 * ss;
        if (al_37 == 0x25)
        {
            byte al_49 = es_1125->*si_38;
            byte Eq_662::* si_50 = si_38 + 1;
            if (al_49 == 0x25)
                goto l0800_079F;
            byte Eq_22::* wLoc06_1328 = di_53;
            byte ch_1220 = 0x00;
            word16 wLoc10_1329 = 0x00;
            word16 wLoc04_1330 = 0x20;
            byte bLoc0D_1331 = 0x00;
            word16 wLoc0A_1332 = ~0x00;
            word16 wLoc0C_1333 = ~0x00;
l0800_07D7:
            Eq_705 ax_67;
            byte al_66;
            ax_67.u1 = (uint16) al_66;
            cu8 bl_70 = (byte) ax_67;
            byte bh_75 = SLICE(ax_67, byte, 8);
            byte ah_65 = 0x00;
            Eq_705 dx_68 = ax_67;
            byte dl_140 = (byte) ax_67;
            struct Eq_758 Eq_22::* bx_76 = SEQ(bh_75, bl_70 - 0x20);
            word16 cx_1248 = SEQ(ch_845, 0x00);
            if (bl_70 < 0x80)
            {
                byte bl_78 = (ds->*bx_76).b0515;
                cu16 bx_79 = SEQ(bh_75, bl_78);
                word16 cx_1245 = SEQ(ch_845, 0x00);
                if (bx_79 > 0x17)
                    goto l0800_0B7C;
                ui16 bx_83 = bx_79 << 0x01;
                word16 cx_1246 = SEQ(ch_845, 0x00);
                switch (bx_79)
                {
                case 0x00:
                    word16 cx_1251 = SEQ(ch_845, 0x00);
                    if (ch_845 <= 0x00)
                    {
                        byte bLoc0D_1419;
                        if (bLoc0D_1419 != 0x2B)
                        {
                            byte bLoc0D_1554 = dl_140;
                        }
                        goto l0800_07D5;
                    }
                    break;
                case 0x01:
                    word16 cx_1250 = SEQ(ch_845, 0x00);
                    cui16 wLoc04_1334;
                    if (ch_845 <= 0x00)
                    {
                        cui16 wLoc04_1553 = wLoc04_1334 | 0x01;
                        goto l0800_07D5;
                    }
                    break;
                case 0x02:
                    Eq_841 ax_941;
                    struct Eq_837 Eq_22::* wArg02_1338;
                    ax_941.u0 = (ss->*wArg02_1338).t0000.u0;
                    byte ah_1227 = SLICE(ax_941, byte, 8);
                    union Eq_841 Eq_22::* wArg02_1549 = (union Eq_841 Eq_22::*) &wArg02_1338->w0002;
                    if (ch_845 >= 0x02)
                    {
                        word16 cx_1254 = SEQ(ch_845, 0x00);
                        if (ch_845 != 0x04)
                            goto l0800_0843;
                        Eq_841 wLoc0C_1550 = ax_941;
                        cu8 ch_949 = ch_845 + 0x01;
                        goto l0800_07D5;
                    }
                    else
                    {
                        Eq_841 ax_951 = ax_941;
                        if (ax_941 < 0x00)
                        {
                            ci16 ax_956 = -ax_941;
                            cui16 wLoc04_1551 = wLoc04_1334 | 0x02;
                        }
                        word16 wLoc0A_1552 = ax_962;
                        byte ch_966 = 0x03;
                        goto l0800_0864;
                    }
                case 0x03:
                    word16 cx_1249 = SEQ(ch_845, 0x00);
                    if (ch_845 <= 0x00)
                    {
                        cui16 wLoc04_1548 = wLoc04_1334 | 0x02;
                        goto l0800_07D5;
                    }
                    break;
                case 0x04:
                    word16 cx_1253 = SEQ(ch_845, 0x00);
                    if (ch_845 >= 0x04)
                        goto l0800_0843;
                    byte ch_929 = 0x04;
                    wchar_t wLoc0C_1547 = (word16) wLoc0C_1352 + 1;
                    goto l0800_07D5;
                case 0x05:
                    goto l0800_0881;
                case 0x06:
                    cui16 wLoc04_1540 = wLoc04_1334 | 0x10;
                    goto l0800_082B;
                case 0x07:
                    cui16 wLoc04_1537 = wLoc04_1334 | 0x0100;
                    goto l0800_08CB;
                case 0x08:
l0800_08CB:
                    cui16 wLoc04_1539 = wLoc04_1538 & ~0x10;
                    goto l0800_082B;
                case 0x09:
                    int16 wLoc0A_1406;
                    if (ch_845 <= 0x00)
                    {
                        if ((wLoc04_1334 & 0x02) == 0x00)
                        {
                            cui16 wLoc04_1536 = wLoc04_1334 | 0x08;
                            byte ch_857 = 0x01;
                            goto l0800_07D5;
                        }
                    }
                    else
                    {
l0800_0881:
                        int16 ax_884 = (int16) ((byte) ax_67 - 0x30);
                        byte ah_1228 = SLICE(ax_884, byte, 8);
                        if (ch_845 <= 0x02)
                        {
                            byte ch_909 = 0x02;
                            int16 wLoc0A_1545 = ax_884;
                            if (wLoc0A_1406 >= 0x00)
                            {
                                int16 wLoc0A_1546 = ax_884 + wLoc0A_1406 * 0x0A;
                                goto l0800_07D5;
                            }
                        }
                        else
                        {
                            word16 cx_1255 = SEQ(ch_845, 0x00);
                            if (ch_845 != 0x04)
                            {
l0800_0843:
                                goto l0800_0B7C;
                            }
                            int16 wLoc0C_1542 = ax_884;
                            if (wLoc0C_1352 != 0x00)
                            {
                                int16 wLoc0C_1543 = ax_884 + wLoc0C_1352 * 0x0A;
                                goto l0800_07D5;
                            }
                        }
                    }
l0800_0864:
                    goto l0800_07D5;
                case 0x0A:
                    int32 dx_ax_371 = (int32) (ss->*wArg02_1338).t0000.u0;
                    byte bLoc08_1376 = 0x01;
                    word16 ax_396 = (word16) dx_ax_371;
                    word16 dx_404 = SLICE(dx_ax_371, word16, 16);
                    word16 bx_2100 = SEQ(0x0A, bl_78 << 0x01);
                    goto l0800_0904;
                case 11:
                    word16 bx_2112 = SEQ(0x08, bl_78 << 0x01);
                    goto l0800_08E0;
                case 0x0C:
                    word16 bx_2106 = SEQ(0x0A, bl_78 << 0x01);
                    goto l0800_08E4;
                case 0x0D:
                    word16 bx_2113 = SEQ(0x10, dl_140 + 233);
l0800_08E0:
                    byte bLoc0D_1371 = 0x00;
                    word16 bx_2105 = bx_2114;
l0800_08E4:
                    word16 dx_358 = 0x00;
                    byte bLoc08_1373 = 0x00;
                    Eq_841 ax_365;
                    ax_365.u0 = (ss->*wArg02_1338).t0000.u0;
                    word16 bx_2099 = bx_2107;
l0800_0904:
                    struct Eq_1090 Eq_22::* di_373 = (struct Eq_1090 Eq_22::*) ((char *) &wArg02_1338->t0000 + 1);
                    byte bh_443 = SLICE(bx_2101, byte, 8);
                    union Eq_841 Eq_22::* di_374 = (union Eq_841 Eq_22::*) &di_373->w0001;
                    segptr32 ptrArg04_1638 = &(psegArg06_1805->*si_97);
                    if ((wLoc04_1334 & 0x10) != 0x00)
                    {
                        word16 dx_385 = (ss->*di_373).w0001;
                        struct Eq_1143 Eq_22::* di_387 = (struct Eq_1143 Eq_22::*) (&di_373->w0001 + 1);
                    }
                    word16 wArg02_1382 = di_389;
                    Eq_332 ax_395;
                    struct Eq_22 * dx_402;
                    if (ax_395 == 0x00)
                    {
                        struct Eq_22 * dx_405 = dx_402;
                        if (dx_402 != 0x00)
                            goto l0800_092C;
                        Eq_328 bx_1262 = bx_2101;
                        if (wLoc0C_1352 == 0x00)
                            goto l0800_078F;
                    }
                    else
                    {
l0800_092C:
                        cui16 wLoc04_1385 = wLoc04_1334 | 0x04;
                    }
                    struct Eq_1157 Eq_22::* sp_426 = sp_1284 - 2;
                    (ss->*sp_426).ptr0000 = dx_402;
                    (ss->*sp_426).tFFFFFFFE.u1 = (word16) ax_395;
                    (ss->*sp_426).ptrFFFFFFFC = ss;
                    (ss->*sp_426).tFFFFFFFA.u0 = (mp16) (fp - 71);
                    Eq_330 ax_448;
                    ax_448.u0 = (int16) bh_443;
                    (ss->*sp_426).tFFFFFFF8.u0 = (int16) ax_448;
                    byte bLoc08_1392;
                    (ss->*sp_426).tFFFFFFF6.u1 = (word16) SEQ(SLICE(ax_448, byte, 8), bLoc08_1392);
                    (ss->*sp_426).tFFFFFFF4.u1 = (word16) bx_2101;
                    __LONGTOA((ss->*sp_426).tFFFFFFF4.u0, (ss->*sp_426).tFFFFFFF6.u0, (ss->*sp_426).tFFFFFFF8.u0, (ss->*sp_426).tFFFFFFFA.u1, (ss->*sp_426).tFFFFFFFE.u0);
                    (ss->*sp_426).ptr0000 = ss;
                    struct Eq_22 * es_479 = (ss->*sp_426).ptr0000;
                    struct Eq_1226 Eq_22::* sp_480 = (struct Eq_1226 Eq_22::*) ((char *) &sp_426->ptr0000 + 2);
                    Eq_937 dx_482 = wLoc0C_1352;
                    if (wLoc0C_1352 >= 0x00)
                    {
                        ptr32 es_di_2103 = (ptr32) (fp - 71);
                        goto l0800_0A4E;
                    }
                    else
                    {
                        ptr32 es_di_2108 = (ptr32) (fp - 71);
                        goto l0800_0A40;
                    }
                case 0x0E:
                    struct Eq_914 Eq_22::* sp_281 = sp_1284 - 2;
                    (ss->*sp_281).t0000.u0 = (ss->*wArg02_1338).t0000.u0;
                    mp16 di_278 = fp - 72;
                    union Eq_841 Eq_22::* wArg02_1365 = (union Eq_841 Eq_22::*) &wArg02_1338->w0002;
                    segptr32 ptrArg04_1664 = &(psegArg06_1805->*si_97);
                    if ((wLoc04_1334 & 0x20) != 0x00)
                    {
                        word16 dx_290 = (ss->*wArg02_1338).w0002;
                        (ss->*sp_281).ptrFFFFFFFE = ss;
                        struct Eq_22 * es_297 = (ss->*sp_281).ptrFFFFFFFE;
                        byte Eq_22::* di_303 = fn0800_070A(fp - 72, SLICE(dx_290, byte, 8), (byte) dx_290);
                        es_297->*di_303 = 0x3A;
                        struct Eq_1645 Eq_22::* wArg02_1366 = (struct Eq_1645 Eq_22::*) (&wArg02_1338->w0002 + 1);
                        byte Eq_22::* di_310 = di_303 + 1;
                    }
                    (ss->*sp_281).ptrFFFFFFFE = ss;
                    Eq_841 dx_317;
                    dx_317.u0 = (ss->*sp_281).t0000.u0;
                    struct Eq_22 * es_315 = (ss->*sp_281).ptrFFFFFFFE;
                    byte Eq_22::* di_324 = fn0800_070A(di_319, SLICE(dx_317, byte, 8), (byte) dx_317);
                    ss->*di_324 = 0x00;
                    struct Eq_1608 Eq_22::* sp_318 = (struct Eq_1608 Eq_22::*) ((char *) &sp_281->t0000 + 2);
                    cui16 wLoc04_1369 = wLoc04_1334 & ~0x04;
                    Eq_937 di_336 = di_324 - (fp - 72);
                    Eq_937 dx_340 = wLoc0C_1352;
                    if (wLoc0C_1352 <= di_336)
                    {
                        Eq_937 dx_343 = di_336;
                    }
                    ptr32 es_di_2110 = (ptr32) (fp - 72);
                    goto l0800_0A40;
                case 0x0F:
                    Eq_937 cx_220 = wLoc0C_1352;
                    segptr32 ptrArg04_1661 = &(psegArg06_1805->*si_97);
                    if (wLoc0C_1352 < 0x00)
                    {
                        word16 cx_226 = 0x06;
                    }
                    struct Eq_1270 Eq_22::* sp_228 = sp_1284 - 2;
                    (ss->*sp_228).ptr0000 = ss;
                    (ss->*sp_228).ptrFFFFFFFE = wArg02_1338;
                    word16 cx_235;
                    (ss->*sp_228).wFFFFFFFC = cx_235;
                    (ss->*sp_228).ptrFFFFFFFA = ss;
                    (ss->*sp_228).ptrFFFFFFF8 = fp - 71;
                    (ss->*sp_228).tFFFFFFF6.u0 = (ci16) ax_67;
                    (ss->*sp_228).wFFFFFFF4 = wLoc04_1334 & 0x01;
                    if ((wLoc04_1334 & 0x0100) != 0x00)
                    {
                        word16 ax_259 = 0x08;
                        union Eq_841 Eq_22::* wArg02_1362 = (union Eq_841 Eq_22::*) (&wArg02_1338->w0002 + 4);
                    }
                    else
                    {
                        union Eq_841 Eq_22::* wArg02_1361 = (union Eq_841 Eq_22::*) (&wArg02_1338->w0002 + 3);
                        word16 ax_258 = 0x06;
                    }
                    word16 ax_265;
                    (ss->*sp_228).wFFFFFFF2 = ax_265;
                    __REALCVT(ds);
                    (ss->*sp_228).ptrFFFFFFF0 = ss;
                    struct Eq_1441 Eq_22::* sp_273 = sp_228 - 0x0E;
                    ptr32 es_di_2109 = (ptr32) (fp - 71);
l0800_0A40:
                    struct Eq_1251 * es_508 = SLICE(es_di_2111, selector, 16);
                    word16 di_505 = (word16) es_di_2111;
                    cui16 wLoc04_1396;
                    if ((wLoc04_1396 & 0x08) != 0x00)
                    {
                        int16 dx_499 = wLoc0A_1406;
                        ptr32 es_di_2102 = es_di_2111;
                        if (wLoc0A_1406 > 0x00)
                        {
l0800_0A4E:
                            struct Eq_1113 * es_507 = SLICE(es_di_2104, selector, 16);
                            byte Eq_1113::* di_504 = (word16) es_di_2104;
                            Eq_937 cx_511 = fn0800_073C(di_504, es_507);
                            if (*es_di_2104 == 0x2D)
                            {
                                word16 cx_519 = cx_511 - 0x01;
                            }
                            ci16 dx_524 = dx_522 - cx_523;
                            if (dx_524 > 0x00)
                            {
                                ci16 wLoc10_1412 = dx_524;
                            }
                        }
                    }
                    struct Eq_1113 * es_532;
                    byte Eq_1113::* di_534;
                    if (es_532->*di_534 != 0x2D)
                    {
                        byte bLoc0D_1413;
                        if (bLoc0D_1413 == 0x00)
                            goto l0800_0A80;
                        byte Eq_1113::* di_548 = di_534 - 0x01;
                        es_532->*di_548 = bLoc0D_1413;
                    }
                    ci16 wLoc10_1428;
                    if (wLoc10_1428 > 0x00 && wLoc0C_1352 < 0x00)
                    {
                        ci16 wLoc10_1451 = wLoc10_1428 - 0x01;
                    }
l0800_0A80:
                    Eq_937 cx_571 = fn0800_073C(di_566, es_532);
                    goto l0800_0A83;
                case 0x10:
                    struct Eq_22 * Eq_22::* sp_203 = sp_1284 - 2;
                    ss->*sp_203 = (struct Eq_22 * Eq_22::*) ss;
                    union Eq_841 Eq_22::* wArg02_1355 = (union Eq_841 Eq_22::*) &wArg02_1338->w0002;
                    struct Eq_22 * es_205 = (struct Eq_22 *) (ss->*sp_203);
                    struct Eq_22 * Eq_22::* sp_206 = (struct Eq_22 * Eq_22::*) ((char *) sp_203 + 2);
                    mp16 di_207 = fp - 71;
                    word16 cx_215 = 0x01;
                    segptr32 ptrArg04_1649 = &(psegArg06_1805->*si_97);
                    goto l0800_0A83;
                case 0x11:
                    segptr32 ptrArg04_1653 = &(psegArg06_1805->*si_97);
                    if ((wLoc04_1334 & 0x20) == 0x00)
                    {
                        Eq_841 di_157;
                        di_157.u0 = (ss->*wArg02_1338).t0000.u0;
                        struct Eq_22 * Eq_22::* sp_160 = sp_1284 - 2;
                        ss->*sp_160 = (struct Eq_22 * Eq_22::*) ds;
                        union Eq_841 Eq_22::* wArg02_1349 = (union Eq_841 Eq_22::*) &wArg02_1338->w0002;
                        struct Eq_22 * es_162 = (struct Eq_22 *) (ss->*sp_160);
                        Eq_841 di_164 = di_157;
                        bool v74_1315 = di_157 != 0x00;
                    }
                    else
                    {
                        Eq_841 es_di_147;
                        es_di_147.u2 = (ds->*wArg02_1338).t0000.u2;
                        struct Eq_1054 * es_150 = SLICE(es_di_147, selector, 16);
                        word16 di_152 = (word16) es_di_147;
                        union Eq_841 Eq_22::* wArg02_1348 = (union Eq_841 Eq_22::*) (&wArg02_1338->w0002 + 1);
                        bool v75_1316 = es_di_147 != 0x00;
                    }
                    bool v73_1314;
                    if (!v73_1314)
                    {
                        struct Eq_22 * Eq_22::* sp_172 = sp_1284 - 2;
                        ss->*sp_172 = (struct Eq_22 * Eq_22::*) ds;
                        struct Eq_22 * es_176 = (struct Eq_22 *) (ss->*sp_172);
                        word16 di_178 = 0x050E;
                    }
                    word16 sp_1296 = sp_1284;
                    Eq_937 cx_184 = fn0800_073C(di_179, es_181);
                    if (cx_184 > wLoc0C_1352)
                    {
                        Eq_937 cx_196 = wLoc0C_1352;
                    }
l0800_0A83:
                    cui16 ax_591 = wLoc04_1482 & 0x05;
                    byte Eq_1458::* si_578 = di_576;
                    byte Eq_22::* di_588 = di_53;
                    int16 bx_589 = wLoc0A_1406;
                    if (ax_591 == 0x05)
                    {
                        ci16 wLoc10_1512;
                        if (dl_140 != 111)
                        {
                            if (dl_140 == 0x78 || dl_140 == 88)
                            {
                                ci16 v45_610 = wLoc10_1512 - 0x02;
                                cui16 wLoc04_1510 = wLoc04_1482 | 0x40;
                                ci16 bx_609 = wLoc0A_1406 - 0x02;
                                ci16 wLoc10_1517 = v45_610;
                                if (v45_610 < 0x00)
                                {
                                    word16 wLoc10_1518 = 0x00;
                                }
                            }
l0800_0AC6:
                            ci16 cx_629 = (word16) wLoc10_1520.u0 + cx_619;
                            cui16 wLoc04_1522;
                            Eq_705 dx_679;
                            if ((wLoc04_1522 & 0x02) == 0x00)
                            {
                                ci16 bx_634;
                                while (bx_634 > cx_629)
                                {
                                    byte Eq_22::* di_644;
                                    word16 di_647;
                                    fn0800_0749(fp - 2, 0x20, cx_629, dx_679, bx_634, di_644, out di_647);
                                    ci16 bx_651 = bx_634 - 0x01;
                                }
                            }
                            ci16 bx_673;
                            if ((wLoc04_1522 & 0x40) != 0x00)
                            {
                                byte Eq_22::* di_692;
                                word16 di_701;
                                fn0800_0749(fp - 2, dl_140, cx_629, dx_679, bx_673, di_692, out di_701);
                            }
                            Eq_705 dx_709 = wLoc10_1520;
                            selector es_721;
                            if (wLoc10_1520 > 0x00)
                            {
                                ci16 cx_715 = cx_629 - wLoc10_1520;
                                ci16 bx_717 = bx_673 - wLoc10_1520;
                                byte al_732 = es_721->*di_576;
                                if (al_732 == 0x2D || (al_732 == 0x20 || al_732 == 0x2B))
                                {
                                    byte Eq_1458::* si_743 = di_576 + 1;
                                    byte Eq_22::* di_752;
                                    word16 di_757;
                                    fn0800_0749(fp - 2, es_721->*di_576, cx_715, wLoc10_1520, bx_717, di_752, out di_757);
                                    ci16 cx_761 = cx_715 - 0x01;
                                    word16 bx_762 = bx_717 - 0x01;
                                }
                                Eq_705 cx_768 = wLoc10_1520;
                                Eq_705 cx_766;
                                if (wLoc10_1520 != 0x00)
                                {
                                    word16 cx_784;
                                    do
                                    {
                                        ci16 cx_777;
                                        ci16 bx_793;
                                        byte Eq_22::* di_775;
                                        word16 di_780;
                                        fn0800_0749(fp - 2, 0x30, cx_777, cx_766, bx_793, di_775, out di_780);
                                        cx_784 = cx_777 - 0x01;
                                    } while (cx_784 != 0x00);
                                }
                                word16 dx_788 = cx_787;
                                Eq_705 cx_789 = cx_766;
                            }
                            word16 cx_790;
                            Eq_705 dx_1210;
                            if (cx_790 != 0x00)
                            {
                                ci16 bx_794 = bx_791 - cx_790;
                                word16 cx_820;
                                do
                                {
                                    byte Eq_22::* di_802;
                                    byte Eq_1458::* si_797;
                                    ss->*di_802 = es_721->*si_797;
                                    ci8 v50_806 = bLoc16_1534 - 0x01;
                                    byte Eq_1458::* si_800 = si_797 + 1;
                                    byte Eq_22::* di_804 = di_802 + 1;
                                    ci8 bLoc16_1535 = v50_806;
                                    ci16 cx_811;
                                    if (v50_806 <= 0x00)
                                    {
                                        word16 di_814;
                                        fn0800_0752(fp - 2, cx_811, dx_1210, bx_794, di_804, out di_814);
                                    }
                                    cx_820 = cx_811 - 0x01;
                                } while (cx_820 != 0x00);
                            }
                            ci16 bx_823 = bx_821;
                            if (bx_821 > 0x00)
                            {
                                ci16 cx_828 = bx_821;
                                word16 cx_844;
                                do
                                {
                                    ci16 cx_837;
                                    byte Eq_22::* di_835;
                                    word16 di_840;
                                    fn0800_0749(fp - 2, 0x20, cx_837, dx_1210, bx_821, di_835, out di_840);
                                    cx_844 = cx_837 - 0x01;
                                } while (cx_844 != 0x00);
                            }
                            goto l0800_0792;
                        }
                        if (wLoc10_1512 <= 0x00)
                        {
                            word16 wLoc10_1519 = 0x01;
                        }
                    }
                    goto l0800_0AC6;
                case 0x12:
                    segptr32 ptrArg04_1641 = &(psegArg06_1805->*si_97);
                    if ((wLoc04_1334 & 0x20) == 0x00)
                    {
                        Eq_841 di_108;
                        di_108.u0 = (ss->*wArg02_1338).t0000.u0;
                        struct Eq_22 * Eq_22::* sp_113 = sp_1284 - 2;
                        ss->*sp_113 = (struct Eq_22 * Eq_22::*) ds;
                        union Eq_841 Eq_22::* wArg02_1340 = (union Eq_841 Eq_22::*) &wArg02_1338->w0002;
                        struct Eq_22 * es_115 = (struct Eq_22 *) (ss->*sp_113);
                    }
                    else
                    {
                        Eq_841 es_di_104;
                        es_di_104.u2 = (ds->*wArg02_1338).t0000.u2;
                        struct Eq_1008 * es_127 = SLICE(es_di_104, selector, 16);
                        word16 di_129 = (word16) es_di_104;
                        union Eq_841 Eq_22::* wArg02_1339 = (union Eq_841 Eq_22::*) (&wArg02_1338->w0002 + 1);
                    }
                    selector es_126;
                    struct Eq_1039 Eq_1038::* di_128;
                    (es_126->*di_128).w0000 = (uint16) (0x50 - bLoc16_1583);
                    word16 sp_1287 = sp_1284;
                    if ((wLoc04_1334 & 0x10) != 0x00)
                        (es_126->*di_128).w0002 = 0x00;
                    goto l0800_078F;
                case 0x13:
                case 0x14:
                case 0x15:
                    goto l0800_0B7C;
                case 22:
                    cui16 wLoc04_1336 = wLoc04_1334 & ~0x20;
                    goto l0800_082B;
                case 0x17:
                    cui16 wLoc04_1335 = wLoc04_1334 | 0x20;
l0800_082B:
                    byte ch_876 = 0x05;
l0800_07D5:
                    byte al_998 = es_1125->*si_97;
                    byte Eq_662::* si_999 = si_97 + 1;
                    goto l0800_07D7;
                }
            }
l0800_0B7C:
            Eq_705 dx_1256 = ax_67;
            byte Eq_662::* si_1009 = si_38;
            byte Eq_22::* di_1011 = di_53;
            word16 ax_1229 = SEQ(ah_1224, 0x25);
            byte al_1029;
            do
            {
                word16 ax_1231 = SEQ(ah_1230, al_1029);
                word16 ax_1013;
                ci16 cx_1236;
                ci16 bx_1232;
                byte Eq_22::* di_1017;
                word16 di_1022;
                ah_1230 = SLICE((uint16) fn0800_0749(fp - 2, (byte) ax_1013, cx_1236, ax_67, bx_1232, di_1017, out di_1022), byte, 8);
                byte Eq_816::* si_1027;
                al_1029 = psegArg06_1805->*si_1027;
                byte Eq_816::* si_1030 = si_1027 + 1;
            } while (al_1029 != 0x00);
            break;
        }
l0800_079F:
        byte al_1082;
        ss->*di_53 = al_1082;
        ci8 v18_1088 = bLoc16_1583 - 0x01;
        byte Eq_22::* di_1086 = di_53 + 1;
        ci8 bLoc16_1629 = v18_1088;
        if (v18_1088 <= 0x00)
        {
            ci16 cx_1266;
            Eq_705 dx_1277;
            ci16 bx_1259;
            word16 di_1096;
            fn0800_0752(fp - 2, cx_1266, dx_1277, bx_1259, di_1086, out di_1096);
        }
    }
    if (bLoc16_1583 < 0x50)
    {
        ci16 cx_1044;
        Eq_705 dx_1046;
        ci16 bx_1042;
        byte Eq_22::* di_1048;
        word16 di_2115;
        fn0800_0752(fp - 2, cx_1044, dx_1046, bx_1042, di_1048, out di_2115);
    }
}


word16 fn0800_073C(byte Eq_1113::* di, struct Eq_1113 * es)
{
    word16 cx_13 = ~0x00;
    while (cx_13 != 0x00)
    {
        byte Eq_1113::* di_46;
        di = di_46 + 1;
        --cx_13;
        di_46 = di;
        if (es->*di_46 != 0x00)
            return -cx_13;
    }
    return -cx_13;
}


byte fn0800_0749(struct Eq_703 * ss_bp, byte al, ci16 cx, Eq_705 dx, ci16 bx, byte Eq_22::* di, union Eq_803 & diOut)
{
    Eq_803 di_19;
    word16 ax;
    al = (byte) ax;
    struct Eq_22 * ss = SLICE(ss_bp, selector, 16);
    ss->*di = al;
    Eq_1756 v9_10 = ss_bp->tFFFFFFEC.u0 - 0x01;
    ss_bp->tFFFFFFEC.u0 = (byte) v9_10;
    if (v9_10 != 0x00)
    {
        diOut = di + 1;
        return SLICE(ax, byte, 8);
    }
    else
    {
        uint16 ax_18 = (uint16) fn0800_0752(ss_bp, cx, dx, bx, di + 1, out di_19);
        diOut = di_19;
        return SLICE(ax_18, byte, 8);
    }
}


byte fn0800_0752(struct Eq_703 * ss_bp, ci16 cx, Eq_705 dx, ci16 bx, byte Eq_22::* di, ptr16 & diOut)
{
    word16 bp = (word16) ss_bp;
    word16 di_20 = di - (bp - 0x96);
    selector seg0800;
    (seg0800->*ss_bp->ptr000E)();
    word16 ax_39;
    if (ax_39 == 0x00)
        ss_bp->wFFFFFFEA = 0x01;
    ss_bp->tFFFFFFEC.u1 = 0x50;
    ss_bp->wFFFFFFEE += di_20;
    diOut = bp - 0x96;
    return SLICE(ax_39, byte, 8);
}


void __VPRINTER(struct Eq_22 * cx, ui16 dx, word16 bx, struct Eq_22 * ds, mp16 wArg02, Eq_649 ptrArg04)
{
    byte ah_1230;
    byte ah_1224;
    ci16 bx_821;
    byte bLoc16_1534;
    word16 bx_791;
    word16 cx_787;
    word16 cx_619;
    Eq_705 wLoc10_1520;
    byte Eq_1458::* di_576;
    cui16 wLoc04_1482;
    struct Eq_1113 * es_181;
    byte Eq_1113::* di_179;
    byte Eq_1113::* di_566;
    word16 cx_523;
    word16 dx_522;
    byte * es_di_2104;
    ptr32 es_di_2111;
    <unknown> Eq_22::* di_319;
    word16 sp_1284;
    word16 di_389;
    byte Eq_662::* si_97;
    Eq_328 bx_2101;
    word16 bx_2107;
    word16 bx_2114;
    cui16 wLoc04_1538;
    Eq_937 wLoc0C_1352;
    word16 ax_962;
    cu8 ch_845;
    byte Eq_662::* si_35;
    segptr32 ptrArg04_1326;
    word16 wLoc06_1325;
    Eq_652 fp;
    byte bLoc16_1808 = 0x50;
    mp16 sp_20 = fp - 0x9E;
    mp16 wLoc06_1324 = fp - 0x98;
l0800_078F:
    word16 di_29 = wLoc06_1325;
l0800_0792:
    struct Eq_816 * psegArg06_1805 = SLICE(ptrArg04_1326, selector, 16);
    struct Eq_662 * es_1125 = SLICE(ptrArg04_1326, selector, 16);
    word16 si_1127 = (word16) ptrArg04_1326;
    ci8 bLoc16_1583;
    while (true)
    {
        byte al_37 = es_1125->*si_35;
        byte Eq_662::* si_38 = si_35 + 1;
        byte al_39 = al_37;
        if (al_37 == 0x00)
            break;
        byte Eq_22::* di_53;
        struct Eq_22 * ss;
        if (al_37 == 0x25)
        {
            byte al_49 = es_1125->*si_38;
            byte Eq_662::* si_50 = si_38 + 1;
            if (al_49 == 0x25)
                goto l0800_079F;
            byte Eq_22::* wLoc06_1328 = di_53;
            byte ch_1220 = 0x00;
            word16 wLoc10_1329 = 0x00;
            word16 wLoc04_1330 = 0x20;
            byte bLoc0D_1331 = 0x00;
            word16 wLoc0A_1332 = ~0x00;
            word16 wLoc0C_1333 = ~0x00;
l0800_07D7:
            Eq_705 ax_67;
            byte al_66;
            ax_67.u1 = (uint16) al_66;
            cu8 bl_70 = (byte) ax_67;
            byte bh_75 = SLICE(ax_67, byte, 8);
            byte ah_65 = 0x00;
            Eq_705 dx_68 = ax_67;
            byte dl_140 = (byte) ax_67;
            struct Eq_758 Eq_22::* bx_76 = SEQ(bh_75, bl_70 - 0x20);
            word16 cx_1248 = SEQ(ch_845, 0x00);
            if (bl_70 < 0x80)
            {
                byte bl_78 = (ds->*bx_76).b0515;
                cu16 bx_79 = SEQ(bh_75, bl_78);
                word16 cx_1245 = SEQ(ch_845, 0x00);
                if (bx_79 > 0x17)
                    goto l0800_0B7C;
                ui16 bx_83 = bx_79 << 0x01;
                word16 cx_1246 = SEQ(ch_845, 0x00);
                switch (bx_79)
                {
                case 0x00:
                    word16 cx_1251 = SEQ(ch_845, 0x00);
                    if (ch_845 <= 0x00)
                    {
                        byte bLoc0D_1419;
                        if (bLoc0D_1419 != 0x2B)
                        {
                            byte bLoc0D_1554 = dl_140;
                        }
                        goto l0800_07D5;
                    }
                    break;
                case 0x01:
                    word16 cx_1250 = SEQ(ch_845, 0x00);
                    cui16 wLoc04_1334;
                    if (ch_845 <= 0x00)
                    {
                        cui16 wLoc04_1553 = wLoc04_1334 | 0x01;
                        goto l0800_07D5;
                    }
                    break;
                case 0x02:
                    Eq_841 ax_941;
                    struct Eq_837 Eq_22::* wArg02_1338;
                    ax_941.u0 = (ss->*wArg02_1338).t0000.u0;
                    byte ah_1227 = SLICE(ax_941, byte, 8);
                    union Eq_841 Eq_22::* wArg02_1549 = (union Eq_841 Eq_22::*) &wArg02_1338->w0002;
                    if (ch_845 >= 0x02)
                    {
                        word16 cx_1254 = SEQ(ch_845, 0x00);
                        if (ch_845 != 0x04)
                            goto l0800_0843;
                        Eq_841 wLoc0C_1550 = ax_941;
                        cu8 ch_949 = ch_845 + 0x01;
                        goto l0800_07D5;
                    }
                    else
                    {
                        Eq_841 ax_951 = ax_941;
                        if (ax_941 < 0x00)
                        {
                            ci16 ax_956 = -ax_941;
                            cui16 wLoc04_1551 = wLoc04_1334 | 0x02;
                        }
                        word16 wLoc0A_1552 = ax_962;
                        byte ch_966 = 0x03;
                        goto l0800_0864;
                    }
                case 0x03:
                    word16 cx_1249 = SEQ(ch_845, 0x00);
                    if (ch_845 <= 0x00)
                    {
                        cui16 wLoc04_1548 = wLoc04_1334 | 0x02;
                        goto l0800_07D5;
                    }
                    break;
                case 0x04:
                    word16 cx_1253 = SEQ(ch_845, 0x00);
                    if (ch_845 >= 0x04)
                        goto l0800_0843;
                    byte ch_929 = 0x04;
                    wchar_t wLoc0C_1547 = (word16) wLoc0C_1352 + 1;
                    goto l0800_07D5;
                case 0x05:
                    goto l0800_0881;
                case 0x06:
                    cui16 wLoc04_1540 = wLoc04_1334 | 0x10;
                    goto l0800_082B;
                case 0x07:
                    cui16 wLoc04_1537 = wLoc04_1334 | 0x0100;
                    goto l0800_08CB;
                case 0x08:
l0800_08CB:
                    cui16 wLoc04_1539 = wLoc04_1538 & ~0x10;
                    goto l0800_082B;
                case 0x09:
                    int16 wLoc0A_1406;
                    if (ch_845 <= 0x00)
                    {
                        if ((wLoc04_1334 & 0x02) == 0x00)
                        {
                            cui16 wLoc04_1536 = wLoc04_1334 | 0x08;
                            byte ch_857 = 0x01;
                            goto l0800_07D5;
                        }
                    }
                    else
                    {
l0800_0881:
                        int16 ax_884 = (int16) ((byte) ax_67 - 0x30);
                        byte ah_1228 = SLICE(ax_884, byte, 8);
                        if (ch_845 <= 0x02)
                        {
                            byte ch_909 = 0x02;
                            int16 wLoc0A_1545 = ax_884;
                            if (wLoc0A_1406 >= 0x00)
                            {
                                int16 wLoc0A_1546 = ax_884 + wLoc0A_1406 * 0x0A;
                                goto l0800_07D5;
                            }
                        }
                        else
                        {
                            word16 cx_1255 = SEQ(ch_845, 0x00);
                            if (ch_845 != 0x04)
                            {
l0800_0843:
                                goto l0800_0B7C;
                            }
                            int16 wLoc0C_1542 = ax_884;
                            if (wLoc0C_1352 != 0x00)
                            {
                                int16 wLoc0C_1543 = ax_884 + wLoc0C_1352 * 0x0A;
                                goto l0800_07D5;
                            }
                        }
                    }
l0800_0864:
                    goto l0800_07D5;
                case 0x0A:
                    int32 dx_ax_371 = (int32) (ss->*wArg02_1338).t0000.u0;
                    byte bLoc08_1376 = 0x01;
                    word16 ax_396 = (word16) dx_ax_371;
                    word16 dx_404 = SLICE(dx_ax_371, word16, 16);
                    word16 bx_2100 = SEQ(0x0A, bl_78 << 0x01);
                    goto l0800_0904;
                case 11:
                    word16 bx_2112 = SEQ(0x08, bl_78 << 0x01);
                    goto l0800_08E0;
                case 0x0C:
                    word16 bx_2106 = SEQ(0x0A, bl_78 << 0x01);
                    goto l0800_08E4;
                case 0x0D:
                    word16 bx_2113 = SEQ(0x10, dl_140 + 233);
l0800_08E0:
                    byte bLoc0D_1371 = 0x00;
                    word16 bx_2105 = bx_2114;
l0800_08E4:
                    word16 dx_358 = 0x00;
                    byte bLoc08_1373 = 0x00;
                    Eq_841 ax_365;
                    ax_365.u0 = (ss->*wArg02_1338).t0000.u0;
                    word16 bx_2099 = bx_2107;
l0800_0904:
                    struct Eq_1090 Eq_22::* di_373 = (struct Eq_1090 Eq_22::*) ((char *) &wArg02_1338->t0000 + 1);
                    byte bh_443 = SLICE(bx_2101, byte, 8);
                    union Eq_841 Eq_22::* di_374 = (union Eq_841 Eq_22::*) &di_373->w0001;
                    segptr32 ptrArg04_1638 = &(psegArg06_1805->*si_97);
                    if ((wLoc04_1334 & 0x10) != 0x00)
                    {
                        word16 dx_385 = (ss->*di_373).w0001;
                        struct Eq_1143 Eq_22::* di_387 = (struct Eq_1143 Eq_22::*) (&di_373->w0001 + 1);
                    }
                    word16 wArg02_1382 = di_389;
                    Eq_332 ax_395;
                    struct Eq_22 * dx_402;
                    if (ax_395 == 0x00)
                    {
                        struct Eq_22 * dx_405 = dx_402;
                        if (dx_402 != 0x00)
                            goto l0800_092C;
                        Eq_328 bx_1262 = bx_2101;
                        if (wLoc0C_1352 == 0x00)
                            goto l0800_078F;
                    }
                    else
                    {
l0800_092C:
                        cui16 wLoc04_1385 = wLoc04_1334 | 0x04;
                    }
                    struct Eq_1157 Eq_22::* sp_426 = sp_1284 - 2;
                    (ss->*sp_426).ptr0000 = dx_402;
                    (ss->*sp_426).tFFFFFFFE.u1 = (word16) ax_395;
                    (ss->*sp_426).ptrFFFFFFFC = ss;
                    (ss->*sp_426).tFFFFFFFA.u0 = (mp16) (fp - 71);
                    Eq_330 ax_448;
                    ax_448.u0 = (int16) bh_443;
                    (ss->*sp_426).tFFFFFFF8.u0 = (int16) ax_448;
                    byte bLoc08_1392;
                    (ss->*sp_426).tFFFFFFF6.u1 = (word16) SEQ(SLICE(ax_448, byte, 8), bLoc08_1392);
                    (ss->*sp_426).tFFFFFFF4.u1 = (word16) bx_2101;
                    __LONGTOA((ss->*sp_426).tFFFFFFF4.u0, (ss->*sp_426).tFFFFFFF6.u0, (ss->*sp_426).tFFFFFFF8.u0, (ss->*sp_426).tFFFFFFFA.u1, (ss->*sp_426).tFFFFFFFE.u0);
                    (ss->*sp_426).ptr0000 = ss;
                    struct Eq_22 * es_479 = (ss->*sp_426).ptr0000;
                    struct Eq_1226 Eq_22::* sp_480 = (struct Eq_1226 Eq_22::*) ((char *) &sp_426->ptr0000 + 2);
                    Eq_937 dx_482 = wLoc0C_1352;
                    if (wLoc0C_1352 >= 0x00)
                    {
                        ptr32 es_di_2103 = (ptr32) (fp - 71);
                        goto l0800_0A4E;
                    }
                    else
                    {
                        ptr32 es_di_2108 = (ptr32) (fp - 71);
                        goto l0800_0A40;
                    }
                case 0x0E:
                    struct Eq_914 Eq_22::* sp_281 = sp_1284 - 2;
                    (ss->*sp_281).t0000.u0 = (ss->*wArg02_1338).t0000.u0;
                    mp16 di_278 = fp - 72;
                    union Eq_841 Eq_22::* wArg02_1365 = (union Eq_841 Eq_22::*) &wArg02_1338->w0002;
                    segptr32 ptrArg04_1664 = &(psegArg06_1805->*si_97);
                    if ((wLoc04_1334 & 0x20) != 0x00)
                    {
                        word16 dx_290 = (ss->*wArg02_1338).w0002;
                        (ss->*sp_281).ptrFFFFFFFE = ss;
                        struct Eq_22 * es_297 = (ss->*sp_281).ptrFFFFFFFE;
                        byte Eq_22::* di_303 = fn0800_070A(fp - 72, SLICE(dx_290, byte, 8), (byte) dx_290);
                        es_297->*di_303 = 0x3A;
                        struct Eq_1645 Eq_22::* wArg02_1366 = (struct Eq_1645 Eq_22::*) (&wArg02_1338->w0002 + 1);
                        byte Eq_22::* di_310 = di_303 + 1;
                    }
                    (ss->*sp_281).ptrFFFFFFFE = ss;
                    Eq_841 dx_317;
                    dx_317.u0 = (ss->*sp_281).t0000.u0;
                    struct Eq_22 * es_315 = (ss->*sp_281).ptrFFFFFFFE;
                    byte Eq_22::* di_324 = fn0800_070A(di_319, SLICE(dx_317, byte, 8), (byte) dx_317);
                    ss->*di_324 = 0x00;
                    struct Eq_1608 Eq_22::* sp_318 = (struct Eq_1608 Eq_22::*) ((char *) &sp_281->t0000 + 2);
                    cui16 wLoc04_1369 = wLoc04_1334 & ~0x04;
                    Eq_937 di_336 = di_324 - (fp - 72);
                    Eq_937 dx_340 = wLoc0C_1352;
                    if (wLoc0C_1352 <= di_336)
                    {
                        Eq_937 dx_343 = di_336;
                    }
                    ptr32 es_di_2110 = (ptr32) (fp - 72);
                    goto l0800_0A40;
                case 0x0F:
                    Eq_937 cx_220 = wLoc0C_1352;
                    segptr32 ptrArg04_1661 = &(psegArg06_1805->*si_97);
                    if (wLoc0C_1352 < 0x00)
                    {
                        word16 cx_226 = 0x06;
                    }
                    struct Eq_1270 Eq_22::* sp_228 = sp_1284 - 2;
                    (ss->*sp_228).ptr0000 = ss;
                    (ss->*sp_228).ptrFFFFFFFE = wArg02_1338;
                    word16 cx_235;
                    (ss->*sp_228).wFFFFFFFC = cx_235;
                    (ss->*sp_228).ptrFFFFFFFA = ss;
                    (ss->*sp_228).ptrFFFFFFF8 = fp - 71;
                    (ss->*sp_228).tFFFFFFF6.u0 = (ci16) ax_67;
                    (ss->*sp_228).wFFFFFFF4 = wLoc04_1334 & 0x01;
                    if ((wLoc04_1334 & 0x0100) != 0x00)
                    {
                        word16 ax_259 = 0x08;
                        union Eq_841 Eq_22::* wArg02_1362 = (union Eq_841 Eq_22::*) (&wArg02_1338->w0002 + 4);
                    }
                    else
                    {
                        union Eq_841 Eq_22::* wArg02_1361 = (union Eq_841 Eq_22::*) (&wArg02_1338->w0002 + 3);
                        word16 ax_258 = 0x06;
                    }
                    word16 ax_265;
                    (ss->*sp_228).wFFFFFFF2 = ax_265;
                    __REALCVT(ds);
                    (ss->*sp_228).ptrFFFFFFF0 = ss;
                    struct Eq_1441 Eq_22::* sp_273 = sp_228 - 0x0E;
                    ptr32 es_di_2109 = (ptr32) (fp - 71);
l0800_0A40:
                    struct Eq_1251 * es_508 = SLICE(es_di_2111, selector, 16);
                    word16 di_505 = (word16) es_di_2111;
                    cui16 wLoc04_1396;
                    if ((wLoc04_1396 & 0x08) != 0x00)
                    {
                        int16 dx_499 = wLoc0A_1406;
                        ptr32 es_di_2102 = es_di_2111;
                        if (wLoc0A_1406 > 0x00)
                        {
l0800_0A4E:
                            struct Eq_1113 * es_507 = SLICE(es_di_2104, selector, 16);
                            byte Eq_1113::* di_504 = (word16) es_di_2104;
                            Eq_937 cx_511 = fn0800_073C(di_504, es_507);
                            if (*es_di_2104 == 0x2D)
                            {
                                word16 cx_519 = cx_511 - 0x01;
                            }
                            ci16 dx_524 = dx_522 - cx_523;
                            if (dx_524 > 0x00)
                            {
                                ci16 wLoc10_1412 = dx_524;
                            }
                        }
                    }
                    struct Eq_1113 * es_532;
                    byte Eq_1113::* di_534;
                    if (es_532->*di_534 != 0x2D)
                    {
                        byte bLoc0D_1413;
                        if (bLoc0D_1413 == 0x00)
                            goto l0800_0A80;
                        byte Eq_1113::* di_548 = di_534 - 0x01;
                        es_532->*di_548 = bLoc0D_1413;
                    }
                    ci16 wLoc10_1428;
                    if (wLoc10_1428 > 0x00 && wLoc0C_1352 < 0x00)
                    {
                        ci16 wLoc10_1451 = wLoc10_1428 - 0x01;
                    }
l0800_0A80:
                    Eq_937 cx_571 = fn0800_073C(di_566, es_532);
                    goto l0800_0A83;
                case 0x10:
                    struct Eq_22 * Eq_22::* sp_203 = sp_1284 - 2;
                    ss->*sp_203 = (struct Eq_22 * Eq_22::*) ss;
                    union Eq_841 Eq_22::* wArg02_1355 = (union Eq_841 Eq_22::*) &wArg02_1338->w0002;
                    struct Eq_22 * es_205 = (struct Eq_22 *) (ss->*sp_203);
                    struct Eq_22 * Eq_22::* sp_206 = (struct Eq_22 * Eq_22::*) ((char *) sp_203 + 2);
                    mp16 di_207 = fp - 71;
                    word16 cx_215 = 0x01;
                    segptr32 ptrArg04_1649 = &(psegArg06_1805->*si_97);
                    goto l0800_0A83;
                case 0x11:
                    segptr32 ptrArg04_1653 = &(psegArg06_1805->*si_97);
                    if ((wLoc04_1334 & 0x20) == 0x00)
                    {
                        Eq_841 di_157;
                        di_157.u0 = (ss->*wArg02_1338).t0000.u0;
                        struct Eq_22 * Eq_22::* sp_160 = sp_1284 - 2;
                        ss->*sp_160 = (struct Eq_22 * Eq_22::*) ds;
                        union Eq_841 Eq_22::* wArg02_1349 = (union Eq_841 Eq_22::*) &wArg02_1338->w0002;
                        struct Eq_22 * es_162 = (struct Eq_22 *) (ss->*sp_160);
                        Eq_841 di_164 = di_157;
                        bool v74_1315 = di_157 != 0x00;
                    }
                    else
                    {
                        Eq_841 es_di_147;
                        es_di_147.u2 = (ds->*wArg02_1338).t0000.u2;
                        struct Eq_1054 * es_150 = SLICE(es_di_147, selector, 16);
                        word16 di_152 = (word16) es_di_147;
                        union Eq_841 Eq_22::* wArg02_1348 = (union Eq_841 Eq_22::*) (&wArg02_1338->w0002 + 1);
                        bool v75_1316 = es_di_147 != 0x00;
                    }
                    bool v73_1314;
                    if (!v73_1314)
                    {
                        struct Eq_22 * Eq_22::* sp_172 = sp_1284 - 2;
                        ss->*sp_172 = (struct Eq_22 * Eq_22::*) ds;
                        struct Eq_22 * es_176 = (struct Eq_22 *) (ss->*sp_172);
                        word16 di_178 = 0x050E;
                    }
                    word16 sp_1296 = sp_1284;
                    Eq_937 cx_184 = fn0800_073C(di_179, es_181);
                    if (cx_184 > wLoc0C_1352)
                    {
                        Eq_937 cx_196 = wLoc0C_1352;
                    }
l0800_0A83:
                    cui16 ax_591 = wLoc04_1482 & 0x05;
                    byte Eq_1458::* si_578 = di_576;
                    byte Eq_22::* di_588 = di_53;
                    int16 bx_589 = wLoc0A_1406;
                    if (ax_591 == 0x05)
                    {
                        ci16 wLoc10_1512;
                        if (dl_140 != 111)
                        {
                            if (dl_140 == 0x78 || dl_140 == 88)
                            {
                                ci16 v45_610 = wLoc10_1512 - 0x02;
                                cui16 wLoc04_1510 = wLoc04_1482 | 0x40;
                                ci16 bx_609 = wLoc0A_1406 - 0x02;
                                ci16 wLoc10_1517 = v45_610;
                                if (v45_610 < 0x00)
                                {
                                    word16 wLoc10_1518 = 0x00;
                                }
                            }
l0800_0AC6:
                            ci16 cx_629 = (word16) wLoc10_1520.u0 + cx_619;
                            cui16 wLoc04_1522;
                            Eq_705 dx_679;
                            if ((wLoc04_1522 & 0x02) == 0x00)
                            {
                                ci16 bx_634;
                                while (bx_634 > cx_629)
                                {
                                    byte Eq_22::* di_644;
                                    word16 di_647;
                                    fn0800_0749(fp - 2, 0x20, cx_629, dx_679, bx_634, di_644, out di_647);
                                    ci16 bx_651 = bx_634 - 0x01;
                                }
                            }
                            ci16 bx_673;
                            if ((wLoc04_1522 & 0x40) != 0x00)
                            {
                                byte Eq_22::* di_692;
                                word16 di_701;
                                fn0800_0749(fp - 2, dl_140, cx_629, dx_679, bx_673, di_692, out di_701);
                            }
                            Eq_705 dx_709 = wLoc10_1520;
                            selector es_721;
                            if (wLoc10_1520 > 0x00)
                            {
                                ci16 cx_715 = cx_629 - wLoc10_1520;
                                ci16 bx_717 = bx_673 - wLoc10_1520;
                                byte al_732 = es_721->*di_576;
                                if (al_732 == 0x2D || (al_732 == 0x20 || al_732 == 0x2B))
                                {
                                    byte Eq_1458::* si_743 = di_576 + 1;
                                    byte Eq_22::* di_752;
                                    word16 di_757;
                                    fn0800_0749(fp - 2, es_721->*di_576, cx_715, wLoc10_1520, bx_717, di_752, out di_757);
                                    ci16 cx_761 = cx_715 - 0x01;
                                    word16 bx_762 = bx_717 - 0x01;
                                }
                                Eq_705 cx_768 = wLoc10_1520;
                                Eq_705 cx_766;
                                if (wLoc10_1520 != 0x00)
                                {
                                    word16 cx_784;
                                    do
                                    {
                                        ci16 cx_777;
                                        ci16 bx_793;
                                        byte Eq_22::* di_775;
                                        word16 di_780;
                                        fn0800_0749(fp - 2, 0x30, cx_777, cx_766, bx_793, di_775, out di_780);
                                        cx_784 = cx_777 - 0x01;
                                    } while (cx_784 != 0x00);
                                }
                                word16 dx_788 = cx_787;
                                Eq_705 cx_789 = cx_766;
                            }
                            word16 cx_790;
                            Eq_705 dx_1210;
                            if (cx_790 != 0x00)
                            {
                                ci16 bx_794 = bx_791 - cx_790;
                                word16 cx_820;
                                do
                                {
                                    byte Eq_22::* di_802;
                                    byte Eq_1458::* si_797;
                                    ss->*di_802 = es_721->*si_797;
                                    ci8 v50_806 = bLoc16_1534 - 0x01;
                                    byte Eq_1458::* si_800 = si_797 + 1;
                                    byte Eq_22::* di_804 = di_802 + 1;
                                    ci8 bLoc16_1535 = v50_806;
                                    ci16 cx_811;
                                    if (v50_806 <= 0x00)
                                    {
                                        word16 di_814;
                                        fn0800_0752(fp - 2, cx_811, dx_1210, bx_794, di_804, out di_814);
                                    }
                                    cx_820 = cx_811 - 0x01;
                                } while (cx_820 != 0x00);
                            }
                            ci16 bx_823 = bx_821;
                            if (bx_821 > 0x00)
                            {
                                ci16 cx_828 = bx_821;
                                word16 cx_844;
                                do
                                {
                                    ci16 cx_837;
                                    byte Eq_22::* di_835;
                                    word16 di_840;
                                    fn0800_0749(fp - 2, 0x20, cx_837, dx_1210, bx_821, di_835, out di_840);
                                    cx_844 = cx_837 - 0x01;
                                } while (cx_844 != 0x00);
                            }
                            goto l0800_0792;
                        }
                        if (wLoc10_1512 <= 0x00)
                        {
                            word16 wLoc10_1519 = 0x01;
                        }
                    }
                    goto l0800_0AC6;
                case 0x12:
                    segptr32 ptrArg04_1641 = &(psegArg06_1805->*si_97);
                    if ((wLoc04_1334 & 0x20) == 0x00)
                    {
                        Eq_841 di_108;
                        di_108.u0 = (ss->*wArg02_1338).t0000.u0;
                        struct Eq_22 * Eq_22::* sp_113 = sp_1284 - 2;
                        ss->*sp_113 = (struct Eq_22 * Eq_22::*) ds;
                        union Eq_841 Eq_22::* wArg02_1340 = (union Eq_841 Eq_22::*) &wArg02_1338->w0002;
                        struct Eq_22 * es_115 = (struct Eq_22 *) (ss->*sp_113);
                    }
                    else
                    {
                        Eq_841 es_di_104;
                        es_di_104.u2 = (ds->*wArg02_1338).t0000.u2;
                        struct Eq_1008 * es_127 = SLICE(es_di_104, selector, 16);
                        word16 di_129 = (word16) es_di_104;
                        union Eq_841 Eq_22::* wArg02_1339 = (union Eq_841 Eq_22::*) (&wArg02_1338->w0002 + 1);
                    }
                    selector es_126;
                    struct Eq_1039 Eq_1038::* di_128;
                    (es_126->*di_128).w0000 = (uint16) (0x50 - bLoc16_1583);
                    word16 sp_1287 = sp_1284;
                    if ((wLoc04_1334 & 0x10) != 0x00)
                        (es_126->*di_128).w0002 = 0x00;
                    goto l0800_078F;
                case 0x13:
                case 0x14:
                case 0x15:
                    goto l0800_0B7C;
                case 22:
                    cui16 wLoc04_1336 = wLoc04_1334 & ~0x20;
                    goto l0800_082B;
                case 0x17:
                    cui16 wLoc04_1335 = wLoc04_1334 | 0x20;
l0800_082B:
                    byte ch_876 = 0x05;
l0800_07D5:
                    byte al_998 = es_1125->*si_97;
                    byte Eq_662::* si_999 = si_97 + 1;
                    goto l0800_07D7;
                }
            }
l0800_0B7C:
            Eq_705 dx_1256 = ax_67;
            byte Eq_662::* si_1009 = si_38;
            byte Eq_22::* di_1011 = di_53;
            word16 ax_1229 = SEQ(ah_1224, 0x25);
            byte al_1029;
            do
            {
                word16 ax_1231 = SEQ(ah_1230, al_1029);
                word16 ax_1013;
                ci16 cx_1236;
                ci16 bx_1232;
                byte Eq_22::* di_1017;
                word16 di_1022;
                ah_1230 = SLICE((uint16) fn0800_0749(fp - 2, (byte) ax_1013, cx_1236, ax_67, bx_1232, di_1017, out di_1022), byte, 8);
                byte Eq_816::* si_1027;
                al_1029 = psegArg06_1805->*si_1027;
                byte Eq_816::* si_1030 = si_1027 + 1;
            } while (al_1029 != 0x00);
            break;
        }
l0800_079F:
        byte al_1082;
        ss->*di_53 = al_1082;
        ci8 v18_1088 = bLoc16_1583 - 0x01;
        byte Eq_22::* di_1086 = di_53 + 1;
        ci8 bLoc16_1629 = v18_1088;
        if (v18_1088 <= 0x00)
        {
            ci16 cx_1266;
            Eq_705 dx_1277;
            ci16 bx_1259;
            word16 di_1096;
            fn0800_0752(fp - 2, cx_1266, dx_1277, bx_1259, di_1086, out di_1096);
        }
    }
    if (bLoc16_1583 < 0x50)
    {
        ci16 cx_1044;
        Eq_705 dx_1046;
        ci16 bx_1042;
        byte Eq_22::* di_1048;
        word16 di_2115;
        fn0800_0752(fp - 2, cx_1044, dx_1046, bx_1042, di_1048, out di_2115);
    }
}


word16 <unnamed> = 0x0811;

word16 <unnamed> = 0x07FB;

word16 <unnamed> = 2118;

word16 <unnamed> = 0x0806;

word16 <unnamed> = 0x0874;

word16 <unnamed> = 0x0881;

word16 <unnamed> = 2239;

word16 <unnamed> = 2246;

word16 <unnamed> = 2251;

word16 <unnamed> = 0x082F;

word16 <unnamed> = 2292;

word16 <unnamed> = 2258;

word16 <unnamed> = 2262;

word16 <unnamed> = 2266;

word16 <unnamed> = 2383;

word16 <unnamed> = 2556;

word16 <unnamed> = 0x099F;

word16 <unnamed> = 0x09BF;

word16 <unnamed> = 0x0B40;

word16 <unnamed> = 0x0B7C;

word16 <unnamed> = 0x0B7C;

word16 <unnamed> = 0x0B7C;

word16 <unnamed> = 0x0821;

word16 <unnamed> = 0x0827;

word32 __VPTR(struct Eq_22 * ds, Eq_1810 wArg02, Eq_1811 wArg04)
{
    return SEQ(ds->w0583, ds->w0581 + (wArg02 - 0x01) *s (uint16) ds->b057E + (wArg04 - 0x01) << 0x01);
}


void __VRAM(struct Eq_22 * ds, uint16 wArg02, uint16 * ptrArg04, uint16 * ptrArg08)
{
    uint16 ax_20 = (uint16) ds->b0580;
    uint16 cx_100 = wArg02;
    if (wArg02 != 0x00)
    {
        <unknown>* di_125 = (word16) ptrArg08;
        struct Eq_1846 * es_46 = SLICE(ptrArg08, selector, 16);
        <unknown>* si_115 = (word16) ptrArg04;
        struct Eq_1846 * ds_49 = SLICE(ptrArg04, selector, 16);
        if (si_115 < di_125)
        {
            uint16 ax_34 = wArg02 - 0x01 << 0x01;
            si_115 = (word16) si_115 + ax_34;
            di_125 = (word16) di_125 + ax_34;
        }
        if (ax_20 == 0x00)
            memcpy(di_125, si_115, wArg02 *32 0x02);
        else if (es_46 != ds_49)
        {
            do
            {
                __cli();
                while ((__in<byte>(0x03DA) & 0x01) != 0x00)
                    ;
                while ((__in<byte>(0x03DA) & 0x01) == 0x00)
                    ;
                es_46->*di_125 = (<unknown>*) (ds_49->*si_115);
                __sti();
                si_115 = (<unknown>*) ((char *) si_115 + 2);
                di_125 = (<unknown>*) ((char *) di_125 + 2);
                --cx_100;
            } while (cx_100 != 0x00);
        }
        else
        {
            do
            {
                __cli();
                while ((__in<byte>(0x03DA) & 0x01) != 0x00)
                    ;
                while ((__in<byte>(0x03DA) & 0x01) == 0x00)
                    ;
                word16 ax_70 = (word16) (ds_49->*si_115);
                __sti();
                si_115 = (<unknown>*) ((char *) si_115 + 2);
                while ((__in<byte>(0x03DA) & 0x01) != 0x00)
                    ;
                while ((__in<byte>(0x03DA) & 0x01) == 0x00)
                    ;
                es_46->*di_125 = (<unknown>*) ax_70;
                __sti();
                di_125 = (<unknown>*) ((char *) di_125 + 2);
                --cx_100;
            } while (cx_100 != 0x00);
        }
    }
}


word32 __stpcpy(Eq_484 dwArg04, Eq_194 dwArg08)
{
    word16 wArg06 = SLICE(dwArg04, word16, 16);
    word16 wArg04 = (word16) dwArg04;
    word16 ax_18 = _strlen(dwArg08);
    _memcpy(dwArg04, dwArg08, ax_18 + 0x01);
    return SEQ(wArg06, wArg04 + ax_18);
}


word16 _clrscr(struct Eq_22 * ds, ptr16 & dxOut, ptr16 & bxOut)
{
    byte al_45 = __SCROLL(ds, 0x00, ds->b0579, ds->b0578, ds->b0577, ds->b0576, 0x06);
    byte dl_41 = ds->b0576;
    byte dh_42 = ds->b0577;
    word16 cx_50;
    ptr16 bx_69;
    __VideoInt(SEQ(0x02, al_45), out cx_50, out bx_69);
    dxOut = SEQ(dh_42, dl_41);
    bxOut = bx_69;
    return cx_50;
}


byte fn0800_0D09(union Eq_554 & cxOut, union Eq_536 & blOut)
{
    cui16 ax;
    word16 cx_27;
    word16 bx_28;
    __VideoInt(ax, out cx_27, out bx_28);
    cxOut.u1 = (<unknown>*) <invalid>;
    Eq_536 bl_10;
    bl_10.u0 = <invalid>;
    blOut = bl_10;
    return (byte) bl_10.u0 + 0x00F0;
}


cui16 __VideoInt(cui16 ax, ptr16 & cxOut, union Eq_554 & bxOut)
{
    ptr16 cx;
    byte ah = SLICE(ax, byte, 8);
    byte al = (byte) ax;
    ptr16 cx_138 = cx;
    cui16 ax_112;
    if (ah != 0x00)
    {
        ax_112 = ax;
        if (ah == 0x0F)
        {
            __syscall<byte>(0x10);
            ax_112 = ax;
            if (al == 0x03)
            {
                ax_112 = ax;
                byte ah_118 = SLICE(ax, byte, 8);
                byte bl_80;
                if (fn0800_0D09(out cx_138, out bl_80) != 0x00 && 0040:0084 != 0x18)
                    ax_112 = SEQ(ah_118, 0x40);
            }
            goto l0800_0D98;
        }
    }
    else if (al == 0x03)
    {
        byte bh_55;
        bios_video_get_DAC_color_page_state(out bh_55);
        if (true)
            0040:0087 = (byte *) (0040:0087 & ~0x01);
        bios_video_set_text_mode_cursor_shape(0x06, 0x07);
        cx_138 = (<unknown>*) 0x0607;
        ax_112 = 0x03;
    }
    else
    {
        ax_112 = ax;
        if (al == 0x40)
        {
            byte bl_28;
            byte ch_29;
            byte cl_30;
            bios_video_get_EGA_info(out bl_28, out ch_29, out cl_30);
            ax_112 = SEQ(0x12, al);
            cx_138 = SEQ(ch_29, cl_30);
            if (bl_28 != 0x10)
            {
                bios_video_load_ROM_8x8_dbl_dot_patterns(0x00);
                byte bl_35;
                byte ch_36;
                byte cl_37;
                bios_video_get_EGA_info(out bl_35, out ch_36, out cl_37);
                byte bh_40;
                bios_video_get_DAC_color_page_state(out bh_40);
                ax_112 = 6656;
                cx_138 = SEQ(ch_36, cl_37);
                if (true)
                {
                    0040:0087 = (byte *) (0040:0087 | 0x01);
                    bios_video_set_text_mode_cursor_shape(0x06, 0x00);
                    cx_138 = (<unknown>*) 0x0600;
                    ax_112 = 0x0100;
                }
            }
l0800_0D98:
            cxOut = cx_138;
            bxOut.u1 = (<unknown>*) <invalid>;
            return ax_112;
        }
    }
    __syscall<byte>(0x10);
    goto l0800_0D98;
}


void fn0800_0EBE(Eq_194 dx)
{
    selector seg0800;
    if (dx != seg0800->t0EB2)
    {
        Eq_194 ds_9 = Mem0[dx:0x02:selector];
        struct Eq_22 * ds_36;
        Eq_194 wLoc02_69;
        if (Mem0[ds_9:0x02:word16] != 0x00)
        {
            seg0800->t0EB4 = ds_9;
l0800_0F11:
            ds_36 = seg0800->ptr0EB8;
            wLoc02_69 = dx;
            goto l0800_0F1A;
        }
        if (ds_9 != seg0800->t0EB2)
        {
            Mem20[0800:0EB4:word16] = Mem0[ds_9:0x08:word16];
            fn0800_0F92(ds_9);
            wLoc02_69 = ds_9;
            ds_36 = seg0800->ptr0EB8;
l0800_0F1A:
            __brk(ds_36, 0x00, wLoc02_69);
            return;
        }
        dx = seg0800->t0EB2;
    }
    seg0800->t0EB2.u0 = 0x00;
    seg0800->t0EB4.u0 = 0x00;
    seg0800->t0EB6.u0 = 0x00;
    goto l0800_0F11;
}


void fn0800_0F21(Eq_194 dx)
{
    Eq_194 es_9 = Mem8[dx:0x02:selector];
    Mem10[dx:0x02:word16] = 0x00;
    Mem11[dx:0x08:word16] = es_9;
    Eq_194 wLoc02_68 = dx;
    selector seg0800;
    if (dx != seg0800->t0EB2 && Mem11[es_9:0x02:word16] == 0x00)
    {
        Eq_2234 ax_26 = Mem11[dx:0x00:word16];
        Mem32[es_9:0x00:word16] = Mem30[es_9:0x00:word16] + ax_26;
        wLoc02_68 = es_9;
        word16 dx_34 = dx + ax_26;
        if (dx_34->t0002 == 0x00)
            dx_34->t0008 = es_9;
        else
            dx_34->t0002 = es_9;
    }
    else
        fn0800_0FBB(dx);
    Eq_194 ax_49 = wLoc02_68 + Mem44[wLoc02_68:0x00:word16];
    if (Mem44[ax_49:0x02:word16] != 0x00)
        return;
    Mem55[wLoc02_68:0x00:word16] = Mem44[wLoc02_68:0x00:word16] + Mem44[ax_49:0x00:word16];
    (ax_49 + Mem55[ax_49:0x00:word16])->t0002 = wLoc02_68;
    fn0800_0F92(ax_49);
}


void fn0800_0F92(Eq_194 ds)
{
    selector seg0800;
    if (ds == Mem0[ds:0x06:word16])
        seg0800->t0EB6.u0 = 0x00;
    else
    {
        Eq_194 es_10 = Mem0[ds:0x06:selector];
        Eq_194 ds_11 = Mem0[ds:0x04:selector];
        Mem12[ds_11:0x06:word16] = es_10;
        Mem13[es_10:0x04:word16] = ds_11;
        seg0800->t0EB6 = ds_11;
    }
}


void fn0800_0FBB(Eq_194 ds)
{
    selector seg0800;
    Eq_194 ax_5 = seg0800->t0EB6;
    if (ax_5 == 0x00)
    {
        seg0800->t0EB6 = ds;
        Mem13[ds:0x04:word16] = ds;
        Mem14[ds:0x06:word16] = ds;
    }
    else
    {
        __cli();
        Eq_194 es_22 = Mem20[ax_5:0x06:selector];
        Mem23[ax_5:0x06:word16] = ds;
        Mem24[ds:0x04:word16] = ax_5;
        Mem28[es_22:0x04:word16] = ds;
        Mem29[ds:0x06:word16] = es_22;
    }
}


struct Eq_22 * _farfree(struct Eq_22 * ds, Eq_194 wArg06)
{
    selector seg0800;
    seg0800->ptr0EB8 = ds;
    if (wArg06 != 0x00)
    {
        if (wArg06 == seg0800->t0EB4)
            fn0800_0EBE(wArg06);
        else
            fn0800_0F21(wArg06);
    }
    return seg0800->ptr0EB8;
}


Eq_2234 fn0800_101B(Eq_2234 ax, union Eq_194 & dxOut)
{
    word16 dx_133;
    selector seg0800;
    cui16 ax_17 = __sbrk(seg0800->ptr0EB8, 0x00, out dx_133);
    if ((ax_17 & 0x0F) != 0x00)
    {
        word16 dx_134;
        __sbrk(seg0800->ptr0EB8, (uint32) (0x10 - (ax_17 & 0x0F)), out dx_134);
    }
    Eq_194 dx_78;
    if (__sbrk(seg0800->ptr0EB8, SEQ((uint16) SLICE(ax, byte, 8) >> 0x04, ax << 0x04), out dx_78) == ~0x00)
    {
        dxOut.u0 = 0x00;
        return 0x00;
    }
    else
    {
        seg0800->t0EB2 = dx_78;
        seg0800->t0EB4 = dx_78;
        Mem97[dx_78:0x00:word16] = ax;
        Mem98[dx_78:0x02:word16] = dx_78;
        dxOut = dx_78;
        return 0x04;
    }
}


Eq_2234 fn0800_107F(Eq_2234 ax, union Eq_194 & dxOut)
{
    Eq_194 dx_25;
    selector seg0800;
    byte ah_9 = SLICE(ax, byte, 8);
    struct Eq_22 * ds_17 = seg0800->ptr0EB8;
    cui16 ax_22 = __sbrk(ds_17, SEQ((uint16) ah_9 >> 0x04, ax << 0x04), out dx_25);
    if (ax_22 != ~0x00)
    {
        if ((ax_22 & 0x0F) == 0x00)
        {
l0800_10A2:
            Eq_194 cx_79 = seg0800->t0EB4;
            seg0800->t0EB4 = dx_25;
            Mem84[dx_25:0x00:word16] = ax;
            Mem85[dx_25:0x02:word16] = cx_79;
            dxOut = dx_25;
            return 0x04;
        }
        word16 dx_131;
        if (__sbrk(ds_17, (uint32) (-(ax_22 & 0x0F) + 0x10), out dx_131) != ~0x00)
        {
            dx_25.u4 = (word32) dx_25 + 1;
            goto l0800_10A2;
        }
    }
    dxOut.u0 = 0x00;
    return 0x00;
}


Eq_2234 fn0800_10D9(Eq_2234 ax, Eq_194 dx, Eq_194 ds, struct Eq_2449 & dxOut)
{
    Mem10[ds:0x00:word16] = Mem0[ds:0x00:word16] - ax;
    struct Eq_2449 * dx_11 = dx + Mem10[ds:0x00:word16];
    dx_11->t0000 = ax;
    dx_11->t0002 = dx;
    (dx_11 + Mem14[dx_11:0x00:word16])->ptr0002 = dx_11;
    dxOut = dx_11;
    return 0x04;
}


Eq_194 _malloc(struct Eq_22 * ds, Eq_194 wArg04, union Eq_194 & dxOut, struct Eq_22 & dsOut)
{
    Eq_194 dx_109;
    dx_109.u0 = 0x00;
    Eq_194 ax_105 = wArg04;
    selector seg0800;
    seg0800->ptr0EB8 = ds;
    if (wArg04 != 0x00)
    {
        Eq_2490 dx_ax_131 = (uint32) wArg04 + 0x13;
        cu16 ax_28 = (word16) dx_ax_131;
        cu16 dx_31 = SLICE(dx_ax_131, word16, 16);
        if (dx_31 >= 0x00 && (dx_31 & ~0x0F) == 0x00)
        {
            Eq_2234 ax_56 = SEQ(SLICE(ax_28 >> 0x04, byte, 8) | (byte) dx_31 << 0x04, (byte) (ax_28 >> 0x04));
            if (seg0800->t0EB2 != 0x00)
            {
                Eq_194 dx_67 = seg0800->t0EB6;
                dx_109 = dx_67;
                if (dx_67 != 0x00)
                {
                    do
                    {
                        if (Mem26[dx_109:0x00:word16] >=u ax_56)
                        {
                            if (Mem26[dx_109:0x00:word16] <=u ax_56)
                            {
                                fn0800_0F92(dx_109);
                                Mem96[dx_109:0x02:word16] = Mem26[dx_109:0x08:word16];
                                ax_105.u0 = 0x04;
                            }
                            else
                                ax_105 = fn0800_10D9(ax_56, dx_109, dx_109, out dx_109);
                            goto l0800_117A;
                        }
                        dx_109 = Mem26[dx_109:0x06:word16];
                    } while (dx_109 != dx_67);
                }
                ax_105 = fn0800_107F(ax_56, out dx_109);
            }
            else
                ax_105 = fn0800_101B(ax_56, out dx_109);
        }
        else
        {
            ax_105.u0 = 0x00;
            dx_109.u0 = 0x00;
        }
    }
l0800_117A:
    struct Eq_22 * ds_118 = seg0800->ptr0EB8;
    dxOut = dx_109;
    dsOut = ds_118;
    return ax_105;
}


Eq_194 _malloc(struct Eq_22 * ds, Eq_194 wArg04, union Eq_194 & dxOut, struct Eq_22 & dsOut)
{
    Eq_194 dx_109;
    dx_109.u0 = 0x00;
    Eq_194 ax_105 = wArg04;
    selector seg0800;
    seg0800->ptr0EB8 = ds;
    if (wArg04 != 0x00)
    {
        Eq_2490 dx_ax_131 = (uint32) wArg04 + 0x13;
        cu16 ax_28 = (word16) dx_ax_131;
        cu16 dx_31 = SLICE(dx_ax_131, word16, 16);
        if (dx_31 >= 0x00 && (dx_31 & ~0x0F) == 0x00)
        {
            Eq_2234 ax_56 = SEQ(SLICE(ax_28 >> 0x04, byte, 8) | (byte) dx_31 << 0x04, (byte) (ax_28 >> 0x04));
            if (seg0800->t0EB2 != 0x00)
            {
                Eq_194 dx_67 = seg0800->t0EB6;
                dx_109 = dx_67;
                if (dx_67 != 0x00)
                {
                    do
                    {
                        if (Mem26[dx_109:0x00:word16] >=u ax_56)
                        {
                            if (Mem26[dx_109:0x00:word16] <=u ax_56)
                            {
                                fn0800_0F92(dx_109);
                                Mem96[dx_109:0x02:word16] = Mem26[dx_109:0x08:word16];
                                ax_105.u0 = 0x04;
                            }
                            else
                                ax_105 = fn0800_10D9(ax_56, dx_109, dx_109, out dx_109);
                            goto l0800_117A;
                        }
                        dx_109 = Mem26[dx_109:0x06:word16];
                    } while (dx_109 != dx_67);
                }
                ax_105 = fn0800_107F(ax_56, out dx_109);
            }
            else
                ax_105 = fn0800_101B(ax_56, out dx_109);
        }
        else
        {
            ax_105.u0 = 0x00;
            dx_109.u0 = 0x00;
        }
    }
l0800_117A:
    struct Eq_22 * ds_118 = seg0800->ptr0EB8;
    dxOut = dx_109;
    dsOut = ds_118;
    return ax_105;
}


word16 fn0800_12E4(struct Eq_22 * ds, Eq_194 wArg02, Eq_194 wArg04)
{
    cu16 si_15 = (word32) wArg04 + 1 - ds->ptr007B;
    word16 ax_100;
    if (si_15 + 0x3F >> 0x06 != ds->w05C0)
    {
        cu16 si_22 = __align(si_15 + 0x3F, 64);
        cup16 dx_23 = ds->w0091;
        cup16 ax_25 = ds->ptr007B + __align(si_15 + 0x3F, 64);
        byte al_41 = (byte) ax_25;
        if (ax_25 > dx_23)
            si_22 = dx_23 - ds->ptr007B;
        word16 ax_42 = _setblock(al_41, ds, ds->ptr007B, si_22);
        if (ax_42 != ~0x00)
        {
            ds->w0091 = ds->ptr007B + ax_42;
            ds->w008F = 0x00;
            ax_100 = 0x00;
            return ax_100;
        }
        ds->w05C0 = si_22 >> 0x06;
    }
    ds->t008D = wArg04;
    ds->t008B = wArg02;
    ax_100 = 0x01;
    return ax_100;
}


void __brk(struct Eq_22 * ds, Eq_194 wArg02, Eq_194 wArg04)
{
    fn0800_05D4(wArg02, ds->w0089, wArg04, ds->w0087);
    bool C_19 = <invalid>;
    if (C_19)
        return;
    fn0800_05D4(wArg02, ds->w0091, wArg04, ds->w008F);
    bool Z_32 = <invalid>;
    bool C_31 = <invalid>;
    if (Z_32 | C_31 || fn0800_12E4(ds, wArg02, wArg04) == 0x00)
        ;
}


Eq_194 __sbrk(struct Eq_22 * ds, uint32 dwArg02, union Eq_194 & dxOut)
{
    word16 bx_173;
    Eq_194 dx_50;
    word16 cx_172;
    uint32 dx_ax_125 = fn0800_0361(ds->t008D, 0x04, 0x00) + (uint32) ds->t008B + dwArg02;
    cup16 ax_30 = (word16) dx_ax_125;
    ci16 dx_33 = SLICE(dx_ax_125, word16, 16);
    Eq_194 dx_109;
    Eq_194 ax_110;
    if (dx_33 < 0x0F || dx_33 <= 0x0F && ax_30 <= ~0x00)
    {
        Eq_194 ax_48 = fn0800_0382(dwArg02, ds->t008B, ds->t008D, out cx_172, out dx_50, out bx_173);
        fn0800_05D4(ax_48, ds->w0089, dx_50, ds->w0087);
        struct Eq_2715 Eq_2744::* sp_52 = &Eq_2744::tBADDCAFE;
        bool C_68 = <invalid>;
        if (!C_68)
        {
            fn0800_05D4(ax_48, ds->w0091, dx_50, ds->w008F);
            bool Z_81 = <invalid>;
            bool C_80 = <invalid>;
            if (!(Z_81 | C_80))
            {
                Eq_194 ax_84 = ds->t008D;
                Eq_194 dx_85 = ds->t008B;
                selector ss;
                (ss->*sp_52).tFFFFFFFE.u0 = (wchar_t) dx_50;
                (ss->*sp_52).tFFFFFFFC.u0 = (wchar_t) ax_48;
                if (fn0800_12E4(ds, (ss->*sp_52).tFFFFFFFC.u0, (ss->*sp_52).tFFFFFFFE.u0) != 0x00)
                {
                    dx_109 = ax_84;
                    ax_110 = dx_85;
l0800_1423:
                    dxOut = dx_109;
                    return ax_110;
                }
            }
        }
    }
    dx_109.u0 = ~0x00;
    ax_110.u0 = ~0x00;
    goto l0800_1423;
}


void _gettext(struct Eq_22 * ds, Eq_1811 wArg04, Eq_1810 wArg06, Eq_1811 wArg08, Eq_1810 wArg0A, <unknown> Eq_2771::* wArg0C, struct Eq_2771 * wArg0E)
{
    Eq_1810 si_18 = wArg06;
    if (__VALIDATEXY(ds, wArg0A, wArg08, wArg06, wArg04) != 0x00)
    {
        cui16 ax_45 = wArg08 - wArg04;
        while (si_18 <= wArg0A)
        {
            __SCREENIO(ds, ax_45 + 0x01, __VPTR(ds, si_18, wArg04), wArg0C);
            wArg0C = (<unknown> Eq_2771::*) ((char *) wArg0C + (ax_45 + 0x01 << 0x01));
            si_18 = (word16) si_18 + 1;
        }
    }
}


byte _puttext(struct Eq_22 * ds, Eq_1811 wArg04, Eq_1810 wArg06, uint16 wArg08, Eq_1810 wArg0A, <unknown> Eq_2771::* wArg0C, struct Eq_2771 * wArg0E)
{
    cui16 ax_18 = wArg08 - wArg04;
    Eq_1810 si_23 = wArg06;
    while (si_23 <= wArg0A)
    {
        __SCREENIO(ds, ax_18 + 0x01, wArg0C, __VPTR(ds, si_23, wArg04));
        wArg0C = (<unknown> Eq_2771::*) ((char *) wArg0C + (ax_18 + 0x01 << 0x01));
        si_23 = (word16) si_23 + 1;
    }
    return 0x01;
}


Eq_194 _ioctl(struct Eq_22 * ds, word16 wArg06, ptr16 & cxOut)
{
    segptr32 ptrArg08;
    byte bArg06 = (byte) wArg06;
    msdos_unknown_2144();
    Eq_194 dx_31 = (word16) ptrArg08;
    Eq_194 ax_21 = SEQ(0x44, bArg06);
    bool C;
    if (!C)
    {
        if (wArg06 == 0x00)
            ax_21 = dx_31;
    }
    else
        ax_21 = __IOERROR(ds, ax_21);
    ptr16 wArg0C;
    cxOut = wArg0C;
    return ax_21;
}


void _movetext(struct Eq_22 * ds, Eq_1811 wArg04, Eq_1810 wArg06, Eq_1811 wArg08, Eq_1810 wArg0A, Eq_1811 wArg0C, Eq_1810 wArg0E)
{
    if (__VALIDATEXY(ds, wArg0A, wArg08, wArg06, wArg04) != 0x00 && __VALIDATEXY(ds, (word16) wArg0E + (wArg0A - wArg06), (word16) wArg0C + (wArg08 - wArg04), wArg0E, wArg0C) != 0x00)
    {
        Eq_1810 dx_77 = wArg06;
        Eq_1810 wLoc04_196 = wArg0A;
        ci16 wLoc06_197 = 0x01;
        if (wArg06 < wArg0E)
        {
            dx_77 = wArg0A;
            wLoc04_196 = wArg06;
            wLoc06_197 = ~0x00;
        }
        Eq_1810 di_152 = dx_77;
        while ((word16) wLoc04_196 + wLoc06_197 != di_152)
        {
            __SCREENIO(ds, wArg08 - wArg04 + 0x01, __VPTR(ds, di_152, wArg04), __VPTR(ds, (word16) wArg0E + (di_152 - wArg06), wArg0C));
            di_152 = (word16) di_152 + wLoc06_197;
        }
    }
}


void __REALCVT(struct Eq_22 * ds)
{
    ds->t05D8();
}


void fn0800_158A()
{
}


byte fn0800_15C2(byte al, struct Eq_22 * ds, word16 * ptrArg02, word16 * ptrArg06)
{
    uint16 ax;
    al = (byte) ax;
    word16 dx_12 = *ptrArg06;
    cu8 dl_35 = (byte) dx_12;
    byte dh_43 = SLICE(dx_12, byte, 8);
    uint16 ax_22 = ax;
    if (dx_12 != *ptrArg02)
    {
        word16 cx_90;
        word16 bx_91;
        ax_22 = __VideoInt(SEQ(0x02, al), out cx_90, out bx_91);
        *ptrArg02 = dx_12;
    }
    word16 dx_53 = SEQ(dh_43, dl_35 + 0x01);
    if (dl_35 + 0x01 >= ds->b057E)
        dx_53 = SEQ(dh_43 + 0x01, 0x00);
    *ptrArg06 = dx_53;
    return (byte) ax_22;
}


void fn0800_15F6(byte al, struct Eq_22 * ds, uint16 wArg02, uint16 * ptrArg04, uint16 * ptrArg08)
{
    word16 wArg0A = SLICE(ptrArg08, word16, 16);
    word16 wArg06 = SLICE(ptrArg04, word16, 16);
    __wherexy(al);
    uint16 di_105 = wArg02;
    word16 ax_33;
    if (wArg0A == ds->w0583)
        ax_33 = 0x01;
    else
        ax_33 = 0x00;
    uint16 * ptrArg08_271 = ptrArg08;
    if (ax_33 != 0x00)
        fn0800_158A();
    word16 ax_72;
    if (wArg06 == ds->w0583)
        ax_72 = 0x01;
    else
        ax_72 = 0x00;
    uint16 * ptrArg04_259 = ptrArg04;
    if (ax_72 != 0x00)
        fn0800_158A();
    byte al_212;
    while (true)
    {
        word16 wArg08_272 = (word16) ptrArg08_271;
        word16 wArg0A_310 = SLICE(ptrArg08_271, word16, 16);
        word16 wArg04_260 = (word16) ptrArg04_259;
        word16 wArg06_303 = SLICE(ptrArg04_259, word16, 16);
        --di_105;
        al_212 = (byte) di_105;
        if (di_105 == 0x00)
            break;
        uint16 si_121;
        <unknown> Eq_3020::* fp;
        if (ax_72 != 0x00)
        {
            word16 cx_328;
            word16 bx_329;
            si_121 = __VideoInt(SEQ(0x08, fn0800_15C2((byte) fp - 0x08, ds, fp - 8, fp - 6)), out cx_328, out bx_329);
        }
        else
        {
            si_121 = *ptrArg04_259;
            ptrArg04_259 = SEQ(wArg06_303, wArg04_260 + 0x02);
        }
        if (ax_33 != 0x00)
        {
            fn0800_15C2((byte) fp - 0x08, ds, fp - 8, fp - 4);
            word16 cx_330;
            word16 bx_331;
            __VideoInt(SEQ(0x09, (byte) si_121), out cx_330, out bx_331);
        }
        else
        {
            *ptrArg08_271 = si_121;
            ptrArg08_271 = SEQ(wArg0A_310, wArg08_272 + 0x02);
        }
    }
    word16 cx_326;
    word16 bx_327;
    __VideoInt(SEQ(0x02, al_212), out cx_326, out bx_327);
}


void __SCREENIO(struct Eq_22 * ds, uint16 wArg02, uint16 * dwArg04, uint16 * dwArg08)
{
    uint16 ax_13 = (uint16) ds->b057F;
    byte al_41 = (byte) ax_13;
    if (ax_13 == 0x00 && ds->w0585 != 0x00)
        __VRAM(ds, wArg02, dwArg04, dwArg08);
    else
        fn0800_15F6(al_41, ds, wArg02, dwArg04, dwArg08);
}


word16 __VALIDATEXY(struct Eq_22 * ds, Eq_1810 wArg02, Eq_1811 wArg04, Eq_1810 wArg06, Eq_1811 wArg08)
{
    Eq_1811 ax_21;
    ax_21.u1 = (uint16) ds->b057E;
    Eq_1810 ax_25;
    ax_25.u2 = (uint16) ds->b057D;
    word16 ax_39;
    if (wArg08 <= ax_21 && (wArg04 <= ax_21 && (wArg08 <= wArg04 && (wArg06 <= ax_25 && (wArg02 <= ax_25 && wArg06 <= wArg02)))))
        ax_39 = 0x01;
    else
        ax_39 = 0x00;
    return ax_39;
}


void fn0800_173F(struct Eq_22 * ds, Eq_3103 wArg02, Eq_3103 wArg04, word16 * ptrArg06)
{
    Eq_3103 dx_13 = wArg04;
    word16 cx_32 = SEQ(ds->b057A, 0x20);
    while (true)
    {
        word16 wArg06_37 = (word16) ptrArg06;
        word16 wArg08_42 = SLICE(ptrArg06, word16, 16);
        if (dx_13 > wArg02)
            break;
        *ptrArg06 = cx_32;
        dx_13 = (word16) dx_13.u0 + 1;
        ptrArg06 = SEQ(wArg08_42, wArg06_37 + 0x02);
    }
}


byte __SCROLL(struct Eq_22 * ds, byte bArg02, byte bArg04, byte bArg06, byte bArg08, byte bArg0A, byte bArg0C)
{
    uint16 ax_286;
    if ((uint16) ds->b057F != 0x00 || (ds->w0585 == 0x00 || bArg02 != 0x01))
    {
        word16 cx_413;
        word16 bx_414;
        ax_286 = __VideoInt(SEQ(bArg0C, bArg02), out cx_413, out bx_414);
    }
    else
    {
        <unknown> Eq_2771::* fp;
        struct Eq_2771 * ss;
        Eq_1810 wLocA8_353;
        uint16 wLocAA_354;
        byte al_151;
        if (bArg0C != 0x06)
        {
            _movetext(ds, (uint16) (bArg0A + 0x01), (uint16) (bArg08 + 0x01), (uint16) (bArg06 + 0x01), (uint16) (bArg04 + 0x01) - 0x01, (uint16) (bArg0A + 0x01), (uint16) (bArg08 + 0x01) + 0x01);
            _gettext(ds, (uint16) (bArg0A + 0x01), (uint16) (bArg08 + 0x01), (uint16) (bArg0A + 0x01), (uint16) (bArg08 + 0x01), fp - 0xA2, ss);
            fn0800_173F(ds, (uint16) (bArg06 + 0x01), (uint16) (bArg0A + 0x01), fp - 0xA2);
            wLocA8_353.u2 = (uint16) (bArg08 + 0x01);
            wLocAA_354 = (uint16) (bArg06 + 0x01);
            al_151 = bArg08 + 0x01;
        }
        else
        {
            _movetext(ds, (uint16) (bArg0A + 0x01), (uint16) (bArg08 + 0x01) + 0x01, (uint16) (bArg06 + 0x01), (uint16) (bArg04 + 0x01), (uint16) (bArg0A + 0x01), (uint16) (bArg08 + 0x01));
            _gettext(ds, (uint16) (bArg0A + 0x01), (uint16) (bArg04 + 0x01), (uint16) (bArg0A + 0x01), (uint16) (bArg04 + 0x01), fp - 0xA2, ss);
            fn0800_173F(ds, (uint16) (bArg06 + 0x01), (uint16) (bArg0A + 0x01), fp - 0xA2);
            wLocA8_353.u2 = (uint16) (bArg04 + 0x01);
            wLocAA_354 = (uint16) (bArg06 + 0x01);
            al_151 = bArg04 + 0x01;
        }
        ax_286 = (uint16) _puttext(ds, (uint16) (bArg0A + 0x01), (uint16) al_151, wLocAA_354, wLocA8_353, fp - 0xA2, ss);
    }
    return (byte) ax_286;
}


word16 _setblock(byte al, struct Eq_22 * ds, struct Eq_2562 * psegArg04, cu16 wArg06)
{
    Eq_194 ax_18 = SEQ(0x4A, al);
    word16 bx_12;
    word16 ax_27;
    if (!msdos_resize_memory_block(psegArg04, wArg06, out bx_12))
        ax_27 = ~0x00;
    else
    {
        __IOERROR(ds, ax_18);
        ax_27 = bx_12;
    }
    return ax_27;
}


void __wherexy(byte al)
{
    word16 cx_18;
    word16 bx_19;
    __VideoInt(SEQ(0x03, al), out cx_18, out bx_19);
}


Eq_194 __chmod(struct Eq_22 * ds, byte bArg08, union Eq_194 & cxOut)
{
    Eq_194 wArg0A;
    __syscall<byte>(33);
    Eq_194 cx_12 = wArg0A;
    Eq_194 ax_20 = SEQ(0x43, bArg08);
    bool C;
    Eq_194 ax_22;
    if (!C)
    {
        cx_12 = ax_20;
        ax_22 = wArg0A;
    }
    else
        ax_22 = __IOERROR(ds, ax_20);
    cxOut = cx_12;
    return ax_22;
}


void _close(byte al, struct Eq_22 * ds, Eq_194 wArg04)
{
    if (wArg04 >= ds->t0478)
        __IOERROR(ds, 0x06);
    else
    {
        (*((char *) &ds->t0478 + 2))[wArg04] = 0x00;
        __close(al, ds, wArg04);
    }
}


void __close(byte al, struct Eq_22 * ds, Eq_194 wArg04)
{
    Eq_194 ax_14 = SEQ(0x3E, al);
    if (!msdos_close_file(wArg04))
        (*((char *) &ds->t0478 + 2))[wArg04] = 0x00;
    else
        __IOERROR(ds, ax_14);
}


Eq_194 _eof(struct Eq_22 * ds, Eq_194 wArg04)
{
    word16 dx_22;
    Eq_194 wLoc08_120;
    if (wArg04 >= ds->t0478)
    {
        wLoc08_120.u0 = 0x06;
        goto l0800_1B0F;
    }
    Eq_194 ax_129;
    if (((*((char *) &ds->t0478 + 2))[wArg04] & 0x0200) != 0x00)
    {
l0800_1AC1:
        ax_129.u0 = 0x01;
        return ax_129;
    }
    Eq_194 ax_20;
    ax_20.u0 = 0x4400;
    byte dl_24 = (byte) dx_22;
    if (!msdos_ioctl_get_device_info(wArg04, out dx_22))
    {
        if ((dl_24 & 0x80) != 0x00)
        {
l0800_1B0A:
            ax_129.u0 = 0x00;
            return ax_129;
        }
        int32 * dx_ax_36;
        ax_20 = (word16) dx_ax_36;
        if (!msdos_set_file_position(wArg04, 0x00, 0x01, out dx_ax_36))
        {
            int32 * dx_ax_51;
            ax_20 = (word16) dx_ax_51;
            cup16 dx_55 = SLICE(dx_ax_51, word16, 16);
            if (!msdos_set_file_position(wArg04, 0x00, 0x02, out dx_ax_51))
            {
                int32 * dx_ax_64;
                ax_20 = (word16) dx_ax_64;
                cup16 dx_72 = SLICE(dx_ax_64, word16, 16);
                if (!msdos_set_file_position(wArg04, dx_ax_36, 0x00, out dx_ax_64))
                {
                    if (dx_72 >= dx_55 && (dx_72 > dx_55 || ax_20 >= ax_20))
                        goto l0800_1AC1;
                    goto l0800_1B0A;
                }
            }
        }
    }
    wLoc08_120 = ax_20;
l0800_1B0F:
    ax_129 = __IOERROR(ds, wLoc08_120);
    return ax_129;
}


word16 _fclose(struct Eq_22 * ds, Eq_3416 ptrArg04, struct Eq_22 & dsOut)
{
    ptr16 wArg04_183 = (word16) ptrArg04;
    word16 bx_114 = (word16) ptrArg04;
    if (ptrArg04.u1->ptr0012 == wArg04_183)
    {
        if (ptrArg04.u1->t0006.u0 != 0x00)
        {
            if (ptrArg04.u1->w0000 < 0x00 && _fflush(ds, ptrArg04, out bx_114) != 0x00)
                goto l0800_1BBC;
            if ((ptrArg04.u1->w0002 & 0x04) != 0x00)
                ds = _farfree(ds, ptrArg04.u1->t000A.u0);
        }
        if (ptrArg04.u1->b0004 >= 0x00)
        {
            Eq_194 ax_92;
            ax_92.u1 = (int16) ptrArg04.u1->b0004;
            _close((byte) ax_92, ds, ax_92);
        }
        ptrArg04.u1->w0002 = 0x00;
        ptrArg04.u1->t0006.u0 = 0x00;
        ptrArg04.u1->w0000 = 0x00;
        ptrArg04.u1->b0004 = ~0x00;
        bx_114 = (word16) ptrArg04;
        if (ptrArg04.u1->w0010 != 0x00)
        {
            _unlink(ds, __MKNAME(ds, ptrArg04.u1->w0010, 0x00, 0x00, 0<32>));
            ptrArg04.u1->w0010 = 0x00;
            bx_114 = (word16) ptrArg04;
        }
    }
l0800_1BBC:
    dsOut = ds;
    return bx_114;
}


word16 _fflush(struct Eq_22 * ds, Eq_3416 ptrArg04, ptr16 & bxOut)
{
    ptr16 wArg04 = (word16) ptrArg04;
    Eq_194 wArg06;
    wArg06.u4 = SLICE(ptrArg04, word16, 16);
    ptr16 bx_111;
    if (ptrArg04 == 0x00)
    {
        bx_111 = _flushall(ds);
        goto l0800_1C89;
    }
    word16 ax_108;
    if (ptrArg04.u1->ptr0012 == wArg04)
    {
        bx_111 = wArg04;
        if (ptrArg04.u1->w0000 >= 0x00)
        {
            if ((ptrArg04.u1->w0002 & 0x08) != 0x00 || ((ptrArg04.u1)->t000E).u0 == wArg06 && ((ptrArg04.u1)->t000C).u0 == wArg04 + 0x05)
            {
                ptrArg04.u1->w0000 = 0x00;
                bx_111 = wArg04;
                if (ptrArg04.u1->t000E.u0 == wArg06 && ((ptrArg04.u1)->t000C).u0 == wArg04 + 0x05)
                {
                    Eq_484 dx_105;
                    dx_105.u0 = ptrArg04.u1->t0008.u0;
                    ptrArg04.u1->t000E.u0 = ptrArg04.u1->t000A.u0;
                    ptrArg04.u1->t000C.u0 = (cui16) dx_105;
                }
            }
        }
        else
        {
            ci16 ax_33 = ptrArg04.u1->t0006.u0 + (ptrArg04.u1)->w0000;
            ptrArg04.u1->w0000 -= ax_33 + 0x01;
            Eq_194 ax_41;
            ax_41.u0 = ptrArg04.u1->t000A.u0;
            Eq_484 dx_42;
            dx_42.u0 = ptrArg04.u1->t0008.u0;
            ptrArg04.u1->t000E.u0 = (wchar_t) ax_41;
            ptrArg04.u1->t000C.u0 = (cui16) dx_42;
            word16 cx_206;
            if (___write(ds, (int16) ptrArg04.u1->b0004, SEQ(ax_41, dx_42), ax_33 + 0x01, out cx_206, out bx_111) != ax_33 + 0x01)
            {
                bx_111 = wArg04;
                if ((ptrArg04.u1->w0002 & 0x0200) == 0x00)
                {
                    ptrArg04.u1->w0002 |= 0x10;
                    goto l0800_1BE1;
                }
            }
        }
l0800_1C89:
        ax_108 = 0x00;
        goto l0800_1C8B;
    }
    else
    {
l0800_1BE1:
        bx_111 = wArg04;
        ax_108 = ~0x00;
l0800_1C8B:
        bxOut = bx_111;
        return ax_108;
    }
}


word16 _fflush(struct Eq_22 * ds, Eq_3416 ptrArg04, ptr16 & bxOut)
{
    ptr16 wArg04 = (word16) ptrArg04;
    Eq_194 wArg06;
    wArg06.u4 = SLICE(ptrArg04, word16, 16);
    ptr16 bx_111;
    if (ptrArg04 == 0x00)
    {
        bx_111 = _flushall(ds);
        goto l0800_1C89;
    }
    word16 ax_108;
    if (ptrArg04.u1->ptr0012 == wArg04)
    {
        bx_111 = wArg04;
        if (ptrArg04.u1->w0000 >= 0x00)
        {
            if ((ptrArg04.u1->w0002 & 0x08) != 0x00 || ((ptrArg04.u1)->t000E).u0 == wArg06 && ((ptrArg04.u1)->t000C).u0 == wArg04 + 0x05)
            {
                ptrArg04.u1->w0000 = 0x00;
                bx_111 = wArg04;
                if (ptrArg04.u1->t000E.u0 == wArg06 && ((ptrArg04.u1)->t000C).u0 == wArg04 + 0x05)
                {
                    Eq_484 dx_105;
                    dx_105.u0 = ptrArg04.u1->t0008.u0;
                    ptrArg04.u1->t000E.u0 = ptrArg04.u1->t000A.u0;
                    ptrArg04.u1->t000C.u0 = (cui16) dx_105;
                }
            }
        }
        else
        {
            ci16 ax_33 = ptrArg04.u1->t0006.u0 + (ptrArg04.u1)->w0000;
            ptrArg04.u1->w0000 -= ax_33 + 0x01;
            Eq_194 ax_41;
            ax_41.u0 = ptrArg04.u1->t000A.u0;
            Eq_484 dx_42;
            dx_42.u0 = ptrArg04.u1->t0008.u0;
            ptrArg04.u1->t000E.u0 = (wchar_t) ax_41;
            ptrArg04.u1->t000C.u0 = (cui16) dx_42;
            word16 cx_206;
            if (___write(ds, (int16) ptrArg04.u1->b0004, SEQ(ax_41, dx_42), ax_33 + 0x01, out cx_206, out bx_111) != ax_33 + 0x01)
            {
                bx_111 = wArg04;
                if ((ptrArg04.u1->w0002 & 0x0200) == 0x00)
                {
                    ptrArg04.u1->w0002 |= 0x10;
                    goto l0800_1BE1;
                }
            }
        }
l0800_1C89:
        ax_108 = 0x00;
        goto l0800_1C8B;
    }
    else
    {
l0800_1BE1:
        bx_111 = wArg04;
        ax_108 = ~0x00;
l0800_1C8B:
        bxOut = bx_111;
        return ax_108;
    }
}


Eq_3416 _flushall(struct Eq_22 * ds)
{
    Eq_194 si_21 = ds->t0478;
    struct Eq_3651 * ptrLoc06_86 = (struct Eq_3651 *) ((char *) &ds->ptr02E4 + 4);
    Eq_3416 bx;
    while (true)
    {
        word16 wLoc04_87 = SLICE(ptrLoc06_86, word16, 16);
        word16 wLoc06_89 = (word16) ptrLoc06_86;
        --si_21;
        if (si_21 == 0x00)
            break;
        bx = (word16) ptrLoc06_86;
        if ((ptrLoc06_86->w0002 & 0x03) != 0x00)
            _fflush(ds, bx, out bx);
        ptrLoc06_86 = SEQ(wLoc04_87, wLoc06_89 + 0x14);
    }
    return bx;
}


cui16 fn0800_1CD0(struct Eq_22 * ds, word16 * ptrArg02, cui16 * ptrArg06, union Eq_3680 * ptrArg0A, union Eq_3680 & clOut)
{
    word16 wArg0A_169 = (word16) ptrArg0A;
    word16 wArg0C_172 = SLICE(ptrArg0A, word16, 16);
    word16 di_141 = 0x00;
    Eq_3680 cl_21;
    cl_21.u0 = ptrArg0A->u0;
    union Eq_3680 * ptrArg0A_173 = SEQ(wArg0C_172, wArg0A_169 + 0x01);
    cui16 dx_33;
    cui16 si_111;
    cui16 ax_148;
    if (cl_21 == 114)
    {
        dx_33 = 0x01;
        si_111 = 0x01;
    }
    else
    {
        if (cl_21 == 0x77)
            dx_33 = 770;
        else
        {
            if (cl_21 != 0x61)
            {
                ax_148 = 0x00;
l0800_1D86:
                clOut = cl_21;
                return ax_148;
            }
            dx_33 = 0x0902;
        }
        di_141 = 0x80;
        si_111 = 0x02;
    }
    cl_21.u0 = ptrArg0A_173->u0;
    union Eq_3680 * ptrArg0A_178 = SEQ(wArg0C_172, wArg0A_169 + 0x02);
    if (cl_21 == 0x2B || ptrArg0A_178->u0 == 0x2B && (cl_21 == 116 || cl_21 == 0x62))
    {
        if (cl_21 == 0x2B)
            cl_21.u0 = ptrArg0A_178->u0;
        dx_33 = dx_33 & ~0x03 | 0x04;
        di_141 = 0x0180;
        si_111 = 0x03;
    }
    cui16 dx_100;
    if (cl_21 == 116)
    {
        dx_100 = dx_33 | 0x4000;
        goto l0800_1D6C;
    }
    if (cl_21 == 0x62)
        dx_100 = dx_33 | 0x8000;
    else
    {
        dx_100 = dx_33 | ds->w04A2 & 0xC000;
        if ((dx_100 & 0x8000) == 0x00)
            goto l0800_1D6C;
    }
    si_111 |= 0x40;
l0800_1D6C:
    selector seg0800;
    ds->ptr02E2 = seg0800;
    ds->t02E0.u0 = 11722;
    *ptrArg06 = dx_100;
    *ptrArg02 = di_141;
    ax_148 = si_111;
    goto l0800_1D86;
}


word16 __OPENFP(struct Eq_22 * ds, cui16 wArg02, union Eq_3680 * dwArg04, char * dwArg08, Eq_3416 ptrArg0C, ptr16 & cxOut, ptr16 & dxOut, ptr16 & bxOut, struct Eq_22 & dsOut)
{
    cui16 wLoc06;
    cui16 wLoc04;
    byte ch;
    byte cl_30;
    <unknown> Eq_3788::* fp;
    ptr16 wArg0E_258 = SLICE(ptrArg0C, word16, 16);
    cui16 ax_28 = fn0800_1CD0(ds, fp - 6, fp - 4, dwArg04, out cl_30);
    ptrArg0C.u1->w0002 = ax_28;
    ptr16 cx_165 = SEQ(ch, cl_30);
    ptr16 bx_180;
    if (ax_28 == 0x00)
    {
l0800_1DDA:
        ptrArg0C.u1->b0004 = ~0x00;
        ptrArg0C.u1->w0002 = 0x00;
        bx_180 = (word16) ptrArg0C;
        goto l0800_1DE8;
    }
    else
    {
        if (ptrArg0C.u1->b0004 < 0x00)
        {
            ci8 al_77 = _open(ds, dwArg08, wLoc04 | wArg02, wLoc06, out cx_165);
            ptrArg0C.u1->b0004 = al_77;
            if (al_77 < 0x00)
                goto l0800_1DDA;
        }
        if (_isatty((int16) ptrArg0C.u1->b0004) != 0x00)
            ptrArg0C.u1->w0002 |= 0x0200;
        ci16 ax_133;
        if ((ptrArg0C.u1->w0002 & 0x0200) != 0x00)
            ax_133 = 0x01;
        else
            ax_133 = 0x00;
        ptr16 wArg0E_266 = SLICE(ptrArg0C, word16, 16);
        word16 wArg0C_267 = (word16) ptrArg0C;
        ptr16 dx_182;
        word16 ax_183;
        if (_setvbuf(ds, ptrArg0C, 0x00, 0x00, ax_133, 0x0200, out cx_165, out ds) == 0x00)
        {
            ptrArg0C.u1->w0010 = 0x00;
            bx_180 = (word16) ptrArg0C;
            dx_182 = wArg0E_266;
            ax_183 = wArg0C_267;
            goto l0800_1E55;
        }
        bx_180 = _fclose(ds, ptrArg0C, out ds);
        cx_165 = wArg0E_258;
l0800_1DE8:
        dx_182 = 0x00;
        ax_183 = 0x00;
l0800_1E55:
        cxOut = cx_165;
        dxOut = dx_182;
        bxOut = bx_180;
        dsOut = ds;
        return ax_183;
    }
}


word32 __GETFP(struct Eq_22 * ds, ptr16 & bxOut)
{
    struct Eq_3907 * ptrLoc06_62 = (struct Eq_3907 *) ((char *) &ds->ptr02E4 + 4);
    Eq_3921 wLoc06_63;
    do
    {
        wLoc06_63 = (word16) ptrLoc06_62;
        word16 wLoc04_67 = SLICE(ptrLoc06_62, word16, 16);
        if (ptrLoc06_62->b0004 < 0x00)
            break;
        ptrLoc06_62 = SEQ(wLoc04_67, (word16) wLoc06_63.u1 + 20);
    } while (wLoc06_63 < (word16) (ds->t0478 *s16 0x14) + 744);
    word16 wLoc04_69 = SLICE(ptrLoc06_62, word16, 16);
    word16 wLoc06_70 = (word16) ptrLoc06_62;
    ptr16 bx_43 = (word16) ptrLoc06_62;
    word16 dx_46;
    word16 ax_47;
    if (ptrLoc06_62->b0004 >= 0x00)
    {
        dx_46 = 0x00;
        ax_47 = 0x00;
    }
    else
    {
        dx_46 = wLoc04_69;
        ax_47 = wLoc06_70;
    }
    bxOut = bx_43;
    return SEQ(dx_46, ax_47);
}


word32 _fopen(struct Eq_22 * ds, char * dwArg04, union Eq_3680 * dwArg08, ptr16 & cxOut, ptr16 & bxOut, struct Eq_22 & dsOut)
{
    ptr16 bx_13;
    Eq_3416 dx_ax_110 = __GETFP(ds, out bx_13);
    word16 dx_54;
    word16 ax_51;
    ptr16 cx;
    if (dx_ax_110 == 0x00)
    {
        dx_54 = 0x00;
        ax_51 = 0x00;
    }
    else
        ax_51 = __OPENFP(ds, 0x00, dwArg08, dwArg04, dx_ax_110, out cx, out dx_54, out bx_13, out ds);
    cxOut = cx;
    bxOut = bx_13;
    dsOut = ds;
    return SEQ(dx_54, ax_51);
}


Eq_194 fn0800_1EDA(Eq_194 dx, struct Eq_22 * ds, Eq_3416 ptrArg02, Eq_194 wArg06, Eq_194 ptrArg08, union Eq_554 & chOut)
{
    Eq_194 ptrArg08_238 = ptrArg08;
    Eq_194 di_128;
    Eq_194 ax_168;
    do
    {
        word16 wArg08_252 = (word16) ptrArg08_238;
        word16 wArg0A_248 = SLICE(ptrArg08_238, word16, 16);
        if (wArg06 == 0x00)
            goto l0800_1FC5;
        Eq_194 v11_20;
        v11_20.u4 = (word32) wArg06 + 1;
        wArg06 = v11_20;
        Eq_194 ax_31;
        if (ptrArg02.u1->t0006.u0 > v11_20)
            ax_31 = v11_20;
        else
            ax_31.u0 = ptrArg02.u1->t0006.u0;
        Eq_194 di_34 = ax_31;
        if ((ptrArg02.u1->w0002 & 0x40) == 0x00 || (((ptrArg02.u1)->t0006).u0 == 0x00 || (((ptrArg02.u1)->t0006).u0 >= v11_20 || (ptrArg02.u1)->w0000 != 0x00)))
        {
            while (true)
            {
                Eq_194 v22_55 = wArg06 - 0x01;
                word16 wArg08_239 = (word16) ptrArg08_238;
                word16 wArg0A_293 = SLICE(ptrArg08_238, word16, 16);
                wArg06 = v22_55;
                if (v22_55 == 0x00)
                    break;
                --di_34;
                if (di_34 == 0x00)
                    break;
                ci16 v23_70 = ptrArg02.u1->w0000 - 0x01;
                ptrArg02.u1->w0000 = v23_70;
                Eq_194 ax_104;
                if (v23_70 >= 0x00)
                {
                    Eq_194 ax_96;
                    ax_96.u0 = ptrArg02.u1->t000E.u0;
                    Eq_484 si_97;
                    si_97.u0 = ptrArg02.u1->t000C.u0;
                    ptrArg02.u1->t000C.u0 = (cui16) (ptrArg02.u1->t000C.u0 + 0x01);
                    ax_104.u2 = (uint16) (ax_96->*si_97.u1);
                }
                else
                    ax_104 = __fgetc(ds, ptrArg02);
                dx = ax_104;
                Eq_2234 dl_113 = (byte) ax_104;
                if (ax_104 == ~0x00)
                    break;
                *ptrArg08_238.u4 = (char) dl_113;
                ptrArg08_238 = SEQ(wArg0A_293, wArg08_239 + 0x01);
            }
            if (dx != ~0x00)
                continue;
            goto l0800_1F66;
        }
        di_128.u0 = 0x00;
        for (wArg06 = v11_20 - 0x01; ptrArg02.u1->t0006.u0 <= wArg06; wArg06 -= ptrArg02.u1->t0006.u0)
            di_128.u4 += ptrArg02.u1->t0006.u0;
        byte ch_351;
        ax_168 = __read(ds, (int16) ptrArg02.u1->b0004, ptrArg08_238, di_128, out ch_351);
        dx = ax_168;
        ptrArg08_238 = SEQ(wArg0A_248, ax_168.u4 + wArg08_252);
    } while (ax_168 == di_128);
    wArg06.u4 += di_128 - ax_168;
l0800_1F66:
    ptrArg02.u1->w0002 |= 0x20;
l0800_1FC5:
    chOut.u1 = (<unknown>*) <invalid>;
    return wArg06;
}


byte _fread(struct Eq_22 * ds, Eq_194 dwArg04, Eq_194 wArg08, uint16 wArg0A, Eq_3416 dwArg0C, union Eq_194 & diOut)
{
    Eq_194 dx_30;
    mp16 fp;
    Eq_194 wArg06;
    wArg06.u4 = SLICE(dwArg04, word16, 16);
    Eq_194 wArg04 = (word16) dwArg04;
    word16 wArg0E = SLICE(dwArg0C, word16, 16);
    Eq_3416 wArg0C = (word16) dwArg0C;
    struct Eq_2715 Eq_2744::* sp_109 = fp - 0x0A;
    struct Eq_2715 Eq_2744::* cx;
    selector ss;
    if (wArg08 != 0x00)
    {
        Eq_194 ax_29 = fn0800_05BD(wArg0A, 0x00, 0x00, wArg08, out cx, out dx_30);
        if (dx_30 <= 0x01 && (dx_30 < 0x01 || Test(ULT,false)))
        {
            byte ch_277;
            fn0800_1EDA(dx_30, ds, dwArg0C, ax_29, dwArg04, out ch_277);
            cx = &Eq_2744::tBADDCAFE;
            sp_109 = fp - 0x0A;
        }
        else
        {
            word16 si_50 = wArg0A + 0x01;
            while (true)
            {
                --si_50;
                if (si_50 == 0x00)
                    break;
                struct Eq_4180 Eq_2744::* sp_65 = sp_109 - 2;
                (ss->*sp_65).t0000.u0 = (wchar_t) wArg06;
                (ss->*sp_65).tFFFFFFFE.u0 = (wchar_t) wArg04;
                (ss->*sp_65).tFFFFFFFC.u0 = (wchar_t) wArg08;
                (ss->*sp_65).wFFFFFFFA = wArg0E;
                (ss->*sp_65).tFFFFFFF8.u0 = (word16) wArg0C;
                cx = &Eq_2744::tBADDCAFE;
                byte ch_278;
                if (fn0800_1EDA(dx_30, ds, (ss->*sp_65).tFFFFFFF8.u1, (ss->*sp_65).tFFFFFFFC.u0, (ss->*sp_65).tFFFFFFFE.u4, out ch_278) != 0x00)
                    break;
                sp_109 = &Eq_2744::tBADDCAFE;
                Eq_194 dx_107;
                wArg06 = dx_107;
                word16 bx_279;
                wArg04 = fn0800_0382((uint32) wArg08, wArg04, wArg06, out cx, out dx_107, out bx_279);
                dx_30 = dx_107;
            }
        }
    }
    diOut.u0 = (ss->*sp_109).t0000.u0;
    return SLICE(cx, byte, 8);
}


ui16 fn0800_205C(Eq_3416 ptrArg02)
{
    byte * ptrLoc06_163;
    ui16 cx_111;
    ui16 si_29;
    if (ptrArg02.u1->w0000 < 0x00)
    {
        word16 cx_31 = ptrArg02.u1->t0006.u0 + (ptrArg02.u1)->w0000;
        cx_111 = cx_31 + 0x01;
        si_29 = cx_31 + 0x01;
    }
    else
    {
        int32 dx_ax_22 = (int32) ptrArg02.u1->w0000;
        ui16 dx_24 = SLICE(dx_ax_22, word16, 16);
        ui16 ax_26 = ((word16) dx_ax_22 ^ dx_24) - dx_24;
        cx_111 = ax_26;
        si_29 = ax_26;
    }
    if ((ptrArg02.u1->w0002 & 0x40) == 0x00)
    {
        Eq_484 dx_50;
        dx_50.u0 = ptrArg02.u1->t000C.u0;
        Eq_194 ax_49;
        ax_49.u0 = ptrArg02.u1->t000E.u0;
        Eq_484 wLoc06_117 = dx_50;
        byte * ptrLoc06_118 = SEQ(ax_49, dx_50);
        if (ptrArg02.u1->w0000 < 0x00)
        {
            while (true)
            {
                --cx_111;
                if (cx_111 == 0x00)
                    break;
                Eq_484 v22_87 = wLoc06_117 - 0x01;
                wLoc06_117 = v22_87;
                if (*SEQ(ax_49, v22_87) == 0x0A)
                    ++si_29;
            }
        }
        else
        {
            while (true)
            {
                word16 wLoc06_119 = (word16) ptrLoc06_163;
                word16 wLoc04_134 = SLICE(ptrLoc06_163, word16, 16);
                --cx_111;
                if (cx_111 == 0x00)
                    break;
                ptrLoc06_118 = SEQ(wLoc04_134, wLoc06_119 + 0x01);
                ptrLoc06_163 = ptrLoc06_118;
                if (*ptrLoc06_163 == 0x0A)
                    ++si_29;
            }
        }
    }
    return si_29;
}


word16 _fseek(struct Eq_22 * ds, Eq_3416 ptrArg04, int32 * dwArg08, word16 wArg0C)
{
    word16 dx_106;
    word16 wArg06 = SLICE(ptrArg04, word16, 16);
    word16 cx_105 = wArg06;
    word16 bx_205;
    if (_fflush(ds, ptrArg04, out bx_205) == 0x00)
    {
        if (wArg0C == 0x01 && (ptrArg04.u1)->w0000 > 0x00)
            dwArg08 -= (int32) fn0800_205C(ptrArg04);
        ptrArg04.u1->w0002 &= ~0x01A0;
        ptrArg04.u1->w0000 = 0x00;
        Eq_484 dx_85;
        dx_85.u0 = ptrArg04.u1->t0008.u0;
        ptrArg04.u1->t000E.u0 = ptrArg04.u1->t000A.u0;
        ptrArg04.u1->t000C.u0 = (cui16) dx_85;
        word16 ax_103 = _lseek(ds, (int16) ptrArg04.u1->b0004, dwArg08, (byte) wArg0C, out cx_105, out dx_106);
        if (dx_106 != ~0x00 || ax_103 != ~0x00)
            ;
    }
    return cx_105;
}


word16 _fwrite(struct Eq_22 * ds, Eq_194 dwArg04, Eq_194 wArg08, uint16 wArg0A, Eq_3416 dwArg0C, union Eq_194 & dxOut, union Eq_194 & bxOut, ptr16 & siOut, union Eq_194 & diOut)
{
    Eq_194 dx;
    word16 cx;
    mp16 fp;
    Eq_194 wArg06;
    wArg06.u4 = SLICE(dwArg04, word16, 16);
    Eq_194 wArg04 = (word16) dwArg04;
    word16 wArg0E = SLICE(dwArg0C, word16, 16);
    Eq_3416 wArg0C = (word16) dwArg0C;
    struct Eq_2715 Eq_2744::* sp_103 = fp - 0x0A;
    if (wArg08 != 0x00)
    {
        Eq_194 bx = wArg08;
        Eq_194 ax_29 = fn0800_05BD(wArg0A, 0x00, 0x00, wArg08, out cx, out dx);
        selector ss;
        if (dx <= 0x01 && (dx < 0x01 || Test(ULT,false)))
        {
            sp_103 = fp - 0x0A;
            dx = (uint32) __FPUTN(ds, dwArg0C, ax_29, dwArg04, out cx, out bx) % wArg08;
l0800_223B:
            Eq_194 di_168;
            di_168.u0 = (ss->*sp_103).t0000.u0;
            ptr16 si_170 = (ss->*sp_103).ptr0002;
            dxOut = dx;
            bxOut = bx;
            siOut = si_170;
            diOut = di_168;
            return cx;
        }
        uint16 si_111 = 0x00;
        if (wArg0A > 0x00)
        {
            do
            {
                struct Eq_4452 Eq_2744::* sp_57 = sp_103 - 2;
                (ss->*sp_57).t0000.u0 = (wchar_t) wArg06;
                (ss->*sp_57).tFFFFFFFE.u0 = (wchar_t) wArg04;
                (ss->*sp_57).tFFFFFFFC.u0 = (wchar_t) wArg08;
                (ss->*sp_57).wFFFFFFFA = wArg0E;
                (ss->*sp_57).tFFFFFFF8.u0 = (word16) wArg0C;
                Eq_194 ax_72 = __FPUTN(ds, (ss->*sp_57).tFFFFFFF8.u1, (ss->*sp_57).tFFFFFFFC.u0, (ss->*sp_57).tFFFFFFFE.u4, out cx, out bx);
                sp_103 = (struct Eq_2715 Eq_2744::*) &sp_57->ptr0002;
                dx.u0 = 0x00;
                if (false || ax_72 != wArg08)
                    goto l0800_223B;
                sp_103 = &Eq_2744::tBADDCAFE;
                wArg06 = dx;
                wArg04 = fn0800_0382((uint32) wArg08, wArg04, wArg06, out cx, out dx, out bx);
                ++si_111;
            } while (si_111 < wArg0A);
        }
    }
    goto l0800_223B;
}


void fn0800_2241(struct Eq_22 * ds)
{
    word16 si_14 = 0x14;
    Eq_3416 ptrLoc06_74;
    ptrLoc06_74.u1 = (char *) &ds->ptr02E4 + 4;
    while (true)
    {
        word16 wLoc04_75 = SLICE(ptrLoc06_74, word16, 16);
        word16 wLoc06_80 = (word16) ptrLoc06_74;
        --si_14;
        if (si_14 == 0x00)
            break;
        if ((ptrLoc06_74.u1->w0002 & 0x0300) == 0x0300)
        {
            word16 bx_107;
            _fflush(ds, ptrLoc06_74, out bx_107);
        }
        ptrLoc06_74 = SEQ(wLoc04_75, wLoc06_80 + 0x14);
    }
}


word16 fn0800_227F(struct Eq_22 * ds, Eq_3416 ptrArg02)
{
    if ((ptrArg02.u1->w0002 & 0x0200) != 0x00)
        fn0800_2241(ds);
    Eq_194 v13_32;
    v13_32.u0 = ptrArg02.u1->t0006.u0;
    Eq_194 ax_36;
    ax_36.u0 = ptrArg02.u1->t000A.u0;
    Eq_484 dx_37;
    dx_37.u0 = ptrArg02.u1->t0008.u0;
    ptrArg02.u1->t000E.u0 = (wchar_t) ax_36;
    ptrArg02.u1->t000C.u0 = (cui16) dx_37;
    ci16 ax_49 = ___read(ds, (int16) ptrArg02.u1->b0004, SEQ(ax_36, dx_37), v13_32);
    ptrArg02.u1->w0000 = ax_49;
    word16 ax_118;
    if (ax_49 > 0x00)
    {
        ptrArg02.u1->w0002 &= ~0x20;
        ax_118 = 0x00;
    }
    else
    {
        if (ptrArg02.u1->w0000 == 0x00)
            ptrArg02.u1->w0002 = ptrArg02.u1->w0002 & 65151 | 0x20;
        else
        {
            ptrArg02.u1->w0000 = 0x00;
            ptrArg02.u1->w0002 |= 0x10;
        }
        ax_118 = ~0x00;
    }
    return ax_118;
}


word16 __fgetc(struct Eq_22 * ds, Eq_3416 ptrArg04)
{
    ++ptrArg04.u1->w0000;
    return _fgetc(ds, ptrArg04);
}


uint16 _fgetc(struct Eq_22 * ds, Eq_3416 ptrArg04)
{
    uint16 ax_203;
    if (ptrArg04 == 0x00)
    {
l0800_231B:
        ax_203 = ~0x00;
        return ax_203;
    }
    byte al_122;
    if (ptrArg04.u1->w0000 <= 0x00)
    {
        if (ptrArg04.u1->w0000 < 0x00 || (((ptrArg04.u1)->w0002 & 0x0110) != 0x00 || ((ptrArg04.u1)->w0002 & 0x01) == 0x00))
        {
l0800_23BC:
            ptrArg04.u1->w0002 |= 0x10;
            goto l0800_231B;
        }
        ptrArg04.u1->w0002 |= 0x80;
        if (ptrArg04.u1->t0006.u0 == 0x00)
        {
            do
            {
                if ((ptrArg04.u1->w0002 & 0x0200) != 0x00)
                    fn0800_2241(ds);
                if (___read(ds, (int16) ptrArg04.u1->b0004, &ds->b0680, 0x01) == 0x00)
                {
                    if (_eof(ds, (int16) ptrArg04.u1->b0004) != 0x01)
                        goto l0800_23BC;
                    ptrArg04.u1->w0002 = ptrArg04.u1->w0002 & 65151 | 0x20;
                    goto l0800_231B;
                }
            } while (ds->b0680 == 0x0D && ((ptrArg04.u1)->w0002 & 0x40) == 0x00);
            ptrArg04.u1->w0002 &= ~0x20;
            al_122 = ds->b0680;
l0800_23F8:
            ax_203 = (uint16) al_122;
            return ax_203;
        }
        if (fn0800_227F(ds, ptrArg04) != 0x00)
            goto l0800_231B;
    }
    --ptrArg04.u1->w0000;
    Eq_194 ax_195;
    ax_195.u0 = ptrArg04.u1->t000E.u0;
    Eq_484 si_196;
    si_196.u0 = ptrArg04.u1->t000C.u0;
    ptrArg04.u1->t000C.u0 = (cui16) (ptrArg04.u1->t000C.u0 + 0x01);
    al_122 = (byte) (ax_195->*si_196.u1);
    goto l0800_23F8;
}


uint16 _fgetc(struct Eq_22 * ds, Eq_3416 ptrArg04)
{
    uint16 ax_203;
    if (ptrArg04 == 0x00)
    {
l0800_231B:
        ax_203 = ~0x00;
        return ax_203;
    }
    byte al_122;
    if (ptrArg04.u1->w0000 <= 0x00)
    {
        if (ptrArg04.u1->w0000 < 0x00 || (((ptrArg04.u1)->w0002 & 0x0110) != 0x00 || ((ptrArg04.u1)->w0002 & 0x01) == 0x00))
        {
l0800_23BC:
            ptrArg04.u1->w0002 |= 0x10;
            goto l0800_231B;
        }
        ptrArg04.u1->w0002 |= 0x80;
        if (ptrArg04.u1->t0006.u0 == 0x00)
        {
            do
            {
                if ((ptrArg04.u1->w0002 & 0x0200) != 0x00)
                    fn0800_2241(ds);
                if (___read(ds, (int16) ptrArg04.u1->b0004, &ds->b0680, 0x01) == 0x00)
                {
                    if (_eof(ds, (int16) ptrArg04.u1->b0004) != 0x01)
                        goto l0800_23BC;
                    ptrArg04.u1->w0002 = ptrArg04.u1->w0002 & 65151 | 0x20;
                    goto l0800_231B;
                }
            } while (ds->b0680 == 0x0D && ((ptrArg04.u1)->w0002 & 0x40) == 0x00);
            ptrArg04.u1->w0002 &= ~0x20;
            al_122 = ds->b0680;
l0800_23F8:
            ax_203 = (uint16) al_122;
            return ax_203;
        }
        if (fn0800_227F(ds, ptrArg04) != 0x00)
            goto l0800_231B;
    }
    --ptrArg04.u1->w0000;
    Eq_194 ax_195;
    ax_195.u0 = ptrArg04.u1->t000E.u0;
    Eq_484 si_196;
    si_196.u0 = ptrArg04.u1->t000C.u0;
    ptrArg04.u1->t000C.u0 = (cui16) (ptrArg04.u1->t000C.u0 + 0x01);
    al_122 = (byte) (ax_195->*si_196.u1);
    goto l0800_23F8;
}


word16 _memcpy(Eq_484 ptrArg04, Eq_194 ptrArg08, Eq_194 wArg0C)
{
    Eq_4785 size_23 = (wArg0C >> 0x01) *32 0x02;
    memcpy(ptrArg04, ptrArg08, size_23);
    struct Eq_4793 * es_24 = SLICE(ptrArg04, selector, 16);
    struct Eq_4795 * ds_26 = SLICE(ptrArg08, selector, 16);
    byte Eq_4795::* si_29 = SLICE(ptrArg08 + size_23, word16, 0);
    byte Eq_4793::* di_30 = SLICE(ptrArg04 + size_23, word16, 0);
    if (wArg0C >> 0x01 < 0x00)
        es_24->*di_30 = ds_26->*si_29;
    return 0x00;
}


Eq_194 fn0800_242D(struct Eq_22 * ds, Eq_4817 wArg02, char * ptrArg04, union Eq_4817 & cxOut)
{
    Eq_194 ax_17;
    if (msdos_create_truncate_file(ptrArg04, wArg02, out ax_17))
        ax_17 = __IOERROR(ds, ax_17);
    cxOut = wArg02;
    return ax_17;
}


word16 fn0800_2448(struct Eq_22 * ds, Eq_194 wArg02)
{
    word16 ax_16;
    msdos_write_file(wArg02, 0x00, &ds->b0000, out ax_16);
    return 0x00;
}


byte _open(struct Eq_22 * ds, char * dwArg04, cui16 wArg08, cui16 wArg0A, ptr16 & cxOut)
{
    ptr16 cx_119;
    cui16 si_16 = wArg08;
    if ((wArg08 & 0xC000) == 0x00)
        si_16 = wArg08 | ds->w04A2 & 0xC000;
    Eq_4817 ax_45 = __chmod(ds, 0x00, out cx_119);
    Eq_4817 wLoc04_400 = ax_45;
    Eq_194 di_123;
    if ((si_16 & 0x0100) != 0x00)
    {
        cui16 di_56 = wArg0A & ds->w04A4;
        if ((di_56 & 0x0180) == 0x00)
            __IOERROR(ds, 0x01);
        Eq_194 ax_172;
        Eq_194 wLoc0C_403;
        if (ax_45 == ~0x00)
        {
            if (ds->t04A6 == 0x02)
            {
                Eq_4817 ax_93;
                if ((di_56 & 0x80) != 0x00)
                    ax_93.u0 = 0x00;
                else
                    ax_93.u0 = 0x01;
                wLoc04_400 = ax_93;
                if ((si_16 & 0xF0) != 0x00)
                {
                    Eq_194 ax_142 = fn0800_242D(ds, 0x00, dwArg04, out cx_119);
                    di_123 = ax_142;
                    byte al_156 = (byte) ax_142;
                    if (ax_142 >= 0x00)
                    {
                        __close(al_156, ds, ax_142);
                        goto l0800_2513;
                    }
                }
                else
                {
                    Eq_194 ax_118 = fn0800_242D(ds, ax_93, dwArg04, out cx_119);
                    di_123 = ax_118;
                    if (ax_118 >= 0x00)
                    {
l0800_258E:
                        if (di_123 >= 0x00)
                        {
                            cui16 ax_325;
                            if ((si_16 & 0x0300) != 0x00)
                                ax_325 = 0x1000;
                            else
                                ax_325 = 0x00;
                            cui16 dx_334 = si_16 & ~0x0700 | ax_325;
                            cui16 ax_347;
                            if ((wLoc04_400 & 0x01) != 0x00)
                                ax_347 = 0x00;
                            else
                                ax_347 = 0x0100;
                            (*((char *) &ds->t0478 + 2))[di_123] = dx_334 | ax_347;
                        }
                        goto l0800_25C1;
                    }
                }
l0800_25C1:
                ax_172 = di_123;
                goto l0800_25C3;
            }
            wLoc0C_403 = ds->t04A6;
        }
        else
        {
            if ((si_16 & 0x0400) == 0x00)
                goto l0800_2513;
            wLoc0C_403.u0 = 0x50;
        }
        ax_172 = __IOERROR(ds, wLoc0C_403);
l0800_25C3:
        cxOut = cx_119;
        return (byte) ax_172;
    }
    else
    {
l0800_2513:
        Eq_194 ax_191 = __open(ds, dwArg04, si_16, out cx_119);
        di_123 = ax_191;
        if (ax_191 >= 0x00)
        {
            cx_119 = 0x00;
            word16 cx_548;
            if ((_ioctl(ds, 0x00, out cx_548) & 0x80) != 0x00)
            {
                si_16 |= 0x2000;
                if ((si_16 & 0x8000) != 0x00)
                    _ioctl(ds, 0x01, out cx_119);
            }
            else if ((si_16 & 0x0200) != 0x00)
                cx_119 = fn0800_2448(ds, ax_191);
            if ((wLoc04_400 & 0x01) != 0x00 && ((si_16 & 0x0100) != 0x00 && (si_16 & 0xF0) != 0x00))
                __chmod(ds, 0x01, out cx_119);
        }
        goto l0800_258E;
    }
}


Eq_194 __open(struct Eq_22 * ds, char * ptrArg04, cui16 wArg08, ptr16 & cxOut)
{
    byte bArg08_72 = (byte) wArg08;
    byte al_10 = 0x01;
    byte ch_88 = SLICE(wArg08, byte, 8);
    if ((wArg08 & 0x02) == 0x00)
    {
        al_10 = 0x02;
        if ((wArg08 & 0x04) == 0x00)
            al_10 = 0x00;
    }
    ptr16 cx_89 = SEQ(ch_88, bArg08_72 & 0xF0);
    Eq_194 ax_39;
    Eq_194 ax_45;
    if (!msdos_open_file(ptrArg04, al_10 | bArg08_72 & 0xF0, out ax_39))
    {
        (*((char *) &ds->t0478 + 2))[ax_39] = wArg08 & ~0x4700 | 0x8000;
        ax_45 = ax_39;
    }
    else
        ax_45 = __IOERROR(ds, ax_39);
    cxOut = cx_89;
    return ax_45;
}


void _printf(struct Eq_22 * cx, ui16 dx, word16 bx, struct Eq_22 * ds, Eq_649 dwArg04)
{
    mp16 fp;
    __VPRINTER(cx, dx, bx, ds, fp + 8, dwArg04);
}


word16 __fputc(struct Eq_22 * ds, Eq_2234 bArg04, Eq_3416 ptrArg06, ptr16 & cxOut, ptr16 & bxOut)
{
    ptr16 bx_26;
    ptr16 cx_29;
    --ptrArg06.u1->w0000;
    word16 ax_25 = _fputc(ds, bArg04, ptrArg06, out cx_29, out bx_26);
    cxOut = cx_29;
    bxOut = bx_26;
    return ax_25;
}


uint16 _fputc(struct Eq_22 * ds, Eq_2234 bArg04, Eq_3416 ptrArg06, ptr16 & cxOut, ptr16 & bxOut)
{
    ptr16 wArg06_324 = (word16) ptrArg06;
    ptr16 wArg08_329 = SLICE(ptrArg06, word16, 16);
    ds->t0682 = bArg04;
    ptr16 bx_134 = (word16) ptrArg06;
    ptr16 cx;
    if (ptrArg06.u1->w0000 >= ~0x00)
    {
        if ((ptrArg06.u1->w0002 & 0x90) == 0x00 && ((ptrArg06.u1)->w0002 & 0x02) != 0x00)
        {
            ptrArg06.u1->w0002 |= 0x0100;
            ptr16 wArg08_347;
            if (ptrArg06.u1->t0006.u0 != 0x00)
            {
                if (ptrArg06.u1->w0000 != 0x00)
                {
                    cx = wArg08_329;
                    if (_fflush(ds, ptrArg06, out bx_134) != 0x00)
                        goto l0800_26AD;
                }
                ptrArg06.u1->w0000 = -ptrArg06.u1->t0006.u0;
                Eq_194 ax_190;
                ax_190.u0 = ptrArg06.u1->t000E.u0;
                Eq_484 si_191;
                si_191.u0 = ptrArg06.u1->t000C.u0;
                ptrArg06.u1->t000C.u0 = (cui16) (ptrArg06.u1->t000C.u0 + 0x01);
                (ax_190->*si_191.u1).u1 = ds->t0682;
                bx_134 = wArg06_324;
                if ((ptrArg06.u1->w0002 & 0x08) == 0x00 || ds->t0682 != 0x0A && ds->t0682 != 0x0D)
                    goto l0800_27DA;
                cx = wArg08_347;
                uint16 ax_297;
                if (_fflush(ds, ptrArg06, out bx_134) == 0x00)
                {
l0800_27DA:
                    ax_297 = (uint16) ds->t0682;
                    goto l0800_27DF;
                }
l0800_26AD:
                ax_297 = ~0x00;
l0800_27DF:
                cxOut = cx;
                bxOut = bx_134;
                return ax_297;
            }
            if (((*((char *) &ds->t0478 + 2))[(int16) ptrArg06.u1->b0004] & 0x0800) != 0x00)
            {
                word16 cx_459;
                word16 dx_460;
                _lseek(ds, (int16) ptrArg06.u1->b0004, 0x00, 0x02, out cx_459, out dx_460);
                wArg08_347 = SLICE(ptrArg06, word16, 16);
            }
            word16 bx_461;
            if ((ds->t0682 != 0x0A || (((ptrArg06.u1)->w0002 & 0x40) != 0x00 || __write(ds, (int16) (ptrArg06.u1)->b0004, &ds->w05C0 + 9, 0x01, out cx, out bx_461) == 0x01)) && __write(ds, (int16) (ptrArg06.u1)->b0004, &(ds->t0682).u0, 0x01, out cx, out bx_134) == 0x01)
                goto l0800_27DA;
            bx_134 = (word16) ptrArg06;
            if ((ptrArg06.u1->w0002 & 0x0200) != 0x00)
                goto l0800_27DA;
        }
        ptrArg06.u1->w0002 |= 0x10;
        bx_134 = (word16) ptrArg06;
        goto l0800_26AD;
    }
    else
    {
        ++ptrArg06.u1->w0000;
        Eq_194 ax_251;
        ax_251.u0 = ptrArg06.u1->t000E.u0;
        Eq_484 si_252;
        si_252.u0 = ptrArg06.u1->t000C.u0;
        ptrArg06.u1->t000C.u0 = (cui16) (ptrArg06.u1->t000C.u0 + 0x01);
        (ax_251->*si_252.u1).u1 = ds->t0682;
        if ((ptrArg06.u1->w0002 & 0x08) == 0x00 || ds->t0682 != 0x0A && ds->t0682 != 0x0D)
            goto l0800_27DA;
        cx = wArg08_329;
        if (_fflush(ds, ptrArg06, out bx_134) == 0x00)
            goto l0800_27DA;
        goto l0800_26AD;
    }
}


uint16 _fputc(struct Eq_22 * ds, Eq_2234 bArg04, Eq_3416 ptrArg06, ptr16 & cxOut, ptr16 & bxOut)
{
    ptr16 wArg06_324 = (word16) ptrArg06;
    ptr16 wArg08_329 = SLICE(ptrArg06, word16, 16);
    ds->t0682 = bArg04;
    ptr16 bx_134 = (word16) ptrArg06;
    ptr16 cx;
    if (ptrArg06.u1->w0000 >= ~0x00)
    {
        if ((ptrArg06.u1->w0002 & 0x90) == 0x00 && ((ptrArg06.u1)->w0002 & 0x02) != 0x00)
        {
            ptrArg06.u1->w0002 |= 0x0100;
            ptr16 wArg08_347;
            if (ptrArg06.u1->t0006.u0 != 0x00)
            {
                if (ptrArg06.u1->w0000 != 0x00)
                {
                    cx = wArg08_329;
                    if (_fflush(ds, ptrArg06, out bx_134) != 0x00)
                        goto l0800_26AD;
                }
                ptrArg06.u1->w0000 = -ptrArg06.u1->t0006.u0;
                Eq_194 ax_190;
                ax_190.u0 = ptrArg06.u1->t000E.u0;
                Eq_484 si_191;
                si_191.u0 = ptrArg06.u1->t000C.u0;
                ptrArg06.u1->t000C.u0 = (cui16) (ptrArg06.u1->t000C.u0 + 0x01);
                (ax_190->*si_191.u1).u1 = ds->t0682;
                bx_134 = wArg06_324;
                if ((ptrArg06.u1->w0002 & 0x08) == 0x00 || ds->t0682 != 0x0A && ds->t0682 != 0x0D)
                    goto l0800_27DA;
                cx = wArg08_347;
                uint16 ax_297;
                if (_fflush(ds, ptrArg06, out bx_134) == 0x00)
                {
l0800_27DA:
                    ax_297 = (uint16) ds->t0682;
                    goto l0800_27DF;
                }
l0800_26AD:
                ax_297 = ~0x00;
l0800_27DF:
                cxOut = cx;
                bxOut = bx_134;
                return ax_297;
            }
            if (((*((char *) &ds->t0478 + 2))[(int16) ptrArg06.u1->b0004] & 0x0800) != 0x00)
            {
                word16 cx_459;
                word16 dx_460;
                _lseek(ds, (int16) ptrArg06.u1->b0004, 0x00, 0x02, out cx_459, out dx_460);
                wArg08_347 = SLICE(ptrArg06, word16, 16);
            }
            word16 bx_461;
            if ((ds->t0682 != 0x0A || (((ptrArg06.u1)->w0002 & 0x40) != 0x00 || __write(ds, (int16) (ptrArg06.u1)->b0004, &ds->w05C0 + 9, 0x01, out cx, out bx_461) == 0x01)) && __write(ds, (int16) (ptrArg06.u1)->b0004, &(ds->t0682).u0, 0x01, out cx, out bx_134) == 0x01)
                goto l0800_27DA;
            bx_134 = (word16) ptrArg06;
            if ((ptrArg06.u1->w0002 & 0x0200) != 0x00)
                goto l0800_27DA;
        }
        ptrArg06.u1->w0002 |= 0x10;
        bx_134 = (word16) ptrArg06;
        goto l0800_26AD;
    }
    else
    {
        ++ptrArg06.u1->w0000;
        Eq_194 ax_251;
        ax_251.u0 = ptrArg06.u1->t000E.u0;
        Eq_484 si_252;
        si_252.u0 = ptrArg06.u1->t000C.u0;
        ptrArg06.u1->t000C.u0 = (cui16) (ptrArg06.u1->t000C.u0 + 0x01);
        (ax_251->*si_252.u1).u1 = ds->t0682;
        if ((ptrArg06.u1->w0002 & 0x08) == 0x00 || ds->t0682 != 0x0A && ds->t0682 != 0x0D)
            goto l0800_27DA;
        cx = wArg08_329;
        if (_fflush(ds, ptrArg06, out bx_134) == 0x00)
            goto l0800_27DA;
        goto l0800_26AD;
    }
}


uint16 _fputc(struct Eq_22 * ds, Eq_2234 bArg04, Eq_3416 ptrArg06, ptr16 & cxOut, ptr16 & bxOut)
{
    ptr16 wArg06_324 = (word16) ptrArg06;
    ptr16 wArg08_329 = SLICE(ptrArg06, word16, 16);
    ds->t0682 = bArg04;
    ptr16 bx_134 = (word16) ptrArg06;
    ptr16 cx;
    if (ptrArg06.u1->w0000 >= ~0x00)
    {
        if ((ptrArg06.u1->w0002 & 0x90) == 0x00 && ((ptrArg06.u1)->w0002 & 0x02) != 0x00)
        {
            ptrArg06.u1->w0002 |= 0x0100;
            ptr16 wArg08_347;
            if (ptrArg06.u1->t0006.u0 != 0x00)
            {
                if (ptrArg06.u1->w0000 != 0x00)
                {
                    cx = wArg08_329;
                    if (_fflush(ds, ptrArg06, out bx_134) != 0x00)
                        goto l0800_26AD;
                }
                ptrArg06.u1->w0000 = -ptrArg06.u1->t0006.u0;
                Eq_194 ax_190;
                ax_190.u0 = ptrArg06.u1->t000E.u0;
                Eq_484 si_191;
                si_191.u0 = ptrArg06.u1->t000C.u0;
                ptrArg06.u1->t000C.u0 = (cui16) (ptrArg06.u1->t000C.u0 + 0x01);
                (ax_190->*si_191.u1).u1 = ds->t0682;
                bx_134 = wArg06_324;
                if ((ptrArg06.u1->w0002 & 0x08) == 0x00 || ds->t0682 != 0x0A && ds->t0682 != 0x0D)
                    goto l0800_27DA;
                cx = wArg08_347;
                uint16 ax_297;
                if (_fflush(ds, ptrArg06, out bx_134) == 0x00)
                {
l0800_27DA:
                    ax_297 = (uint16) ds->t0682;
                    goto l0800_27DF;
                }
l0800_26AD:
                ax_297 = ~0x00;
l0800_27DF:
                cxOut = cx;
                bxOut = bx_134;
                return ax_297;
            }
            if (((*((char *) &ds->t0478 + 2))[(int16) ptrArg06.u1->b0004] & 0x0800) != 0x00)
            {
                word16 cx_459;
                word16 dx_460;
                _lseek(ds, (int16) ptrArg06.u1->b0004, 0x00, 0x02, out cx_459, out dx_460);
                wArg08_347 = SLICE(ptrArg06, word16, 16);
            }
            word16 bx_461;
            if ((ds->t0682 != 0x0A || (((ptrArg06.u1)->w0002 & 0x40) != 0x00 || __write(ds, (int16) (ptrArg06.u1)->b0004, &ds->w05C0 + 9, 0x01, out cx, out bx_461) == 0x01)) && __write(ds, (int16) (ptrArg06.u1)->b0004, &(ds->t0682).u0, 0x01, out cx, out bx_134) == 0x01)
                goto l0800_27DA;
            bx_134 = (word16) ptrArg06;
            if ((ptrArg06.u1->w0002 & 0x0200) != 0x00)
                goto l0800_27DA;
        }
        ptrArg06.u1->w0002 |= 0x10;
        bx_134 = (word16) ptrArg06;
        goto l0800_26AD;
    }
    else
    {
        ++ptrArg06.u1->w0000;
        Eq_194 ax_251;
        ax_251.u0 = ptrArg06.u1->t000E.u0;
        Eq_484 si_252;
        si_252.u0 = ptrArg06.u1->t000C.u0;
        ptrArg06.u1->t000C.u0 = (cui16) (ptrArg06.u1->t000C.u0 + 0x01);
        (ax_251->*si_252.u1).u1 = ds->t0682;
        if ((ptrArg06.u1->w0002 & 0x08) == 0x00 || ds->t0682 != 0x0A && ds->t0682 != 0x0D)
            goto l0800_27DA;
        cx = wArg08_329;
        if (_fflush(ds, ptrArg06, out bx_134) == 0x00)
            goto l0800_27DA;
        goto l0800_26AD;
    }
}


Eq_194 __FPUTN(struct Eq_22 * ds, Eq_3416 ptrArg02, Eq_194 wArg06, Eq_194 ptrArg08, ptr16 & cxOut, ptr16 & bxOut)
{
    ptr16 wArg04_463 = SLICE(ptrArg02, word16, 16);
    Eq_194 di_16 = wArg06;
    ptr16 bx_108 = (word16) ptrArg02;
    Eq_194 ptrArg08_528;
    ptr16 cx;
    if ((ptrArg02.u1->w0002 & 0x08) != 0x00)
    {
        Eq_194 ptrArg08_689;
        do
        {
            word16 wArg08_540 = (word16) ptrArg08_689;
            word16 wArg0A_590 = SLICE(ptrArg08_689, word16, 16);
            --di_16;
            if (di_16 == 0x00)
                goto l0800_29EA;
            ptrArg08_528 = SEQ(wArg0A_590, wArg08_540 + 0x01);
            ptrArg08_689 = ptrArg08_528;
        } while (_fputc(ds, *ptrArg08_689.u4, ptrArg02, out cx, out bx_108) != ~0x00);
        goto l0800_282E;
    }
    Eq_194 ax_428;
    if ((ptrArg02.u1->w0002 & 0x40) == 0x00)
    {
        bx_108 = (word16) ptrArg02;
        ptrArg08_528 = ptrArg08;
        if (ptrArg02.u1->t0006.u0 != 0x00)
        {
            uint16 ax_355;
            do
            {
                word16 wArg08_529 = (word16) ptrArg08_528;
                word16 wArg0A_587 = SLICE(ptrArg08_528, word16, 16);
                --di_16;
                if (di_16 == 0x00)
                    goto l0800_29EA;
                ci16 v23_332 = ptrArg02.u1->w0000 + 0x01;
                ptrArg02.u1->w0000 = v23_332;
                if (v23_332 < 0x00)
                {
                    Eq_194 ax_367;
                    ax_367.u0 = ptrArg02.u1->t000E.u0;
                    Eq_484 si_368;
                    si_368.u0 = ptrArg02.u1->t000C.u0;
                    ptrArg02.u1->t000C.u0 = (cui16) (ptrArg02.u1->t000C.u0 + 0x01);
                    Eq_2234 dl_377 = *ptrArg08_528.u4;
                    (ax_367->*si_368.u1).u1 = dl_377;
                    bx_108 = (word16) ptrArg08_528;
                    ax_355 = (uint16) dl_377;
                    ptrArg08_528 = SEQ(wArg0A_587, wArg08_529 + 0x01);
                }
                else
                {
                    ax_355 = __fputc(ds, *ptrArg08_528.u4, ptrArg02, out cx, out bx_108);
                    ptrArg08_528 = SEQ(wArg0A_587, wArg08_529 + 0x01);
                }
            } while (ax_355 != ~0x00);
            goto l0800_282E;
        }
        if (___write(ds, (int16) ptrArg02.u1->b0004, ptrArg08, wArg06, out cx, out bx_108) < wArg06)
            goto l0800_282E;
    }
    else if (ptrArg02.u1->t0006.u0 == 0x00)
    {
        if (((*((char *) &ds->t0478 + 2))[(int16) ptrArg02.u1->b0004] & 0x0800) != 0x00)
        {
            word16 cx_685;
            word16 dx_686;
            _lseek(ds, (int16) ptrArg02.u1->b0004, 0<32>, 0x02, out cx_685, out dx_686);
        }
        if (__write(ds, (int16) ptrArg02.u1->b0004, ptrArg08, wArg06, out cx, out bx_108) < wArg06)
            goto l0800_282E;
    }
    else if (ptrArg02.u1->t0006.u0 < wArg06)
    {
        if (ptrArg02.u1->w0000 != 0x00)
        {
            cx = wArg04_463;
            if (_fflush(ds, ptrArg02, out bx_108) != 0x00)
                goto l0800_282E;
        }
        if (((*((char *) &ds->t0478 + 2))[(int16) ptrArg02.u1->b0004] & 0x0800) != 0x00)
        {
            word16 cx_687;
            word16 dx_688;
            _lseek(ds, (int16) ptrArg02.u1->b0004, 0<32>, 0x02, out cx_687, out dx_688);
        }
        if (__write(ds, (int16) ptrArg02.u1->b0004, ptrArg08, wArg06, out cx, out bx_108) < wArg06)
        {
l0800_282E:
            ax_428.u0 = 0x00;
l0800_29ED:
            cxOut = cx;
            bxOut = bx_108;
            return ax_428;
        }
    }
    else
    {
        if (wArg06.u4 + (ptrArg02.u1)->w0000 >= 0x00)
        {
            if (ptrArg02.u1->w0000 == 0x00)
                ptrArg02.u1->w0000 = ~0x00 - ((ptrArg02.u1)->t0006).u0;
            else
            {
                cx = wArg04_463;
                if (_fflush(ds, ptrArg02, out bx_108) != 0x00)
                    goto l0800_282E;
            }
        }
        cx = _memcpy(ptrArg02.u1->t000C.u1, ptrArg08, wArg06);
        ptrArg02.u1->w0000 = (ci16) (wArg06.u4 + (ptrArg02.u1)->w0000);
        ptrArg02.u1->t000C.u0 = (cui16) (wArg06.u4 + ((ptrArg02.u1)->t000C).u0);
        bx_108 = (word16) ptrArg02;
    }
l0800_29EA:
    ax_428 = wArg06;
    goto l0800_29ED;
}


Eq_194 __FPUTN(struct Eq_22 * ds, Eq_3416 ptrArg02, Eq_194 wArg06, Eq_194 ptrArg08, ptr16 & cxOut, ptr16 & bxOut)
{
    ptr16 wArg04_463 = SLICE(ptrArg02, word16, 16);
    Eq_194 di_16 = wArg06;
    ptr16 bx_108 = (word16) ptrArg02;
    Eq_194 ptrArg08_528;
    ptr16 cx;
    if ((ptrArg02.u1->w0002 & 0x08) != 0x00)
    {
        Eq_194 ptrArg08_689;
        do
        {
            word16 wArg08_540 = (word16) ptrArg08_689;
            word16 wArg0A_590 = SLICE(ptrArg08_689, word16, 16);
            --di_16;
            if (di_16 == 0x00)
                goto l0800_29EA;
            ptrArg08_528 = SEQ(wArg0A_590, wArg08_540 + 0x01);
            ptrArg08_689 = ptrArg08_528;
        } while (_fputc(ds, *ptrArg08_689.u4, ptrArg02, out cx, out bx_108) != ~0x00);
        goto l0800_282E;
    }
    Eq_194 ax_428;
    if ((ptrArg02.u1->w0002 & 0x40) == 0x00)
    {
        bx_108 = (word16) ptrArg02;
        ptrArg08_528 = ptrArg08;
        if (ptrArg02.u1->t0006.u0 != 0x00)
        {
            uint16 ax_355;
            do
            {
                word16 wArg08_529 = (word16) ptrArg08_528;
                word16 wArg0A_587 = SLICE(ptrArg08_528, word16, 16);
                --di_16;
                if (di_16 == 0x00)
                    goto l0800_29EA;
                ci16 v23_332 = ptrArg02.u1->w0000 + 0x01;
                ptrArg02.u1->w0000 = v23_332;
                if (v23_332 < 0x00)
                {
                    Eq_194 ax_367;
                    ax_367.u0 = ptrArg02.u1->t000E.u0;
                    Eq_484 si_368;
                    si_368.u0 = ptrArg02.u1->t000C.u0;
                    ptrArg02.u1->t000C.u0 = (cui16) (ptrArg02.u1->t000C.u0 + 0x01);
                    Eq_2234 dl_377 = *ptrArg08_528.u4;
                    (ax_367->*si_368.u1).u1 = dl_377;
                    bx_108 = (word16) ptrArg08_528;
                    ax_355 = (uint16) dl_377;
                    ptrArg08_528 = SEQ(wArg0A_587, wArg08_529 + 0x01);
                }
                else
                {
                    ax_355 = __fputc(ds, *ptrArg08_528.u4, ptrArg02, out cx, out bx_108);
                    ptrArg08_528 = SEQ(wArg0A_587, wArg08_529 + 0x01);
                }
            } while (ax_355 != ~0x00);
            goto l0800_282E;
        }
        if (___write(ds, (int16) ptrArg02.u1->b0004, ptrArg08, wArg06, out cx, out bx_108) < wArg06)
            goto l0800_282E;
    }
    else if (ptrArg02.u1->t0006.u0 == 0x00)
    {
        if (((*((char *) &ds->t0478 + 2))[(int16) ptrArg02.u1->b0004] & 0x0800) != 0x00)
        {
            word16 cx_685;
            word16 dx_686;
            _lseek(ds, (int16) ptrArg02.u1->b0004, 0<32>, 0x02, out cx_685, out dx_686);
        }
        if (__write(ds, (int16) ptrArg02.u1->b0004, ptrArg08, wArg06, out cx, out bx_108) < wArg06)
            goto l0800_282E;
    }
    else if (ptrArg02.u1->t0006.u0 < wArg06)
    {
        if (ptrArg02.u1->w0000 != 0x00)
        {
            cx = wArg04_463;
            if (_fflush(ds, ptrArg02, out bx_108) != 0x00)
                goto l0800_282E;
        }
        if (((*((char *) &ds->t0478 + 2))[(int16) ptrArg02.u1->b0004] & 0x0800) != 0x00)
        {
            word16 cx_687;
            word16 dx_688;
            _lseek(ds, (int16) ptrArg02.u1->b0004, 0<32>, 0x02, out cx_687, out dx_688);
        }
        if (__write(ds, (int16) ptrArg02.u1->b0004, ptrArg08, wArg06, out cx, out bx_108) < wArg06)
        {
l0800_282E:
            ax_428.u0 = 0x00;
l0800_29ED:
            cxOut = cx;
            bxOut = bx_108;
            return ax_428;
        }
    }
    else
    {
        if (wArg06.u4 + (ptrArg02.u1)->w0000 >= 0x00)
        {
            if (ptrArg02.u1->w0000 == 0x00)
                ptrArg02.u1->w0000 = ~0x00 - ((ptrArg02.u1)->t0006).u0;
            else
            {
                cx = wArg04_463;
                if (_fflush(ds, ptrArg02, out bx_108) != 0x00)
                    goto l0800_282E;
            }
        }
        cx = _memcpy(ptrArg02.u1->t000C.u1, ptrArg08, wArg06);
        ptrArg02.u1->w0000 = (ci16) (wArg06.u4 + (ptrArg02.u1)->w0000);
        ptrArg02.u1->t000C.u0 = (cui16) (wArg06.u4 + ((ptrArg02.u1)->t000C).u0);
        bx_108 = (word16) ptrArg02;
    }
l0800_29EA:
    ax_428 = wArg06;
    goto l0800_29ED;
}


Eq_194 __FPUTN(struct Eq_22 * ds, Eq_3416 ptrArg02, Eq_194 wArg06, Eq_194 ptrArg08, ptr16 & cxOut, ptr16 & bxOut)
{
    ptr16 wArg04_463 = SLICE(ptrArg02, word16, 16);
    Eq_194 di_16 = wArg06;
    ptr16 bx_108 = (word16) ptrArg02;
    Eq_194 ptrArg08_528;
    ptr16 cx;
    if ((ptrArg02.u1->w0002 & 0x08) != 0x00)
    {
        Eq_194 ptrArg08_689;
        do
        {
            word16 wArg08_540 = (word16) ptrArg08_689;
            word16 wArg0A_590 = SLICE(ptrArg08_689, word16, 16);
            --di_16;
            if (di_16 == 0x00)
                goto l0800_29EA;
            ptrArg08_528 = SEQ(wArg0A_590, wArg08_540 + 0x01);
            ptrArg08_689 = ptrArg08_528;
        } while (_fputc(ds, *ptrArg08_689.u4, ptrArg02, out cx, out bx_108) != ~0x00);
        goto l0800_282E;
    }
    Eq_194 ax_428;
    if ((ptrArg02.u1->w0002 & 0x40) == 0x00)
    {
        bx_108 = (word16) ptrArg02;
        ptrArg08_528 = ptrArg08;
        if (ptrArg02.u1->t0006.u0 != 0x00)
        {
            uint16 ax_355;
            do
            {
                word16 wArg08_529 = (word16) ptrArg08_528;
                word16 wArg0A_587 = SLICE(ptrArg08_528, word16, 16);
                --di_16;
                if (di_16 == 0x00)
                    goto l0800_29EA;
                ci16 v23_332 = ptrArg02.u1->w0000 + 0x01;
                ptrArg02.u1->w0000 = v23_332;
                if (v23_332 < 0x00)
                {
                    Eq_194 ax_367;
                    ax_367.u0 = ptrArg02.u1->t000E.u0;
                    Eq_484 si_368;
                    si_368.u0 = ptrArg02.u1->t000C.u0;
                    ptrArg02.u1->t000C.u0 = (cui16) (ptrArg02.u1->t000C.u0 + 0x01);
                    Eq_2234 dl_377 = *ptrArg08_528.u4;
                    (ax_367->*si_368.u1).u1 = dl_377;
                    bx_108 = (word16) ptrArg08_528;
                    ax_355 = (uint16) dl_377;
                    ptrArg08_528 = SEQ(wArg0A_587, wArg08_529 + 0x01);
                }
                else
                {
                    ax_355 = __fputc(ds, *ptrArg08_528.u4, ptrArg02, out cx, out bx_108);
                    ptrArg08_528 = SEQ(wArg0A_587, wArg08_529 + 0x01);
                }
            } while (ax_355 != ~0x00);
            goto l0800_282E;
        }
        if (___write(ds, (int16) ptrArg02.u1->b0004, ptrArg08, wArg06, out cx, out bx_108) < wArg06)
            goto l0800_282E;
    }
    else if (ptrArg02.u1->t0006.u0 == 0x00)
    {
        if (((*((char *) &ds->t0478 + 2))[(int16) ptrArg02.u1->b0004] & 0x0800) != 0x00)
        {
            word16 cx_685;
            word16 dx_686;
            _lseek(ds, (int16) ptrArg02.u1->b0004, 0<32>, 0x02, out cx_685, out dx_686);
        }
        if (__write(ds, (int16) ptrArg02.u1->b0004, ptrArg08, wArg06, out cx, out bx_108) < wArg06)
            goto l0800_282E;
    }
    else if (ptrArg02.u1->t0006.u0 < wArg06)
    {
        if (ptrArg02.u1->w0000 != 0x00)
        {
            cx = wArg04_463;
            if (_fflush(ds, ptrArg02, out bx_108) != 0x00)
                goto l0800_282E;
        }
        if (((*((char *) &ds->t0478 + 2))[(int16) ptrArg02.u1->b0004] & 0x0800) != 0x00)
        {
            word16 cx_687;
            word16 dx_688;
            _lseek(ds, (int16) ptrArg02.u1->b0004, 0<32>, 0x02, out cx_687, out dx_688);
        }
        if (__write(ds, (int16) ptrArg02.u1->b0004, ptrArg08, wArg06, out cx, out bx_108) < wArg06)
        {
l0800_282E:
            ax_428.u0 = 0x00;
l0800_29ED:
            cxOut = cx;
            bxOut = bx_108;
            return ax_428;
        }
    }
    else
    {
        if (wArg06.u4 + (ptrArg02.u1)->w0000 >= 0x00)
        {
            if (ptrArg02.u1->w0000 == 0x00)
                ptrArg02.u1->w0000 = ~0x00 - ((ptrArg02.u1)->t0006).u0;
            else
            {
                cx = wArg04_463;
                if (_fflush(ds, ptrArg02, out bx_108) != 0x00)
                    goto l0800_282E;
            }
        }
        cx = _memcpy(ptrArg02.u1->t000C.u1, ptrArg08, wArg06);
        ptrArg02.u1->w0000 = (ci16) (wArg06.u4 + (ptrArg02.u1)->w0000);
        ptrArg02.u1->t000C.u0 = (cui16) (wArg06.u4 + ((ptrArg02.u1)->t000C).u0);
        bx_108 = (word16) ptrArg02;
    }
l0800_29EA:
    ax_428 = wArg06;
    goto l0800_29ED;
}


Eq_194 ___read(struct Eq_22 * ds, Eq_194 wArg04, Eq_194 ptrArg06, Eq_194 wArg0A)
{
    byte ch_358;
    struct Eq_5602 * wArg08 = SLICE(ptrArg06, word16, 16);
    Eq_5604 wArg06 = (word16) ptrArg06;
    Eq_194 ax_178;
    if (wArg04 >= ds->t0478)
        ax_178 = __IOERROR(ds, 0x06);
    else if (wArg0A >= 0x01 && ((*((char *) (&ds->t0478) + 2))[wArg04] & 0x0200) == 0x00)
    {
        Eq_5604 di_155;
        do
        {
            Eq_194 ax_47 = __read(ds, wArg04, ptrArg06, wArg0A, out ch_358);
            if (ax_47 < 0x01 || ((*((char *) (&ds->t0478) + 2))[wArg04] & 0x4000) == 0x00)
            {
                ax_178 = ax_47;
                return ax_178;
            }
            Eq_194 cx_122 = ax_47;
            Eq_5604 si_69 = wArg06;
            di_155 = wArg06;
            do
            {
                byte al_76 = (byte) (wArg08->*si_69);
                si_69 = (word16) si_69 + 1;
                if (al_76 == 0x1A)
                {
                    word16 cx_359;
                    word16 dx_360;
                    _lseek(ds, wArg04, SEQ(0x00 - (word16) (cx_122 != 0x00), -cx_122), 0x01, out cx_359, out dx_360);
                    (*((char *) &ds->t0478 + 2))[wArg04] |= 0x0200;
                    goto l0800_2ABD;
                }
                if (al_76 != 0x0D)
                {
                    wArg08->*di_155 = al_76;
                    di_155 = (word16) di_155 + 1;
                    --cx_122;
                    if (cx_122 == 0x00)
                        goto l0800_2A93;
                    continue;
                }
                --cx_122;
            } while (cx_122 != 0x00);
            <unknown> Eq_5698::* fp;
            byte ch_361;
            __read(ds, wArg04, fp - 5, 0x01, out ch_361);
            byte bLoc05;
            wArg08->*di_155 = bLoc05;
            di_155 = (word16) di_155 + 1;
l0800_2A93:
        } while (di_155 == wArg06);
l0800_2ABD:
        ax_178 = di_155 - wArg06;
    }
    else
        ax_178.u0 = 0x00;
    return ax_178;
}


word16 _setvbuf(struct Eq_22 * ds, Eq_3416 ptrArg04, Eq_484 wArg08, Eq_194 wArg0A, ci16 wArg0C, Eq_194 wArg0E, union Eq_194 & cxOut, struct Eq_22 & dsOut)
{
    Eq_194 dx_161;
    ptr16 wArg04_235 = (word16) ptrArg04;
    word16 ax_208;
    if (ptrArg04.u1->ptr0012 != wArg04_235 || (wArg0C > 0x02 || wArg0E > 0x7FFF))
    {
l0800_2AE8:
        ax_208 = ~0x00;
        goto l0800_2BDD;
    }
    else
    {
        if (ds->w05D6 == 0x00 && wArg04_235 == (<unknown>*) 0x02FC)
            ds->w05D6 = 0x01;
        else if (ds->w05D4 == 0x00 && wArg04_235 == (<unknown>*) 744)
            ds->w05D4 = 0x01;
        Eq_194 cx;
        if (ptrArg04.u1->w0000 != 0x00)
            cx = _fseek(ds, ptrArg04, 0x00, 0x01);
        Eq_194 wArg06_254;
        wArg06_254.u4 = SLICE(ptrArg04, word16, 16);
        word16 wArg04_255 = (word16) ptrArg04;
        if ((ptrArg04.u1->w0002 & 0x04) != 0x00)
        {
            Eq_194 v22_91;
            v22_91.u0 = ptrArg04.u1->t000A.u0;
            ds = _farfree(ds, v22_91);
            cx = v22_91;
        }
        ptrArg04.u1->w0002 &= ~0x0C;
        ptrArg04.u1->t0006.u0 = 0x00;
        ptrArg04.u1->t000A.u0 = (wchar_t) wArg06_254;
        ptrArg04.u1->t0008.u0 = (cui16) (wArg04_255 + 0x05);
        ptrArg04.u1->t000E.u0 = (wchar_t) wArg06_254;
        ptrArg04.u1->t000C.u0 = (cui16) (wArg04_255 + 0x05);
        if (wArg0C != 0x02 && wArg0E > 0x00)
        {
            selector seg0800;
            ds->ptr02DE = seg0800;
            ds->t02DC.u0 = 0x2E07;
            if ((wArg08 | wArg0A) == 0x00)
            {
                Eq_484 ax_158 = _malloc(ds, wArg0E, out dx_161, out ds);
                cx = wArg0E;
                wArg0A = dx_161;
                wArg08 = ax_158;
                if ((ax_158 | dx_161) == 0x00)
                    goto l0800_2AE8;
                ptrArg04.u1->w0002 |= 0x04;
            }
            ptrArg04.u1->t000E.u0 = (wchar_t) wArg0A;
            ptrArg04.u1->t000C.u0 = (cui16) wArg08;
            ptrArg04.u1->t000A.u0 = (wchar_t) wArg0A;
            ptrArg04.u1->t0008.u0 = (cui16) wArg08;
            ptrArg04.u1->t0006.u0 = (wchar_t) wArg0E;
            if (wArg0C == 0x01)
                ptrArg04.u1->w0002 |= 0x08;
        }
        ax_208 = 0x00;
l0800_2BDD:
        cxOut = cx;
        dsOut = ds;
        return ax_208;
    }
}


word16 _setvbuf(struct Eq_22 * ds, Eq_3416 ptrArg04, Eq_484 wArg08, Eq_194 wArg0A, ci16 wArg0C, Eq_194 wArg0E, union Eq_194 & cxOut, struct Eq_22 & dsOut)
{
    Eq_194 dx_161;
    ptr16 wArg04_235 = (word16) ptrArg04;
    word16 ax_208;
    if (ptrArg04.u1->ptr0012 != wArg04_235 || (wArg0C > 0x02 || wArg0E > 0x7FFF))
    {
l0800_2AE8:
        ax_208 = ~0x00;
        goto l0800_2BDD;
    }
    else
    {
        if (ds->w05D6 == 0x00 && wArg04_235 == (<unknown>*) 0x02FC)
            ds->w05D6 = 0x01;
        else if (ds->w05D4 == 0x00 && wArg04_235 == (<unknown>*) 744)
            ds->w05D4 = 0x01;
        Eq_194 cx;
        if (ptrArg04.u1->w0000 != 0x00)
            cx = _fseek(ds, ptrArg04, 0x00, 0x01);
        Eq_194 wArg06_254;
        wArg06_254.u4 = SLICE(ptrArg04, word16, 16);
        word16 wArg04_255 = (word16) ptrArg04;
        if ((ptrArg04.u1->w0002 & 0x04) != 0x00)
        {
            Eq_194 v22_91;
            v22_91.u0 = ptrArg04.u1->t000A.u0;
            ds = _farfree(ds, v22_91);
            cx = v22_91;
        }
        ptrArg04.u1->w0002 &= ~0x0C;
        ptrArg04.u1->t0006.u0 = 0x00;
        ptrArg04.u1->t000A.u0 = (wchar_t) wArg06_254;
        ptrArg04.u1->t0008.u0 = (cui16) (wArg04_255 + 0x05);
        ptrArg04.u1->t000E.u0 = (wchar_t) wArg06_254;
        ptrArg04.u1->t000C.u0 = (cui16) (wArg04_255 + 0x05);
        if (wArg0C != 0x02 && wArg0E > 0x00)
        {
            selector seg0800;
            ds->ptr02DE = seg0800;
            ds->t02DC.u0 = 0x2E07;
            if ((wArg08 | wArg0A) == 0x00)
            {
                Eq_484 ax_158 = _malloc(ds, wArg0E, out dx_161, out ds);
                cx = wArg0E;
                wArg0A = dx_161;
                wArg08 = ax_158;
                if ((ax_158 | dx_161) == 0x00)
                    goto l0800_2AE8;
                ptrArg04.u1->w0002 |= 0x04;
            }
            ptrArg04.u1->t000E.u0 = (wchar_t) wArg0A;
            ptrArg04.u1->t000C.u0 = (cui16) wArg08;
            ptrArg04.u1->t000A.u0 = (wchar_t) wArg0A;
            ptrArg04.u1->t0008.u0 = (cui16) wArg08;
            ptrArg04.u1->t0006.u0 = (wchar_t) wArg0E;
            if (wArg0C == 0x01)
                ptrArg04.u1->w0002 |= 0x08;
        }
        ax_208 = 0x00;
l0800_2BDD:
        cxOut = cx;
        dsOut = ds;
        return ax_208;
    }
}


void _strcat(Eq_484 ptrArg04, segptr32 ptrArg08)
{
    byte Eq_5865::* di_191 = (word16) ptrArg04;
    struct Eq_5865 * es_128 = SLICE(ptrArg04, selector, 16);
    word16 cx_26 = ~0x00;
    while (cx_26 != 0x00)
    {
        ++di_191;
        --cx_26;
        if (es_128->*di_191 != 0x00)
            break;
    }
    byte Eq_5865::* si_44 = di_191 - 1;
    struct Eq_5872 * es_130 = SLICE(ptrArg08, selector, 16);
    byte Eq_5872::* di_131 = (word16) ptrArg08;
    word16 cx_48 = ~0x00;
    while (cx_48 != 0x00)
    {
        ++di_131;
        --cx_48;
        if (es_130->*di_131 != 0x00)
            break;
    }
    cu16 cx_59 = ~cx_48;
    Eq_5892 di_61 = di_131 - cx_59;
    byte Eq_5865::* di_76 = si_44;
    <unknown>* si_77 = di_61;
    if ((di_61 & 0x01) != 0x00)
    {
        es_128->*si_44 = (byte) (es_130->*di_61);
        si_77 = (<unknown>*) ((word16) di_61 + 1);
        di_76 = si_44 + 1;
        --cx_59;
    }
    uint16 cx_89 = cx_59 >> 0x01;
    memcpy(&(es_128->*di_76), si_77, cx_89 *32 0x02);
    byte Eq_5872::* si_97 = (byte Eq_5872::*) ((char *) si_77 + cx_89 * 0x02);
    byte Eq_5865::* di_98 = di_76 + cx_89 * 0x02;
    if (cx_89 < 0x00)
        es_128->*di_98 = es_130->*si_97;
}


word16 _strlen(Eq_194 ptrArg04)
{
    word16 wArg06_66 = SLICE(ptrArg04, word16, 16);
    byte Eq_5960::* di_16 = (word16) ptrArg04;
    struct Eq_5960 * es_60 = SLICE(ptrArg04, selector, 16);
    word16 ax_13 = 0x00;
    if (wArg06_66 != 0x00 || di_16 != &Eq_5960::b0000)
    {
        word16 cx_20 = ~0x00;
        while (cx_20 != 0x00)
        {
            byte Eq_5960::* di_103;
            di_16 = di_103 + 1;
            --cx_20;
            di_103 = di_16;
            if (es_60->*di_103 != 0x00)
                break;
        }
        ax_13 = -cx_20;
    }
    return ax_13;
}


Eq_194 ___write(struct Eq_22 * ds, Eq_194 wArg04, Eq_194 dwArg06, Eq_194 wArg0A, ptr16 & cxOut, union Eq_3573 & bxOut)
{
    <unknown> Eq_6043::* fp;
    Eq_194 ax_129;
    ptr16 cx;
    Eq_3573 bx;
    if (wArg04 >= ds->t0478)
        ax_129 = __IOERROR(ds, 0x06);
    else if (wArg0A < 0x01)
        ax_129.u0 = 0x00;
    else
    {
        if (((*((char *) &ds->t0478 + 2))[wArg04] & 0x0800) != 0x00)
        {
            word16 dx_481;
            _lseek(ds, wArg04, 0x00, 0x02, out cx, out dx_481);
        }
        if (((*((char *) &ds->t0478 + 2))[wArg04] & 0x4000) == 0x00)
            ax_129 = __write(ds, wArg04, dwArg06, wArg0A, out cx, out bx);
        else
        {
            (*((char *) &ds->t0478 + 2))[wArg04] &= ~0x0200;
            bx = wArg04 << 0x01;
            Eq_194 wLoc08_310 = wArg0A;
            Eq_194 ptrLoc0E_323 = dwArg06;
            Eq_194 si_183;
            Eq_194 dx_123;
            Eq_194 ax_132;
            Eq_194 v24_133;
            Eq_194 ax_202;
            do
            {
                union Eq_2234 * ptrLoc06_476 = (union Eq_2234 *) (fp - 0x90);
                word16 wLoc06_335;
                Eq_6094 bx_dx_290;
                do
                {
                    word16 wLoc04_327 = SLICE(ptrLoc06_476, word16, 16);
                    word16 wLoc0E_324 = (word16) ptrLoc0E_323;
                    word16 wLoc0C_382 = SLICE(ptrLoc0E_323, word16, 16);
                    word16 wLoc06_314 = (word16) ptrLoc06_476;
                    union Eq_2234 * ptrLoc06_328 = ptrLoc06_476;
                    if (wLoc08_310 == 0x00)
                    {
                        si_183 = wLoc06_314 - (fp - 0x90);
                        if (si_183 > 0x00)
                        {
                            Eq_194 ax_113 = __write(ds, wArg04, fp - 0x90, si_183, out cx, out bx);
                            dx_123 = ax_113;
                            if (ax_113 != si_183)
                            {
                                if (ax_113 == ~0x00)
                                    goto l0800_2D2D;
                                ax_132 = wArg0A;
                                goto l0800_2D7F;
                            }
                        }
                        ax_129 = wArg0A;
                        goto l0800_2D88;
                    }
                    v24_133 = wLoc08_310 - 0x01;
                    wLoc08_310 = v24_133;
                    Eq_2234 al_140 = *ptrLoc0E_323.u4;
                    ptrLoc0E_323 = SEQ(wLoc0C_382, wLoc0E_324 + 0x01);
                    if (al_140 == 0x0A)
                    {
                        ptrLoc06_476->u1 = 0x0D;
                        ptrLoc06_328 = SEQ(wLoc04_327, wLoc06_314 + 0x01);
                    }
                    ptrLoc06_328->u1 = (byte) al_140;
                    wLoc06_335 = (word16) ptrLoc06_328;
                    bx_dx_290 = wLoc06_335 + 0x01 - (fp - 0x90);
                    bx = SLICE(bx_dx_290, word16, 16);
                    ptrLoc06_476 = SEQ(SLICE(ptrLoc06_328, word16, 16), wLoc06_335 + 0x01);
                } while (bx_dx_290 < 0x80);
                si_183 = wLoc06_335 + 0x01 - (fp - 0x90);
                ax_202 = __write(ds, wArg04, fp - 0x90, si_183, out cx, out bx);
                dx_123 = ax_202;
            } while (ax_202 == si_183);
            if (ax_202 == ~0x00)
            {
l0800_2D2D:
                ax_129.u0 = ~0x00;
            }
            else
            {
                ax_132 = wArg0A - v24_133;
l0800_2D7F:
                ax_129 = ax_132 + dx_123 - si_183;
            }
        }
    }
l0800_2D88:
    cxOut = cx;
    bxOut = bx;
    return ax_129;
}


Eq_194 __write(struct Eq_22 * ds, Eq_194 wArg04, Eq_194 ptrArg06, Eq_194 wArg0A, union Eq_194 & cxOut, union Eq_194 & bxOut)
{
    Eq_194 bx_10 = wArg04 << 0x01;
    Eq_194 wLoc04_64;
    Eq_194 ax_39;
    if (((*((char *) &ds->t0478 + 2))[wArg04] & 0x01) != 0x00)
        wLoc04_64.u0 = 0x05;
    else
    {
        bx_10 = wArg04;
        Eq_194 cx = wArg0A;
        Eq_194 ax_24;
        if (!msdos_write_file(wArg04, wArg0A, ptrArg06, out ax_24))
        {
            (*((char *) &ds->t0478 + 2))[wArg04] |= 0x1000;
            bx_10 = wArg04 << 0x01;
            ax_39 = ax_24;
l0800_2DC8:
            cxOut = cx;
            bxOut = bx_10;
            return ax_39;
        }
        wLoc04_64 = ax_24;
    }
    ax_39 = __IOERROR(ds, wLoc04_64);
    goto l0800_2DC8;
}


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
    _printf(cx_148, dx_150, bx_147, ds_149, &ds_149->t020A);
    <unknown> Eq_6333::* fp;
    word16 di_1509;
    _fread(ds_149, fp - 42, 0x18, 0x01, dx_ax_1507, out di_1509);
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
    uint16 dx_299 = (uint16) bLoc13;
    struct Eq_22 * cx_319 = SEQ(ch_318, 0x08);
    _printf(cx_319, dx_299 << 0x08, ax_146, ds_149, (char *) &ds_149->t020A + 6);
    _printf(cx_319, dx_299 << 0x08, ax_146, ds_149, (char *) &ds_149->t020A + 26);
    _printf(cx_319, dx_299 << 0x08, ax_146, ds_149, (char *) &ds_149->t020A + 48);
    _printf(cx_319, dx_299 << 0x08, ax_146, ds_149, (char *) &ds_149->t020A + 70);
    _printf(cx_319, dx_299 << 0x08, ax_146, ds_149, (char *) &ds_149->t020A + 92);
    _printf(cx_319, dx_299 << 0x08, ax_146, ds_149, (char *) &ds_149->t020A + 113);
    Eq_3416 dx_ax_1508 = _fopen(ds_149, ptrArg06->ptr0008, (char *) &ds_149->t020A + 0x0091, out cx_436, out bx_435, out ds_437);
    uint16 ax_305 = (uint16) bLoc14 + (dx_299 << 0x08) + di_276;
    ui16 dx_438 = SLICE(dx_ax_1508, word16, 16);
    byte bLoc12_1355 = (byte) ax_305;
    if (dx_ax_1508 == 0x00)
    {
        _printf(cx_436, dx_438, bx_435, ds_437, &ds_437->t029E);
        _exit(ds_437, 0x01);
    }
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
    cup16 si_571;
    for (si_571 = 0x00; si_571 < 0xFF; ++si_571)
    {
        word16 dx_1516;
        word16 bx_1517;
        word16 di_1518;
        _wrVZbyte(ds_437, dx_ax_1508, 0x80, out dx_1516, out bx_1517, out di_1518);
    }
    cup16 si_608;
    for (si_608 = 0x00; si_608 < 0x05; ++si_608)
    {
        word16 dx_1519;
        word16 bx_1520;
        word16 di_1521;
        _wrVZbyte(ds_437, dx_ax_1508, ~0x01, out dx_1519, out bx_1520, out di_1521);
    }
    byte bLoc15;
    word16 dx_1522;
    word16 bx_1523;
    word16 di_1524;
    _wrVZbyte(ds_437, dx_ax_1508, bLoc15, out dx_1522, out bx_1523, out di_1524);
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
    word16 dx_1544;
    word16 bx_1545;
    word16 di_1546;
    _wrVZbyte(ds_437, dx_ax_1508, bLoc0E_1390, out dx_1544, out bx_1545, out di_1546);
    struct Eq_22 * cx_1022 = _wrVZbyte(ds_437, dx_ax_1508, (byte) ((wLoc0E_1374 & 0xFF00) >> 0x08), out dx_1023, out bx_1021, out di_1547);
    cup16 si_1032 = 0x00;
    while (si_1032 < 22050)
    {
        word16 si_1020;
        word16 di_1548;
        cx_1022 = _fwrite(ds_437, fp - 11, 0x01, 0x01, dx_ax_1508, out dx_1023, out bx_1021, out si_1020, out di_1548);
        si_1032 = si_1020 + 0x01;
    }
    _printf(cx_1022, dx_1023, bx_1021, ds_437, (char *) &ds_437->t029E + 25);
    _printf(ds_437, dx_1023, bx_1021, ds_437, (char *) &ds_437->t029E + 31);
    struct Eq_22 * ds_1082;
    _fclose(ds_437, dx_ax_1508, out ds_1082);
    selector ds_1549;
    _fclose(ds_1082, dx_ax_1507, out ds_1549);
}
