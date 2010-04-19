/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

// Adapted from FFmpeg.

/*
 * Common bit i/o utils
 * Copyright (c) 2000, 2001 Fabrice Bellard
 * Copyright (c) 2002-2004 Michael Niedermayer <michaelni@gmx.at>
 * Copyright (c) 2010 Loren Merritt
 *
 * alternative bitstream reader & writer by Michael Niedermayer <michaelni@gmx.at>
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file graphics/video/get_bits.h
 * Bitstream API.
 */

#include "get_bits.h"

const uint8 ff_log2_run[32]={
 0, 0, 0, 0, 1, 1, 1, 1,
 2, 2, 2, 2, 3, 3, 3, 3,
 4, 4, 5, 5, 6, 6, 7, 7,
 8, 9,10,11,12,13,14,15
};

/* VLC decoding */

#define GET_DATA(v, table, i, wrap, size) \
{\
    const uint8 *ptr = (const uint8 *)table + i * wrap;\
    switch(size) {\
    case 1:\
        v = *(const uint8 *)ptr;\
        break;\
    case 2:\
        v = *(const uint16 *)ptr;\
        break;\
    default:\
        v = *(const uint32 *)ptr;\
        break;\
    }\
}


static int alloc_table(VLC *vlc, int size, int use_static)
{
    int index;
    index = vlc->table_size;
    vlc->table_size += size;
    if (vlc->table_size > vlc->table_allocated) {
        if(use_static)
            abort(); //cant do anything, init_vlc() is used with too little memory
        vlc->table_allocated += (1 << vlc->bits);
        vlc->table = (int16 (*)[2]) realloc(vlc->table,
                             sizeof(VLC_TYPE) * 2 * vlc->table_allocated);
        if (!vlc->table)
            return -1;
    }
    return index;
}

typedef struct {
    uint8 bits;
    uint16 symbol;
    /** codeword, with the first bit-to-be-read in the msb
     * (even if intended for a little-endian bitstream reader) */
    uint32 code;
} VLCcode;

static int compare_vlcspec(const void *a, const void *b)
{
    const VLCcode *sa=(const VLCcode *)a, *sb=(const VLCcode *)b;
    return (sa->code >> 1) - (sb->code >> 1);
}

/**
 * Build VLC decoding tables suitable for use with get_vlc().
 *
 * @param vlc            the context to be initted
 *
 * @param table_nb_bits  max length of vlc codes to store directly in this table
 *                       (Longer codes are delegated to subtables.)
 *
 * @param nb_codes       number of elements in codes[]
 *
 * @param codes          descriptions of the vlc codes
 *                       These must be ordered such that codes going into the same subtable are contiguous.
 *                       Sorting by VLCcode.code is sufficient, though not necessary.
 */
static int build_table(VLC *vlc, int table_nb_bits, int nb_codes,
                       VLCcode *codes, int flags)
{
    int table_size, table_index, index, code_prefix, symbol, subtable_bits;
    int i, j, k, n, nb, inc;
    uint32 code;
    VLC_TYPE (*table)[2];

    table_size = 1 << table_nb_bits;
    table_index = alloc_table(vlc, table_size, flags & INIT_VLC_USE_NEW_STATIC);

    if (table_index < 0)
        return -1;
    table = &vlc->table[table_index];

    for (i = 0; i < table_size; i++) {
        table[i][1] = 0; //bits
        table[i][0] = -1; //codes
    }

    /* first pass: map codes and compute auxillary table sizes */
    for (i = 0; i < nb_codes; i++) {
        n = codes[i].bits;
        code = codes[i].code;
        symbol = codes[i].symbol;

        if (n <= table_nb_bits) {
            /* no need to add another table */
            j = code >> (32 - table_nb_bits);
            nb = 1 << (table_nb_bits - n);
            inc = 1;
            if (flags & INIT_VLC_LE) {
                j = SWAP_BYTES_32(code);
                inc = 1 << n;
            }
            for (k = 0; k < nb; k++) {

                if (table[j][1] /*bits*/ != 0) {
                    warning("incorrect codes");
                    return -1;
                }
                table[j][1] = n; //bits
                table[j][0] = symbol;
                j += inc;
            }
        } else {
            /* fill auxiliary table recursively */
            n -= table_nb_bits;
            code_prefix = code >> (32 - table_nb_bits);
            subtable_bits = n;
            codes[i].bits = n;
            codes[i].code = code << table_nb_bits;
            for (k = i+1; k < nb_codes; k++) {
                n = codes[k].bits - table_nb_bits;
                if (n <= 0)
                    break;
                code = codes[k].code;
                if ( ((uint32) (code >> (32 - table_nb_bits))) != ((uint32) code_prefix))
                    break;
                codes[k].bits = n;
                codes[k].code = code << table_nb_bits;
                subtable_bits = MAX(subtable_bits, n);
            }
            subtable_bits = MIN(subtable_bits, table_nb_bits);
            j = (flags & INIT_VLC_LE) ? SWAP_BYTES_32(code_prefix) >> (32 - table_nb_bits) : code_prefix;
            table[j][1] = -subtable_bits;

            index = build_table(vlc, subtable_bits, k-i, codes+i, flags);
            if (index < 0)
                return -1;
            /* note: realloc has been done, so reload tables */
            table = &vlc->table[table_index];
            table[j][0] = index; //code
            i = k-1;
        }
    }
    return table_index;
}


