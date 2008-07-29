// ws-key.cpp
// 1.0

//    This file is part of OpenRedAlert.
//
//    OpenRedAlert is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, version 2 of the License.
//
//    OpenRedAlert is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with OpenRedAlert.  If not, see <http://www.gnu.org/licenses/>.

/*****************************************************************************
 * ws-key.cpp - code to decode Westwood's 80-byte encryption key
 *
 * Originally from Olaf van der Spek's XCC Mixer program.
 * Introduced to the FreeCNC code base by Kareem Dana
 * Various stylistic changes by Euan MacGregor
 *****************************************************************************/
#include "SDL/SDL_types.h"
#include "SDL/SDL_endian.h"
#include <cstring>
#include "ws-key.h"

const char* pubkey_str = "AihRvNoIbTn85FZRYNZRcT+i6KpU+maCsEqr3Q5q+LDB5tH7Tz2qQ38V";

const Sint8 char2num[] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

typedef Uint32 bignum4[4];
typedef Uint32 bignum[64];
typedef Uint32 bignum130[130];

struct pubkey_t {
    bignum key1;
    bignum key2;
    Uint32 len;
} pubkey;
bignum glob1;
Uint32 glob1_bitlen, glob1_len_x2;
bignum130 glob2;
bignum4 glob1_hi, glob1_hi_inv;
Uint32 glob1_hi_bitlen;
Uint32 glob1_hi_inv_lo, glob1_hi_inv_hi;

Uint16* endian_Uint32ToUint16ArracyCast (Uint32 *Input, Uint32 len)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	Uint16 *INPUT;

	for (Uint16 i = 0; i < len; i++){
		Input[i] = SDL_Swap32 (Input[i]);
	}

	INPUT = (Uint16 *)Input;

	for (Uint16 i = 0; i < len*2; i++){
		INPUT[i] = SDL_Swap16(INPUT[i]);
	}
	return (Uint16*) Input;
#else
	return (Uint16 *) Input;
#endif
}

Uint32* endian_Uint16ToUint32ArracyCast (Uint16 *Input, Uint32 len)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	Uint32 *INPUT;

	for (Uint16 i = 0; i < len*2; i++){
		Input[i] = SDL_Swap16(Input[i]);
	}

	INPUT = (Uint32 *)Input;

	for (Uint16 i = 0; i < len; i++){
		INPUT[i] = SDL_Swap32(INPUT[i]);
	}
	return (Uint32*) Input;
#else
	return (Uint32 *) Input;
#endif
}


void init_bignum(bignum n, Uint32 val, Uint32 len)
{
    memset((void *)n, 0, len * 4);
    n[0] = val;
}

void move_key_to_big(bignum n, char *key, Uint32 klen, Uint32 blen)
{
    Uint32 sign;
    int i;

    if (key[0] & 0x80)
        sign = 0xff;
    else
        sign = 0;

    for (i = blen*4; (unsigned int)i > klen; i--)
        ((char *)n)[i-1] = (char)sign;
    for (; i > 0; i--)
        ((char *)n)[i-1] = (char)key[klen-i];

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	for (int j = 0; j < 64; j++)
		n[j] = SDL_Swap32(n[j]);
#endif

}

void key_to_bignum(bignum n, char *key, Uint32 len)
{
    Uint32 keylen;
    int i;

    if (key[0] != 2)
        return;
    key++;

    if (key[0] & 0x80) {
        keylen = 0;
        for (i = 0; i < (key[0] & 0x7f); i++)
            keylen = (keylen << 8) | key[i+1];
        key += (key[0] & 0x7f) + 1;
    } else {
        keylen = key[0];
        key++;
    }
    if (keylen <= len*4)
        move_key_to_big(n, key, keylen, len);

}

Uint32 len_bignum(bignum n, Uint32 len)
{
    int i;
    i = len-1;
    while ((i >= 0) && (n[i] == 0))
        i--;
    return i+1;
}

Uint32 bitlen_bignum(bignum n, Uint32 len)
{
    Uint32 ddlen, bitlen, mask;
    ddlen = len_bignum(n, len);
    if (ddlen == 0)
        return 0;
    bitlen = ddlen * 32;
    mask = 0x80000000;
    while ((mask & n[ddlen-1]) == 0) {
        mask >>= 1;
        bitlen--;
    }

    return bitlen;
}

