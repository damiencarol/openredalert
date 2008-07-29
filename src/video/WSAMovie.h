// WSAMovie.h
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

#ifndef WSAMOVIE_H
#define WSAMOVIE_H

#include <string>

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

#include "WSAHeader.h"
#include "GraphicsEngine.h"

using std::string;

/**
 * Movie in .WSA format
 * 
 * WSA files contain short animations and can be found in the GENERAL.MIX files.
 * They are basically a series of Format40 images, that are then compressed with
 * Format80.
 * 
 * The images are in Format40 but are then compressed with Format80.
 * That means that you first have to uncompress the Format80 and then decode 
 * the Format40 image you obtain.
 * The first frame should be xor-ed over a black image (filled with zeros), 
 * all the other are xor-ed over the previous one.
 */
class WSAMovie
{
public:
	WSAMovie(string fname);
	~WSAMovie();
	
	void animate(GraphicsEngine* grafEngine);

private:
	SDL_Surface *decodeFrame(Uint16 framenum);
	Uint8 *wsadata;
	Uint8 *framedata;
	/** Palette of the video (for frame) */
	SDL_Color palette[256];
	/** Whether WSA loops or not */
	Uint8 loop;
	char *sndfile;
	/** Header of the video */
	WSAHeader header;
};

#endif //WSAMOVIE_H