/* Build VLC decoding tables suitable for use with get_vlc().

   'nb_bits' set thee decoding table size (2^nb_bits) entries. The
   bigger it is, the faster is the decoding. But it should not be too
   big to save memory and L1 cache. '9' is a good compromise.

   'nb_codes' : number of vlcs codes

   'bits' : table which gives the size (in bits) of each vlc code.

   'codes' : table which gives the bit pattern of of each vlc code.

   'symbols' : table which gives the values to be returned from get_vlc().

   'xxx_wrap' : give the number of bytes between each entry of the
   'bits' or 'codes' tables.

   'xxx_size' : gives the number of bytes of each entry of the 'bits'
   or 'codes' tables.

   'wrap' and 'size' allows to use any memory configuration and types
   (byte/word/long) to store the 'bits', 'codes', and 'symbols' tables.

   'use_static' should be set to 1 for tables, which should be freed
   with av_free_static(), 0 if free_vlc() will be used.
*/
int init_vlc_sparse(VLC *vlc, int nb_bits, int nb_codes,
             const void *bits, int bits_wrap, int bits_size,
             const void *codes, int codes_wrap, int codes_size,
             const void *symbols, int symbols_wrap, int symbols_size,
             int flags)
{
    VLCcode buf[nb_codes];
    int i, j;

    vlc->bits = nb_bits;
    if(flags & INIT_VLC_USE_NEW_STATIC){
        if(vlc->table_size && vlc->table_size == vlc->table_allocated){
            return 0;
        }else if(vlc->table_size){
            abort(); // fatal error, we are called on a partially initialized table
        }
    }else {
        vlc->table = NULL;
        vlc->table_allocated = 0;
        vlc->table_size = 0;
    }


    assert(symbols_size <= 2 || !symbols);
    j = 0;
#define COPY(condition)\
    for (i = 0; i < nb_codes; i++) {\
        GET_DATA(buf[j].bits, bits, i, bits_wrap, bits_size);\
        if (!(condition))\
            continue;\
        GET_DATA(buf[j].code, codes, i, codes_wrap, codes_size);\
        if (flags & INIT_VLC_LE)\
            buf[j].code = SWAP_BYTES_32(buf[j].code);\
        else\
            buf[j].code <<= 32 - buf[j].bits;\
        if (symbols)\
            GET_DATA(buf[j].symbol, symbols, i, symbols_wrap, symbols_size)\
        else\
            buf[j].symbol = i;\
        j++;\
    }
    COPY(buf[j].bits > nb_bits);
    // qsort is the slowest part of init_vlc, and could probably be improved or avoided
    qsort(buf, j, sizeof(VLCcode), compare_vlcspec);
    COPY(buf[j].bits && buf[j].bits <= nb_bits);
    nb_codes = j;

    if (build_table(vlc, nb_bits, nb_codes, buf, flags) < 0) {
        free(vlc->table);
        return -1;
    }
    if((flags & INIT_VLC_USE_NEW_STATIC) && vlc->table_size != vlc->table_allocated)
        warning("needed %d had %d", vlc->table_size, vlc->table_allocated);
    return 0;
}


void free_vlc(VLC *vlc)
{
    free(vlc->table);
}

