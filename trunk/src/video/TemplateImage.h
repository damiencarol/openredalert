#ifndef TEMPLATEIMAGE_H
#define TEMPLATEIMAGE_H

#include <string>

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

#include "SHPBase.h"

class VFile;


class TemplateImage : SHPBase
{
public:
    TemplateImage(const char *fname, Sint8 scaleq, bool ratemp = false);
    ~TemplateImage();

    Uint16 getNumTiles();
    SDL_Surface* getImage(Uint16 imgnum);

private:
    bool ratemp;
    VFile* tmpfile;
};

#endif //TEMPLATEIMAGE_H
