// ImageCacheEntry.h
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

#ifndef IMAGECACHEENTRY_H
#define IMAGECACHEENTRY_H

#include <string>

#include "SDL/SDL_video.h"

using std::string;

class ImageCacheEntry
{
public:
    ImageCacheEntry();
    ~ImageCacheEntry();
    
    void clear();
    
    /** Name of the SHP image file */
	string ImageName;
	/** Number of the image in the SHP image */
	int	NumbImages;
	/** Image */
    SDL_Surface *image;
    /** shadow of the image */
    SDL_Surface *shadow;
};

#endif //IMAGECACHEENTRY_H
