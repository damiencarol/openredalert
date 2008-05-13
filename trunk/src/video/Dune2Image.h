#ifndef DUNE2IMAGE_H
#define DUNE2IMAGE_H

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

#include "Dune2Header.h"
#include "SHPBase.h"

class Dune2Image : SHPBase
{
public:
    Dune2Image(const char *fname, Sint8 scaleq);
    ~Dune2Image();

    SDL_Surface* getImage(Uint16 imgnum);

private:
    Uint32 getD2Header(Uint16 imgnum);
    Uint8* shpdata;

    /** @link aggregationByValue*/
    Dune2Header lnkHeader;
};

#endif
