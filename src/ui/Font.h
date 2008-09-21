// Font.h
// 1.4

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

#ifndef FONT_H
#define FONT_H

#include <string>
#include <vector>

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

#include "FontOptions.h"
#include "video/SHPBase.h"

class FontOptions;

using std::vector;

/**
 * The font class
 *
 * Possible fonts for ra...
 *
 * type.fnt
 * scorefnt.fnt
 * grad6fnt.fnt
 *
 * These are in redalert.mix --> submix == local.mix
 *
 * editfnt.fnt
 * 3point.fnt
 * 8point.fnt
 * led.fnt
 * vcr.fnt
 *
 *
 * These are in redalert.mix --> submix == hires.mix????
 *
 * 12metfnt.fnt
 * 6point.fnt
 * grad6fnt.fnt
 * help.fnt
 * scorefnt.fnt
 *
 *
 */
class Font : SHPBase
{
public:
	Font(const string& fontname);
	~Font();

	/** Get Height of the Font */
	unsigned int getHeight() const;

	Uint32 calcTextWidth(const string& text) const;
	bool GetFontColor(SDL_Color FColor, SDL_Color OrgFntColor,
			SDL_Color &FntColor);
	bool GetFontColor(SDL_Color FColor, SDL_Color BColor,
			SDL_Color OrgFntColor, SDL_Color &FntColor);
	void drawCHAR(const char Character);

	void drawText(const string& text, SDL_Surface* SrcSurf,
			Uint32 SrcStartx, Uint32 SrcStarty, SDL_Surface* DestSurf,
			SDL_Color FGcolor, Uint32 DestStartx, Uint32 DestStarty);

	void UseAntiAliasing(bool status);
	void underline(bool status);
	void double_underline(bool status);

	/** Load the specific font */
	void Load(string FontName);
	void reload();

private:
	SDL_Color FontPal[15];
	SDL_Surface *fontimg;
	vector<SDL_Rect> chrdest;
	string fontname;
	FontOptions lnkOptions;
};

#endif //FONT_H
