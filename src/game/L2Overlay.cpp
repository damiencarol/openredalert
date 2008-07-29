// L2Overlay.cpp
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

#include "L2Overlay.h"

#include <vector>

#include "SDL/SDL_types.h"

using std::vector;

/**
 * Create and size an overlay with a fixed number of images
 */
L2Overlay::L2Overlay(Uint8 numimages) 
{
	this->numimages = numimages;
	this->imagenums.resize(numimages);
	this->xoffsets.resize(numimages);
	this->yoffsets.resize(numimages);
}

/**
 * Get all images in the overlay
 */
Uint8 L2Overlay::getImages(Uint32** images, Sint8** xoffs, Sint8** yoffs) 
{
	Uint8 i;
	*images = new Uint32[numimages];
	*xoffs = new Sint8[numimages];
	*yoffs = new Sint8[numimages];
	for (i=0; i<numimages; ++i) {
		(*images)[i] = imagenums[i];
		(*xoffs)[i] = xoffsets[i];
		(*yoffs)[i] = yoffsets[i];
	}
	return numimages;
}
