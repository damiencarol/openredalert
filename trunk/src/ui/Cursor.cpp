#include "Cursor.h"

#include <cstdlib>
#include <cstring>

#include "SDL/SDL_timer.h"

#include "include/config.h"
#include "video/GraphicsEngine.h"
#include "include/sdllayer.h"
#include "CursorPool.h"
#include "CursorInfo.h"
#include "RA_Label.h"
#include "include/Logger.h"
#include "video/Dune2Image.h"
#include "video/TemplateImage.h"
#include "misc/gametypes.h"

namespace pc {
	extern GraphicsEngine * gfxeng;
}
extern Logger * logger;

/** 
 * Constructor, load the cursorimage and set upp all surfaces to buffer
 * background in etc.
 */
Cursor::Cursor() :
	currentcursor(0xffff), x(0), y(0), curimg(0), nimgs(0), ci(0), transw(0),
			transy(0), transr(0) 
{
	// Init some Tooltip stuff
	TooltipSurface = NULL;
	FG_color = SDL_MapRGB(pc::gfxeng->get_SDL_ScreenSurface()->format, 222, 217, 149);
	BG_color = SDL_MapRGB(pc::gfxeng->get_SDL_ScreenSurface()->format, 0, 0, 0);
	CK_color = SDL_MapRGB(pc::gfxeng->get_SDL_ScreenSurface()->format, 255, 255, 255);
	//	ToolTipLabel[0].setcolor (222, 217, 149);
	//	ToolTipLabel[1].setcolor (222, 217, 149);
	ToolTipLabel[0].setColor(246, 210, 123);
	ToolTipLabel[1].setColor(246, 210, 123);

	ToolTipLabel[0].SetFont("6point.fnt");
	ToolTipLabel[1].SetFont("6point.fnt");

	cursorimg = new Dune2Image("mouse.shp", -1);
	if (getConfig().gamenum == GAME_RA) {
		transicn = new TemplateImage("trans.icn", mapscaleq, 1);
		nsoff = 123;
	} else {
		transicn = new TemplateImage("trans.icn", mapscaleq);
		nsoff = 129;
	}

	cursorpool = new CursorPool("cursors.ini");

	// Load the first simple cursor
	setCursor("STANDARD");
	x = 0;
	y = 0;

	old_x = old_y = 0;

	/* All cursors loaded */
	reloadImages();
}

/** 
 * Destructor, free the surfaces
 */
Cursor::~Cursor()
{
	
#ifdef USE_TOOLTIP
	if (TooltipSurface != NULL)
	SDL_FreeSurface( TooltipSurface );
	TooltipSurface = NULL;
#endif

	// Free all surface image
	for (Uint8 i = 0; i < nimgs; ++i) {
		SDL_FreeSurface(image[i]);
	}

	SDL_FreeSurface(transw);
	SDL_FreeSurface(transy);
	SDL_FreeSurface(transr);

	delete cursorimg;
	delete transicn;
	delete cursorpool;
}

/** 
 * Change active cursor.
 * 
 * @param number of the new cursor.
 */
void Cursor::setCursor(Uint16 cursornum, Uint8 animimages) 
{
	int i;

	//Don't load this cursor again
	if (currentcursor == cursornum){
		return;
	}
	
	for (i = 0; i < nimgs; ++i) {
		SDL_FreeSurface(image[i]);
	}

	curimg = 0;
	nimgs = animimages;
	for (i = 0; i < nimgs; ++i)
		image[i] = cursorimg->getImage(cursornum+i);

	if (cursornum != 0) {
		cursor_offset = -((image[0]->w)>>1);
	} else {
		cursor_offset = 0;
	}

	currentcursor = cursornum;
}

void Cursor::setCursor(const char* curname) 
{
	//	printf ("%s line %i: Set cursor %s\n", __FILE__, __LINE__, curname);

	ci = cursorpool->getCursorByName(curname);

	//	printf ("%s line %i: Ci start = %i, Ci end = %i\n", __FILE__, __LINE__, ci->anstart,  ci->anend);

	setCursor(ci->anstart, ci->anend - ci->anstart + 1);
}

