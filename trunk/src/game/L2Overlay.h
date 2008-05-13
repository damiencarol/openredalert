#ifndef L2OVERLAY_H
#define L2OVERLAY_H

#include <vector>

#include "SDL/SDL_types.h"

using std::vector;

struct L2Overlay
{
    L2Overlay(Uint8 numimages);
    Uint8 getImages(Uint32** images, Sint8** xoffs, Sint8** yoffs);
    std::vector<Uint32> imagenums;
    Uint16 cellpos;
    std::vector<Sint8> xoffsets;
    std::vector<Sint8> yoffsets;
    Uint8 numimages;
};

#endif