void init_pubkey()
{
    Uint32 i, i2, tmp;
    char keytmp[256];

    init_bignum(pubkey.key2, 0x10001, 64);

    i = 0;
    i2 = 0;
    while (i < strlen(pubkey_str)) {
        tmp = char2num[(Uint8)pubkey_str[i++]];
        tmp <<= 6;
        tmp |= char2num[(Uint8)pubkey_str[i++]];
        tmp <<= 6;
        tmp |= char2num[(Uint8)pubkey_str[i++]];
        tmp <<= 6;
        tmp |= char2num[(Uint8)pubkey_str[i++]];
        keytmp[i2++] = (char)((tmp >> 16) & 0xff);
        keytmp[i2++] = (char)((tmp >> 8) & 0xff);
        keytmp[i2++] = (char)(tmp & 0xff);
    }
    key_to_bignum(pubkey.key1, keytmp, 64);
    pubkey.len = bitlen_bignum(pubkey.key1, 64) - 1;
}

Uint32 len_predata()
{
    Uint32 a = (pubkey.len - 1) / 8;
    return (55 / a + 1) * (a + 1);
}

long int cmp_bignum(bignum n1, bignum n2, Uint32 len)
{
    n1 += len-1;
    n2 += len-1;
    while (len > 0) {
        if (*n1 < *n2)
            return -1;
        if (*n1 > *n2)
            return 1;
        n1--;
        n2--;
        len--;
    }
    return 0;
}

void mov_bignum(bignum dest, bignum src, Uint32 len)
{
    memmove(dest, src, len*4);
}

void shr_bignum(bignum n, Uint32 bits, long int len)
{
    Uint32 i, i2;

    i2 = bits / 32;
    if (i2 > 0) {
        for (i = 0; i < len - i2; i++)
            n[i] = n[i + i2];
        for (; i < (unsigned long int)len; i++)
            n[i] = 0;
        bits = bits % 32;
    }
    if (bits == 0)
        return;
    for (i = 0; i < (unsigned long int)len - 1; i++)
        n[i] = (n[i] >> bits) | (n[i + 1] << (32 - bits));
    n[i] = n[i] >> bits;

}

void shl_bignum(bignum n, Uint32 bits, Uint32 len)
{
    Uint32 i, i2;

    i2 = bits / 32;
    if (i2 > 0) {
        for (i = len - 1; i > i2; i--)
            n[i] = n[i - i2];
        for (; i > 0; i--)
            n[i] = 0;
        bits = bits % 32;
    }

    if (bits == 0)
        return;
    for (i = len - 1; i > 0; i--)
        n[i] = (n[i] << bits) | (n[i - 1] >> (32 -
                                              bits));
    n[0] <<= bits;

}

Uint32 sub_bignum(bignum dest, bignum src1, bignum src2, Uint32 carry, Sint64 len)
{
    Uint32 i1, i2;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	Sint64 Orglen = len;
	Uint16 *SRC1, *SRC2, *DEST;

	SRC1 = endian_Uint32ToUint16ArracyCast (src1, len);
	SRC2 = endian_Uint32ToUint16ArracyCast (src2, len);
	DEST = (Uint16*) dest;
    len += len;
#else
    len += len;
	Uint16 *SRC1, *SRC2, *DEST;
	SRC1 = (Uint16 *)src1;
	SRC2 = (Uint16 *)src2;
	DEST = (Uint16 *)dest;
#endif
    while ((long)--len != -1) {
        i1 = SRC1[0];
        i2 = SRC2[0];

		DEST[0] =  (unsigned short)(i1 - i2 - carry);

		SRC1++;
		SRC2++;
		DEST++;
        if ((i1 - i2 - carry) & 0x10000)
            carry = 1;
        else
            carry = 0;
    }
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	DEST = (Uint16*) dest;
	SRC2 = (Uint16*) src2;
	dest = endian_Uint16ToUint32ArracyCast (DEST, Orglen);
	src2 = endian_Uint16ToUint32ArracyCast (SRC2, Orglen);
#endif
    return carry;
}




