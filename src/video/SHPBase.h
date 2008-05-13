#ifndef SHPBASE_H
#define SHPBASE_H

#include <string>

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

class VFile;
class ImageProc;

using std::string;


class SHPBase {
public:
    SHPBase(const string& fname, Sint8 scaleq = -1);
    virtual ~SHPBase();

    static void setPalette(SDL_Color *pal);
    static void calculatePalettes();
    static SDL_Color* getPalette(Uint8 palnum) ;
    static Uint32 getColour(SDL_PixelFormat* fmt, Uint8 palnum, Uint16 index);
    static Uint8 numPalettes() ;

    SDL_Surface* scale(SDL_Surface *input, int quality);
    const string& getFileName() const ;

protected:
    static SDL_Color palette[32][256];
    static const Uint8 numpals;
    string name;
    Sint8 scaleq;
    ImageProc* scaler;
};

#endif //SHPBASE_H
