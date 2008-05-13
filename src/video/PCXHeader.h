#ifndef PCXHEADER_H
#define PCXHEADER_H

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

class PCXHeader
{
public:
    Uint8	Signature;
    Uint8	Version;
    Uint8	Encoding;
    Uint8	BitsPixel;
    Uint16	XMin;
    Uint16	YMin;
    Uint16	XMax;
    Uint16	YMax;
    Uint16	HRes;
    Uint16	VRes;
    SDL_Color palette[16];
    Uint8	Reserved;
    Uint8	NumPlanes;
    Uint16	BytesLine;
    /** 1 = Color or s/w, 2 = Black and white */
    Uint16	PalType;
    Uint16	HscreenSize;
    Uint16	VscreenSize;
    Uint8	Dummy[58];
};

#endif //PCXHEADER_H