void Cursor::setPlaceCursor(Uint8 stw, Uint8 sth, Uint8 *icn) 
{
	int i, x, y;
	SDL_Surface *bigimg;

	Uint8 *data;
	SDL_Rect dest;
	Uint32 newptr;

	newptr = 0;
	for (i = 0; i < stw*sth; i++) {
		newptr = newptr<<2|(icn[i]);
	}

	if (currentcursor == 250 && newptr == oldptr) {
		return;
	}
	oldptr = newptr;

	for (i = 0; i < nimgs; i++) {
		SDL_FreeSurface(image[i]);
	}

	curimg = 0;
	currentcursor = 250;
	nimgs = 1;
	cursor_offset = 0;

	data = new Uint8[stw*sth*transw->w*transw->h];
	memset(data, 0, stw*sth*transw->w*transw->h);

	bigimg = SDL_CreateRGBSurfaceFrom(data, stw*transw->w, sth*transw->h, 16,
			stw*transw->w, 0, 0, 0, 0);

	SDL_Surface * tmp;
	tmp = SDL_DisplayFormat(bigimg);
	SDL_FreeSurface(bigimg);
	bigimg = tmp;

	SDL_SetColors(bigimg, SHPBase::getPalette(0), 0, 256);
	SDL_SetColorKey(bigimg, SDL_SRCCOLORKEY, 0);

	image[0] = bigimg; //SDL_DisplayFormat(bigimg);
	//    image[0] = SDL_DisplayFormat(bigimg);
	//    SDL_FreeSurface(bigimg);
	delete[] data;

	dest.w = transw->w;
	dest.h = transw->h;

	dest.y = 0;
	for (y = 0; y < sth; y++) {
		dest.x = 0;
		for (x = 0; x < stw; x++) {
			SDL_Surface** tile = 0;
			if (icn[y*stw+x] == 0) {
				dest.x += dest.w;
				continue;
			}
			switch (icn[y*stw+x]) {
			case 1:
				tile = &transw;
				break;
			case 2:
				tile = &transy;
				break;
			case 4:
				tile = &transr;
				break;
			default:
#if _MSC_VER && _MSC_VER < 1300
				logger->error("Possible memory corruption detected in %s(%d): icn[%i*%i+%i] = %i\n",__FILE__,__LINE__,y,stw,x,icn[y*stw+x]);
#else
				logger->error(
						"Possible memory corruption detected in %s: icn[%i*%i+%i] = %i\n",
						__FUNCTION__, y, stw, x, icn[y*stw+x]);
#endif
				//throw InvalidValue();
				throw 0;
				break;
			}
			SDL_BlitSurface(*tile, NULL, image[0], &dest);
			dest.x += dest.w;
		}
		dest.y += dest.h;
	}
}

SDL_Surface *Cursor::getCursor()
{
	static Uint32 lastchange = 0;
	Uint32 tick = SDL_GetTicks();
	if (tick > lastchange +100) {
		curimg++;
		if (curimg >= nimgs)
			curimg = 0;
		lastchange = tick;
	}
	return image[curimg];
}

void Cursor::reloadImages()
{
	int cursornum;

	// Free all the old images
	SDL_FreeSurface(transw);
	transw = NULL;
	SDL_FreeSurface(transy);
	transy = NULL;
	SDL_FreeSurface(transr);
	transr = NULL;

	// Reload stuff
	transw = transicn->getImage(0);
	transy = transicn->getImage(1);
	transr = transicn->getImage(2);

	//	SDL_Surface* tmp = SDL_DisplayFormatAlpha(transw);
	//	SDL_FreeSurface(transw);
	//	transw = tmp;

	//Reload cursor image array
	if (currentcursor != 250) {
		//If its not a place cursor, invalid current cursor, and reload it
		cursornum = currentcursor;
		currentcursor = 0xffff;
		setCursor(cursornum, nimgs);
	}
}

SDL_Surface *Cursor::getTooltip()
{
	return TooltipSurface;
}

