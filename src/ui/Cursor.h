// Cursor.h
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

#ifndef CURSOR_H
#define CURSOR_H

#include <string>

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

#include "RA_Label.h"

/// @todo Move these hardcoded values
#define MAX_CURS_IN_ANIM 24

#define CUR_NOANIM 1

#define CUR_STANDARD 0
#define CUR_SCROLLUP 1
#define CUR_SCROLLUR 2
#define CUR_SCROLLRIGHT 3
#define CUR_SCROLLDR 4
#define CUR_SCROLLDOWN 5
#define CUR_SCROLLDL 6
#define CUR_SCROLLLEFT 7
#define CUR_SCROLLUL 8
#define CUR_NOSCROLL_OFFSET 129
#define CUR_RA_NOSCROLL_OFFSET 123

#define CUR_PLACE 250

class CursorInfo;
class CursorPool;
class Dune2Image;
class TemplateImage;
class RA_Label;

using std::string;

/**
 * Cursor 
 */
class Cursor
{
public:
	Cursor();
	~Cursor();
		
	void setCursor(Uint16 cursornum, Uint8 animimages);
	void setCursor(const char* curname);
	void setPlaceCursor(Uint8 stw, Uint8 sth, Uint8 *icn);
	SDL_Surface *getCursor();/*{return image[curimg];}*/

	Uint16 getX();
	Uint16 getY();
	void setXY(Uint16 nx, Uint16 ny);

	static Uint8 getNoScrollOffset();

	void reloadImages();

	SDL_Surface* getTooltip();
	void setTooltip(string TipText);

private:
	Uint16 currentcursor;
	Uint16 x;
	Uint16 y;
	Uint16 old_x;
	Uint16 old_y;

	Uint8 curimg;
	Uint8 nimgs;

	/** Either CUR_RA_NOSCROLL_OFFSET or CUR_NOSCROLL_OFFSET */
	static Uint8 nsoff;

	SDL_Surface *image[24];

	Dune2Image *cursorimg;
	/** Image for building placement in game */
	TemplateImage *transicn;
	Sint16 cursor_offset;

	CursorPool* cursorpool;
	CursorInfo* ci;
	SDL_Surface* transw;
	SDL_Surface* transy;
	SDL_Surface* transr;

	/** Used by place cursor */
	Uint32 oldptr;

	RA_Label ToolTipLabel[2];

	//	std::string		ToolTipText;
	SDL_Surface *TooltipSurface;
	Uint32 FG_color;
	Uint32 BG_color;
	Uint32 CK_color;
};

#endif //CURSOR_H
