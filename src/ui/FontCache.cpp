// FontCache.cpp
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

#include "FontCache.h"

#include <cstring>
#include <stdexcept>

#include "video/Renderer.h"
#include "include/fcnc_endian.h"
#include "include/Logger.h"
#include "vfs/vfs.h"
#include "include/sdllayer.h"
#include "TFontImage.h"
#include "Font.h"

using namespace std;

/** 
 * Constructor
 */
FontCache::FontCache (void)
{
	// We don't need to do this
	font_cache.empty();
}

/** 
 * Destructor
 */
FontCache::~FontCache (void)
{
	for (unsigned int i = 0; i < font_cache.size(); i++){
		if (font_cache[i].fontimg != NULL){
			SDL_FreeSurface (font_cache[i].fontimg);
		}
		font_cache[i].fontimg = NULL;
	}
}

/** 
 * Add a font to the cache
 */
void FontCache::Add (TFontImage Image)
{
	font_cache.push_back (Image);
}

/** 
 * Add a font to the cache
 */
void FontCache::Add (std::string fontname, SDL_Surface *img,  std::vector<SDL_Rect> chrdest)
{
	TFontImage FntImg;

	FntImg.fontname = fontname;
	FntImg.chrdest = chrdest;
	FntImg.fontimg = img;
	
	font_cache.push_back (FntImg);
}
/** Get a font from the cache*/
bool FontCache::Get (std::string fontname, SDL_Surface **fontimg,  std::vector<SDL_Rect> &chrdest)
{
	*fontimg = NULL;
	chrdest.empty();
	for (unsigned int i = 0; i < font_cache.size(); i++){
		if (font_cache[i].fontname == fontname){
			chrdest = font_cache[i].chrdest;
			*fontimg = font_cache[i].fontimg;
			//printf ("Found font in new cache\n");
			return true;;
		}
	}
	return false;
}
