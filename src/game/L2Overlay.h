// L2Overlay.h
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

#ifndef L2OVERLAY_H
#define L2OVERLAY_H

#include <vector>

#include "SDL/SDL_types.h"

using std::vector;

/**
 */
class L2Overlay
{
public:
	/** Create and size an overlay with a fixed number of images */
    L2Overlay(Uint8 numimages);
    /** Get all images in the overlay */
    Uint8 getImages(Uint32** images, Sint8** xoffs, Sint8** yoffs);
    
    Uint16 cellpos;
    vector<Uint32> imagenums;
    vector<Sint8> xoffsets;
    vector<Sint8> yoffsets;
    Uint8 numimages;
};

#endif //L2OVERLAY_H