Uint32 sub_bignum1(Uint16 *dest, Uint16* src1, bignum src2, Uint32 carry, Sint64 len)
{
    Uint32 i1, i2;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	Sint64 Orglen = len;
	Uint16 *SRC1,*SRC2, *DEST;

	SRC2 = endian_Uint32ToUint16ArracyCast (src2, len);
	DEST = dest;
    len += len;
#else
    len += len;
	Uint16 *SRC1, *SRC2, *DEST;
	SRC1 = (Uint16 *)src1;
	SRC2 = (Uint16 *)src2;
	DEST = (Uint16 *)dest;
#endif

    while ((long)--len != -1) {
        i1 = SRC1[0];
        i2 = SRC2[0];

		DEST[0] =  (unsigned short)(i1 - i2 - carry);
		SRC1++;
		SRC2++;
		DEST++;
        if ((i1 - i2 - carry) & 0x10000)
            carry = 1;
        else
            carry = 0;
    }

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	SRC2 = (Uint16*) src2;
	src2 = endian_Uint16ToUint32ArracyCast (SRC2, Orglen);
#endif
    return carry;
}




void inv_bignum(bignum n1, bignum n2, Uint32 len)
{
    bignum n_tmp;
    Uint32 n2_bytelen, bit;
    long int n2_bitlen;



    init_bignum(n_tmp, 0, len);
    init_bignum(n1, 0, len);
    n2_bitlen = bitlen_bignum(n2, len);
    bit = ((Uint32)1) << (n2_bitlen % 32);

    n1 += ((n2_bitlen + 32) / 32) - 1;

    n2_bytelen = ((n2_bitlen - 1) / 32) * 4;
    n_tmp[n2_bytelen / 4] |= ((Uint32)1) << ((n2_bitlen - 1) & 0x1f);
    while (n2_bitlen > 0) {
        n2_bitlen--;
        shl_bignum(n_tmp, 1, len);
        if (cmp_bignum(n_tmp, n2, len) != -1) {
            sub_bignum(n_tmp, n_tmp, n2, 0, len);
            *n1 |= bit;
        }
        bit >>= 1;
        if (bit == 0) {
            n1--;
            bit = 0x80000000;
        }
    }
    init_bignum(n_tmp, 0, len);
}

void inc_bignum(bignum n, Uint32 len)
{
    while ((++*n == 0) && (--len > 0))
        n++;
}

void init_two_dw(bignum n, Uint32 len)
{
    mov_bignum(glob1, n, len);
    glob1_bitlen = bitlen_bignum(glob1, len);
    glob1_len_x2 = (glob1_bitlen + 15) / 16;
    mov_bignum(glob1_hi, glob1 + len_bignum(glob1, len) - 2, 2);
    glob1_hi_bitlen = bitlen_bignum(glob1_hi, 2) - 32;
    shr_bignum(glob1_hi, glob1_hi_bitlen, 2);
    inv_bignum(glob1_hi_inv, glob1_hi, 2);
    shr_bignum(glob1_hi_inv, 1, 2);
    glob1_hi_bitlen = (glob1_hi_bitlen + 15) % 16 + 1;
    inc_bignum(glob1_hi_inv, 2);
    if (bitlen_bignum(glob1_hi_inv, 2) > 32) {
        shr_bignum(glob1_hi_inv, 1, 2);
        glob1_hi_bitlen--;
    }
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    glob1_hi_inv_lo = ((Uint16 )glob1_hi_inv[0]);
    glob1_hi_inv_hi = (((Uint16 )(glob1_hi_inv[0]>>16)) );
#else
    glob1_hi_inv_lo = *(Uint16 *)glob1_hi_inv;
    glob1_hi_inv_hi = *(((Uint16 *)glob1_hi_inv) + 1);
#endif
}

void mul_bignum_word(Uint16* n1, bignum n2, Uint32 mul, Uint32 len)
{
    Uint32 i, tmp;
	Uint16 *N2;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    bignum N22;

	for (Uint32 i = 0; i < len/2; i++){
		N22[i] = SDL_Swap32(n2[i]);
	}

	N2 = (Uint16 *)N22;

	for (Uint32 i = 0; i < len; i++){
		N2[i] = SDL_Swap16(N2[i]);
	}
#else
	N2 = (Uint16 *)n2;
#endif
    tmp = 0;
    for (i = 0; i < len; i++) {
        tmp = mul * N2[0] + n1[0] + tmp;
        n1[0] = (unsigned short)tmp;
		n1++;
		N2++;
        tmp >>= 16;
    }
    n1[0] += (unsigned short)tmp;
}



