// SHPBase.cpp
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

#include "SHPBase.h"

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>

#include "misc/Compression.hpp"
#include "include/fcnc_endian.h"
#include "include/imageproc.h"
#include "misc/INIFile.h"
#include "include/Logger.h"
#include "vfs/vfs.h"
#include "video/ImageNotFound.h"
#include "include/config.h"
#include "video/headerformats.h"
#include "Palettes.h"

using std::string;
using std::runtime_error;

namespace pc {
	extern ConfigType Config;
}

//-----------------------------------------------------------------------------
// SHPBase
//-----------------------------------------------------------------------------

const Uint8 SHPBase::numpals = 9;
SDL_Color SHPBase::palette[32][256];

/**
 * Base class for all image based on shp file in archives
 */
SHPBase::SHPBase(const string& fname, Sint8 scaleq) :
	name(fname), 
    scaleq(scaleq), 
    scaler(new ImageProc()) 
{
}

/**
 * Base class for all image based on shp file in archives
 */
SHPBase::~SHPBase() 
{
	delete scaler;
}

void SHPBase::setPalette(SDL_Color *pal) 
{
	memcpy(palette[0], pal, 256*sizeof(SDL_Color));
	memcpy(palette[1], pal, 256*sizeof(SDL_Color));
	memcpy(palette[2], pal, 256*sizeof(SDL_Color));
	memcpy(palette[3], pal, 256*sizeof(SDL_Color));
	memcpy(palette[4], pal, 256*sizeof(SDL_Color));
	memcpy(palette[5], pal, 256*sizeof(SDL_Color));
	memcpy(palette[6], pal, 256*sizeof(SDL_Color));
	memcpy(palette[7], pal, 256*sizeof(SDL_Color));
	memcpy(palette[8], pal, 256*sizeof(SDL_Color));
	memcpy(palette[9], pal, 256*sizeof(SDL_Color)); // window background color palette red

	//if (pc::Config.gamenum == GAME_RA) {
		memcpy(palette[1]+80, col_greece, 16*sizeof(SDL_Color));
		memcpy(palette[2]+80, col_ussr, 16*sizeof(SDL_Color));
		memcpy(palette[3]+80, col_uk, 16*sizeof(SDL_Color));
		memcpy(palette[4]+80, col_spain, 16*sizeof(SDL_Color));
		memcpy(palette[5]+80, col_italy, 16*sizeof(SDL_Color));
		memcpy(palette[6]+80, col_germany, 16*sizeof(SDL_Color));
		memcpy(palette[7]+80, col_france, 16*sizeof(SDL_Color));
		memcpy(palette[8]+80, col_turkey, 16*sizeof(SDL_Color));
		memcpy(palette[9]+171, col_redwindow, 4*sizeof(SDL_Color));
	//}
}

/**
 * @todo DO NOTHING IN RA
 */
void SHPBase::calculatePalettes() 
{
	/*
	int i;
	if (pc::Config.gamenum == GAME_TD) {
		for (i = 0; i < 256; i++)
			palette[1][i] = palette[0][palone[i]];
		for (i = 0; i < 256; ++i)
			palette[2][i] = palette[0][paltwo[i]];
		for (i = 0; i < 256; ++i)
			palette[3][i] = palette[0][palthree[i]];
		for (i = 0; i < 256; ++i)
			palette[4][i] = palette[0][palfour[i]];
		for (i = 0; i < 256; ++i)
			palette[5][i] = palette[0][palfive[i]];
	}*/
}

SDL_Surface* SHPBase::scale(SDL_Surface *input, int quality) 
{
	return scaler->scale(input, quality);
}

SDL_Color* SHPBase::getPalette(Uint8 palnum) 
{
	return palette[palnum];
}

Uint8 SHPBase::numPalettes()
{
	return numpals;
}

const string& SHPBase::getFileName() const 
{
	return name;
}

Uint32 SHPBase::getColour(SDL_PixelFormat* fmt, Uint8 palnum, Uint16 index)
{
	SDL_Color p = palette[palnum][index];
	return SDL_MapRGB(fmt, p.r, p.g, p.b);
}
