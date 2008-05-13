#ifndef CPSIMAGE_H
#define CPSIMAGE_H

#include "SDL/SDL_video.h"

#include "include/imageproc.h"
#include "CPSHeader.h"

//class VFile;
//class ImageProc;

class CPSImage
{
public:
    CPSImage(const char* fname, int scaleq);
    ~CPSImage();
    SDL_Surface* getImage();
private:
    void loadImage();
    void readPalette();
    Uint32 imgsize; Uint32 offset;
    Uint8* cpsdata;
    SDL_Color palette[256];

    /**
     * @link aggregationByValue 
     */
    CPSHeader lnkHeader;
    int scaleq;
    ImageProc lnkScaler;
    SDL_Surface* image;
};

#endif