void mul_bignum(bignum dest, bignum src1, bignum src2, Uint32 len)
{
    Uint32 i;

	Uint16 *SRC2, *DEST;

    init_bignum(dest, 0, len*2);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    bignum SRC22, DEST3;

	/// @HACK This probably isn't the correct place to do this :)
    for (i = 0; i < len; i++) {
		src1[i] = SDL_Swap32(src1[i]);
		src2[i] = SDL_Swap32(src2[i]);
	}

	for (Uint32 i = 0; i < len*2; i++){
		SRC22[i] = SDL_Swap32(src2[i]);
		DEST3[i] = SDL_Swap32(dest[i]);
	}

	SRC2 = (Uint16 *)SRC22;
	DEST = (Uint16 *)DEST3;

	for (Uint32 i = 0; i < len*2; i++){
		SRC2[i] = SDL_Swap16(SRC2[i]);
	}
#else
	SRC2 = (Uint16 *)src2;
	DEST = (Uint16 *)dest;
#endif


    for (i = 0; i < len*2; i++) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        mul_bignum_word(DEST, src1, SRC2[0], len*2);
		SRC2++;
		DEST++;
#else
        mul_bignum_word(DEST, src1, SRC2[0], len*2);
		SRC2++;
		DEST++;
#endif
    }

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
DEST = (Uint16 *)DEST3;

	for (Uint32 i = 0; i < len*4; i++){
		DEST[i] = SDL_Swap16(DEST[i]);
	}

    for (i = 0; i < len*2; i++) {
		dest[i] = SDL_Swap32(DEST3[i]);
	}
#endif
}


void mul_bignum1(bignum dest, bignum src1, bignum src2, Uint32 len)
{
    Uint32 i;

	Uint16 *SRC2, *DEST;

    init_bignum(dest, 0, len*2);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    bignum SRC22, DEST3;

	for (Uint32 i = 0; i < len*2; i++){
		SRC22[i] = SDL_Swap32(src2[i]);
		DEST3[i] = SDL_Swap32(dest[i]);
	}

	SRC2 = (Uint16 *)SRC22;
	DEST = (Uint16 *)DEST3;

	for (Uint32 i = 0; i < len*2; i++){
		SRC2[i] = SDL_Swap16(SRC2[i]);
	}
#else
	SRC2 = (Uint16 *)src2;
	DEST = (Uint16 *)dest;
#endif


    for (i = 0; i < len*2; i++) {
        mul_bignum_word(DEST, src1, SRC2[0], len*2);
		SRC2++;
		DEST++;
    }

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
DEST = (Uint16 *)DEST3;

	for (Uint32 i = 0; i < len*4; i++){
		DEST[i] = SDL_Swap16(DEST[i]);
	}

    for (i = 0; i < len*2; i++) {
		dest[i] = SDL_Swap32(DEST3[i]);
	}
#endif
}


void not_bignum(bignum n, Uint32 len)
{
    Uint32 i;
    for (i = 0; i < len; i++) {
        *n = ~*n;
        ++n;
    }
}

void neg_bignum(bignum n, Uint32 len)
{
    not_bignum(n, len);
    inc_bignum(n, len);
}

Uint32 get_mulword(Uint16 *wn)
{
    Uint32 i;

   i = (((((((((wn[-1] ^ 0xffff) & 0xffff) * glob1_hi_inv_lo + 0x10000) >> 1)
             + (((wn[-2] ^ 0xffff) * glob1_hi_inv_hi + glob1_hi_inv_hi) >> 1) + 1)
            >> 16) + ((((wn[-1] ^ 0xffff) & 0xffff) * glob1_hi_inv_hi) >> 1) +
           (((wn[0] ^ 0xffff) * glob1_hi_inv_lo) >> 1) + 1) >> 14) + glob1_hi_inv_hi
         * (wn[0] ^ 0xffff) * 2) >> glob1_hi_bitlen;

    if (i > 0xffff)
        i = 0xffff;
    return i & 0xffff;
}

void dec_bignum(bignum n, Uint32 len)
{
    while ((--*n == 0xffffffff) && (--len > 0))
        n++;
}


