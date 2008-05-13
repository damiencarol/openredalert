// mode: -*- C++ -*-
/*****************************************************************************
 * fcnc_endian.h - Includes macro definitions to convert bytes to correct
 * endianess
 *       Author: Kareem Dana
 ****************************************************************************/
#ifndef FCNC_ENDIAN_H
#define FCNC_ENDIAN_H

#include <stdio.h>
#include "SDL/SDL_endian.h"
#include "video/Renderer.h"


#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define readbyte(x,y) x[y]
#define readword(x,y) x[y] + (x[y+1] << 8)
#define readthree(x,y)  x[y] + (x[y+1] << 8) + (x[y+2] << 16) + (0 << 24)
#define readlong(x,y) x[y] + (x[y+1] << 8) + (x[y+2] << 16) + (x[y+3] << 24)
#else
#define readbyte(x,y) x[y]
#define readword(x,y) SDL_Swap16((x[y] << 8) ^ x[y+1])
#define readthree(x,y) SDL_Swap32((x[y] << 24) ^ (x[y+1] << 16) ^ (x[y+2] << 8))
#define readlong(x,y) SDL_Swap32((x[y] << 24) ^ (x[y+1] << 16) ^ (x[y+2] << 8) ^ (x[y+3]))
#endif

static __inline__ Uint8 freadbyte(FILE *fptr)
{
    Uint8 x;
    fread(&x,1,1,fptr);
    return x;
}

static __inline__ Uint16 freadword(FILE *fptr)
{
    Uint16 x;
    fread(&x,2,1,fptr);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    return SDL_Swap16(x);
#else
    return x;
#endif
}

static __inline__ Uint32 freadthree(FILE *fptr)
{
    // Can this be made better?
    Uint8 x[3];
    fread(x,3,1,fptr);
    return readthree(x,0);
}

static __inline__ Uint32 freadlong(FILE *fptr)
{
    Uint32 x;
    fread(&x, 4, 1, fptr);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    return SDL_Swap32(x);
#else
    return x;
#endif
}

#endif /* FCNC_ENDIAN_H */
