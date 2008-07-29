// Blowfish.cpp
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

#include "blowfish.h"

#include <cstring>

#include "SDL/SDL_endian.h"

void Cblowfish::set_key(const Uint8* key, Uint32 cb_key)
{
    int i, j;
    Uint32 datal, datar;

    memcpy(m_p, bfp, sizeof(t_bf_p));
    memcpy(m_s, bfs, sizeof(t_bf_s));
    j = 0;
    for (i = 0; i < 18; i++) {
        int a = key[j++];
        j %= cb_key;
        int b = key[j++];
        j %= cb_key;
        int c = key[j++];
        j %= cb_key;
        int d = key[j++];
        j %= cb_key;
        m_p[i] ^= a << 24 | b << 16 | c << 8 | d;
    }

    datal = datar = 0;

    for (i = 0; i < 18;) {
        encipher(datal, datar);

        m_p[i++] = datal;
        m_p[i++] = datar;
    }

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 256;) {
            encipher(datal, datar);

            m_s[i][j++] = datal;
            m_s[i][j++] = datar;
        }
    }
}

inline Uint32 Cblowfish::S(Uint32 x, int i) const
{
    return m_s[i][(x >> ((3 - i) << 3)) & 0xff];
}

inline Uint32 Cblowfish::bf_f(Uint32 x) const
{
    return ((S(x, 0) + S(x, 1)) ^ S(x, 2)) + S(x, 3);
}

inline void Cblowfish::ROUND(Uint32& a, Uint32 b, int n) const
{
    a ^= bf_f(b) ^ m_p[n];
}


void Cblowfish::encipher(Uint32& xl, Uint32& xr) const
{
    Uint32 Xl = xl;
    Uint32 Xr = xr;

    Xl ^= m_p[0];
    ROUND (Xr, Xl, 1);
    ROUND (Xl, Xr, 2);
    ROUND (Xr, Xl, 3);
    ROUND (Xl, Xr, 4);
    ROUND (Xr, Xl, 5);
    ROUND (Xl, Xr, 6);
    ROUND (Xr, Xl, 7);
    ROUND (Xl, Xr, 8);
    ROUND (Xr, Xl, 9);
    ROUND (Xl, Xr, 10);
    ROUND (Xr, Xl, 11);
    ROUND (Xl, Xr, 12);
    ROUND (Xr, Xl, 13);
    ROUND (Xl, Xr, 14);
    ROUND (Xr, Xl, 15);
    ROUND (Xl, Xr, 16);
    Xr ^= m_p[17];

    xr = Xl;
    xl = Xr;
}

void Cblowfish::decipher(Uint32& xl, Uint32& xr) const
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32  Xl = SDL_Swap32(xl);
    Uint32  Xr = SDL_Swap32(xr);
#else
    Uint32  Xl = xl;
    Uint32  Xr = xr;
#endif

    Xl ^= m_p[17];
    ROUND (Xr, Xl, 16);
    ROUND (Xl, Xr, 15);
    ROUND (Xr, Xl, 14);
    ROUND (Xl, Xr, 13);
    ROUND (Xr, Xl, 12);
    ROUND (Xl, Xr, 11);
    ROUND (Xr, Xl, 10);
    ROUND (Xl, Xr, 9);
    ROUND (Xr, Xl, 8);
    ROUND (Xl, Xr, 7);
    ROUND (Xr, Xl, 6);
    ROUND (Xl, Xr, 5);
    ROUND (Xr, Xl, 4);
    ROUND (Xl, Xr, 3);
    ROUND (Xr, Xl, 2);
    ROUND (Xl, Xr, 1);
    Xr ^= m_p[0];



#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    xl = SDL_Swap32(Xr);
    xr = SDL_Swap32(Xl);
#else
    xl = Xr;
    xr = Xl;
#endif
}

static inline Uint32 reverse(Uint32 v)
{
    return SDL_Swap32(v);
    /*_asm
    {
     mov        eax, v
     xchg    al, ah
     rol        eax, 16
     xchg    al, ah
     mov        v, eax
    }
     return v;
     */

}

void Cblowfish::encipher(const void* s, void* d, Uint32 size) const
{
    const Uint32* r = reinterpret_cast<const Uint32*>(s);
    Uint32* w = reinterpret_cast<Uint32*>(d);
    size >>= 3;
    while (size--) {
        Uint32 a = reverse(*r++);
        Uint32 b = reverse(*r++);
        encipher(a, b);
        *w++ = reverse(a);
        *w++ = reverse(b);
    }
}

void Cblowfish::decipher(const void* s, void* d, int size) const
{
    const Uint32* r = reinterpret_cast<const Uint32*>(s);
    Uint32* w = reinterpret_cast<Uint32*>(d);
    size >>= 3;
    while (size--) {
        Uint32 a = reverse(*r++);
        Uint32 b = reverse(*r++);
        decipher(a, b);
        *w++ = reverse(a);
        *w++ = reverse(b);
    }
}