void calc_a_bignum(bignum n1, bignum n2, bignum n3, Uint32 len)
{
    Uint32 g2_len_x2, len_diff;
    Uint16 *esi, *edi, *glb2;
    Uint16 tmp;

    mul_bignum1(glob2, n2, n3, len);

    glob2[len*2] = 0;
    g2_len_x2 = len_bignum(glob2, len*2+1)*2;

    if (g2_len_x2 >= glob1_len_x2) {
       inc_bignum(glob2, len*2+1);
       neg_bignum(glob2, len*2+1);
       len_diff = g2_len_x2 + 1 - glob1_len_x2;

		#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		glb2 = endian_Uint32ToUint16ArracyCast (glob2, 64);
		#else
		glb2 = (Uint16*) glob2;
		#endif

        esi = glb2 + (1 + g2_len_x2 - glob1_len_x2);
        edi = glb2 + (g2_len_x2 + 1);
        for (; len_diff != 0; len_diff--) {
            edi--;
            tmp = (Uint16)get_mulword(edi);
            esi--;
            if (tmp > 0) {
                mul_bignum_word(esi, glob1, tmp, 2*len);;
                if ((*edi & 0x8000) == 0) {
                    if (sub_bignum1(esi, esi, glob1, 0, len)){
                        (*edi)--;
					}
                }
            }
        }
        neg_bignum(glob2, len);
        dec_bignum(glob2, len);
		#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		for (int j = 0; j < 64; j++){
			glob2[j] = SDL_Swap32(glob2[j]);
		}
		Uint16 *tmp = (Uint16*)glob2;
		for (int j = 0; j < 64; j++){
			tmp[j] = SDL_Swap16(tmp[j]);
		}
		#endif
    }
    mov_bignum(n1, glob2, len);
}



void clear_tmp_vars(Uint32 len)
{
    init_bignum(glob1, 0, len);
    init_bignum(glob2, 0, len);
    init_bignum(glob1_hi_inv, 0, 4);
    init_bignum(glob1_hi, 0, 4);
    glob1_bitlen = 0;
    glob1_hi_bitlen = 0;
    glob1_len_x2 = 0;
    glob1_hi_inv_lo = 0;
    glob1_hi_inv_hi = 0;
}

void calc_a_key(bignum n1, bignum n2, bignum n3, bignum n4, Uint32 len)
{
    bignum n_tmp;
    Uint32 n3_len, n4_len, bit_mask;
	Sint64 n3_bitlen;

    init_bignum(n1, 1, len);
    n4_len = len_bignum(n4, len);
    init_two_dw(n4, n4_len);
    n3_bitlen = bitlen_bignum(n3, n4_len);
    n3_len = (n3_bitlen + 31) / 32;
    bit_mask = (((Uint32)1) << ((n3_bitlen - 1) % 32)) >> 1;
    n3 += n3_len - 1;
    n3_bitlen--;
    mov_bignum(n1, n2, n4_len);
    while ((long)--n3_bitlen != -1) {
        if (bit_mask == 0) {
            bit_mask = 0x80000000;
            n3--;
        }
        calc_a_bignum(n_tmp, n1, n1, n4_len);

        if (*n3 & bit_mask){
            calc_a_bignum(n1, n_tmp, n2, n4_len);
        }else{
            mov_bignum(n1, n_tmp, n4_len);

		}
        bit_mask >>= 1;
    }


    init_bignum(n_tmp, 0, n4_len);
    clear_tmp_vars(len);

}

void process_predata(const Uint8* pre, Uint32 pre_len, Uint8 *buf)
{
    bignum n2, n3;
    const Uint32 a = (pubkey.len - 1) / 8;


    while (a + 1 <= pre_len) {
        init_bignum(n2, 0, 64);
        memmove(n2, pre, a + 1);

		#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		for (int j = 0; j < 64; j++){
			n2[j] = SDL_Swap32(n2[j]);
		}
		#endif

        calc_a_key(n3, n2, pubkey.key2, pubkey.key1, 64);

		#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		for (int j = 0; j < 64; j++){
			n3[j] = SDL_Swap32(n3[j]);
		}
		#endif

        memmove(buf, n3, a);

        pre_len -= a + 1;
        pre += a + 1;
        buf += a;
    }
}

void get_blowfish_key(const Uint8* s, Uint8* d)
{
    static bool public_key_initialized = false;
    if (!public_key_initialized) {
        init_pubkey();
        public_key_initialized = true;
    }
    Uint8 key[256];

    process_predata(s, len_predata(), key);

    memcpy(d, key, 56);
}
