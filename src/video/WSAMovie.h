// WSAMovie.h
// 1.0

//    This file is part of OpenRedAlert.
//
//    OpenRedAlert is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
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