void Cursor::setTooltip(std::string TipText)
{
	Uint16 w, h;
	//Font		ToolTipFont("type.fnt");
	SDL_Surface *tmp;
	int splitpos = 0;
	std::string FirstString, SecondString;

	if (TooltipSurface != NULL){
		return;
	}
	
	for (unsigned int i = 0; i < TipText.size(); i++) {
		if (TipText[i] == '\n') {
			splitpos = i;
			break;
		}
	}

	if (splitpos != 0) {
		//
		// We have two lines to draw in the tooltip
		//

		ToolTipLabel[0].setText(TipText.substr( 0, splitpos));
		ToolTipLabel[1].setText(TipText.substr(splitpos+1));

		h = 2*ToolTipLabel[0].getHeight() + 6;
		if (ToolTipLabel[0].getWidth() > ToolTipLabel[1].getWidth())
			w = ToolTipLabel[0].getWidth() + 10;
		else
			w = ToolTipLabel[1].getWidth() + 10;

		// Create the tooltip surface
		TooltipSurface
				= SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, w, h, 16, 0, 0, 0, 0);

		if (TooltipSurface == NULL)
			return;

		tmp = SDL_DisplayFormat(TooltipSurface);
		SDL_FreeSurface(TooltipSurface);
		TooltipSurface = tmp;

		Uint32 TopTextWidth, BottumTextWidth, TextHeigth;

		// First fill the surface with the color key color
		SDLLayer::draw_solidrectangle(TooltipSurface, CK_color, 0, 0, TooltipSurface->w,
				TooltipSurface->h);

		TopTextWidth = ToolTipLabel[0].getWidth() + 10;
		BottumTextWidth = ToolTipLabel[1].getWidth() + 10;
		TextHeigth = ToolTipLabel[0].getHeight() + 3; // Text heigth including edges, should be the same for the top and bottum text

		// Fill the part of the surface that is for the first string with the background color
		SDLLayer::draw_solidrectangle(TooltipSurface, BG_color, 0, 0, TopTextWidth,
				TextHeigth);

		// Fill the part of the surface that is for the second string with the background color
		SDLLayer::draw_solidrectangle(TooltipSurface, BG_color, 0, TextHeigth,
				BottumTextWidth, TextHeigth);

		// Draw the top-edge color
		SDLLayer::draw_rectangle(TooltipSurface, FG_color, 0, 0, TopTextWidth,
				1, 1);
		SDLLayer::draw_rectangle(TooltipSurface, FG_color, TopTextWidth,
				TextHeigth-1, BottumTextWidth - TopTextWidth, 1, 1);

		// Draw the left-edge color
		SDLLayer::draw_rectangle(TooltipSurface, FG_color, 0, 0, 1,
				TooltipSurface->h, 1);

		// Draw the bottum-edge color
		SDLLayer::draw_rectangle(TooltipSurface, FG_color, 0, TooltipSurface->h
				-1, BottumTextWidth, 1, 1);
		SDLLayer::draw_rectangle(TooltipSurface, FG_color, BottumTextWidth,
				TextHeigth, (TooltipSurface->w -(ToolTipLabel[1].getWidth()
						+ 10)), 1, 1);

		// Draw the rigth-edge color
		SDLLayer::draw_rectangle(TooltipSurface, FG_color, TopTextWidth-1, 0,
				1, TextHeigth, 1);
		SDLLayer::draw_rectangle(TooltipSurface, FG_color,
				(ToolTipLabel[1].getWidth() + 10)-1, TextHeigth, 1, TextHeigth,
				1);

		tmp = SDL_DisplayFormat(TooltipSurface);
		SDL_FreeSurface(TooltipSurface);
		TooltipSurface = tmp;

		ToolTipLabel[0].Draw(TooltipSurface, 5, 2);
		ToolTipLabel[1].Draw(TooltipSurface, 5, ToolTipLabel[0].getHeight()+4);
		SDL_SetColorKey(TooltipSurface, SDL_SRCCOLORKEY, CK_color);

	} else {

		//
		// We have one lines to draw in the tooltip
		//

		ToolTipLabel[0].setText(TipText);

		h = ToolTipLabel[0].getHeight() + 4;
		w = ToolTipLabel[0].getWidth() + 10;

		// Create the tooltip surface
		TooltipSurface
				= SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, w, h, 16, 0, 0, 0, 0);

		if (TooltipSurface == NULL)
			return;

		tmp = SDL_DisplayFormat(TooltipSurface);
		SDL_FreeSurface(TooltipSurface);
		TooltipSurface = tmp;

		// Fill the surface with the background color
		SDLLayer::draw_solidrectangle(TooltipSurface, BG_color, 0, 0, TooltipSurface->w,
				TooltipSurface->h);

		// Draw the edge color
		SDLLayer::draw_rectangle(TooltipSurface, FG_color, 0, 0, TooltipSurface->w,
				TooltipSurface->h);

		tmp = SDL_DisplayFormat(TooltipSurface);
		SDL_FreeSurface(TooltipSurface);
		TooltipSurface = tmp;

		ToolTipLabel[0].Draw(TooltipSurface, (TooltipSurface->w
				- ToolTipLabel[0].getWidth())/2, (TooltipSurface->h
				- ToolTipLabel[0].getHeight())/2);
	}
}

Uint8 Cursor::nsoff;

void Cursor::setXY(Uint16 nx, Uint16 ny) 
{
	x = nx;
	y = ny;

	if (x != old_x || y != old_y) {
		old_x = x;
		old_y = y;
		if (TooltipSurface != NULL) {
			SDL_FreeSurface(TooltipSurface);
			TooltipSurface = NULL;
		}
	}
}
