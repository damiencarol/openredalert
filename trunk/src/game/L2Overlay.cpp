#include "L2Overlay.h"
#include "SDL/SDL_types.h"
#include "InfantryGroup.h"
#include "StructureType.h"
#include "UnitType.h"
#include "UnitOrStructureType.h"
#include "MissionData.h"
#include "include/ccmap.h"

L2Overlay::L2Overlay(Uint8 numimages)
{
    this->numimages = numimages;
    imagenums.resize(numimages);
    xoffsets.resize(numimages);
    yoffsets.resize(numimages);
}
Uint8 L2Overlay::getImages(Uint32** images, Sint8** xoffs, Sint8** yoffs)
{
    Uint8 i;
    *images = new Uint32[numimages];
    *xoffs = new Sint8[numimages];
    *yoffs = new Sint8[numimages];
    for (i=0;i<numimages;++i) {
        (*images)[i] = imagenums[i];
        (*xoffs)[i] = xoffsets[i];
        (*yoffs)[i] = yoffsets[i];
    }
    return numimages;
}
