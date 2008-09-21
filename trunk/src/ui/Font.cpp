// Font.cpp
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

#include "Font.h"

#include <string>
#include <stdexcept>

#include "ui/FontCache.h"
#include "video/Renderer.h"
#include "include/fcnc_endian.h"
#include "include/Logger.h"
#include "include/sdllayer.h"
#include "vfs/vfs.h"
#include "vfs/VFile.h"


#define USE_FONTCACHE


using std::runtime_error;
using std::string;
using std::vector;


#ifdef USE_FONTCACHE
//vector <TFontImage> fontcache;
FontCache		fontCache;
#endif

Font::~Font() {
#ifndef USE_FONTCACHE
	if (fontimg != NULL){
		SDL_FreeSurface(fontimg);
    }
#endif
}


/**
 * Make a gues for the font palette
 */
SDL_Color font_pal1[] = {
		{0, 0, 0, 0x0},
		{17, 17, 17, 0x0},
		{34, 34, 34, 0x0},
		{51, 51, 51, 0x0},
		{68, 68, 68, 0x0},
		{85, 85, 85, 0x0},
		{102, 102, 102, 0x0},
		{119, 119, 119, 0x0},
		{136, 136, 136, 0x0},
		{153, 153, 153, 0x0},
		{170, 170, 170, 0x0},
		{187, 187, 187, 0x0},
		{204, 204, 204, 0x0},
		{221, 221, 221, 0x0},
		{238, 238, 238, 0x0},
		{255, 255, 255, 0x0}
};

/**
 * Make a gues for the font palette
 * Note the following is for a 16 bit color and should be rescaled to 24 bit. (example x /63 * 256)
 */
SDL_Color font_pal2[] = {
		{0, 0, 0, 0x0},
		{17, 17, 17, 0x0},
		{200, 200, 200, 0x0},
		{1, 1, 1, 0x0},		// Not used
		{1, 1, 1, 0x0},		// Not used
		{1, 1, 1, 0x0},		// Not used
		{1, 1, 1, 0x0},		// Not used
		{1, 1, 1, 0x0},		// Not used
		{1, 1, 1, 0x0},		// Not used
		{1, 1, 1, 0x0},		// Not used
		{1, 1, 1, 0x0},		// Not used
		{1, 1, 1, 0x0},		// Not used
		{1, 1, 1, 0x0},		// Not used
		{1, 1, 1, 0x0},		// Not used
		{1, 1, 1, 0x0},		// Not used
		{1, 1, 1, 0x0}		// Not used
};

SDL_Color font_pal3[] = {
		{0, 0, 0, 0x0},
		{17, 17, 17, 0x0},
		{150, 150, 150, 0x0},
		{200, 200, 200, 0x0},
		{200, 200, 200, 0x0},	// Not used
		{0, 0, 0, 0x0},		// Not used
		{0, 0, 0, 0x0},		// Not used
		{0, 0, 0, 0x0},		// Not used
		{1, 1, 1, 0x0},		// Not used
		{1, 1, 1, 0x0},		// Not used
		{1, 1, 1, 0x0},		// Not used
		{1, 1, 1, 0x0},		// Not used
		{1, 1, 1, 0x0},		// Not used
		{1, 1, 1, 0x0},		// Not used
		{1, 1, 1, 0x0},		// Not used
		{1, 1, 1, 0x0}		// Not used
};

void Font::reload()
{
	this->Load(this->fontname);
}

/**
 * @return height of the font
 */
unsigned int Font::getHeight() const
{
    return (unsigned  int)chrdest[0].h;
}

/**
 * At the moment there is a lot of overhead in reloading the font each time,
 * These new functions are a start at preventing that...
 */
bool Font::GetFontColor(SDL_Color FColor, SDL_Color BColor, SDL_Color OrgFntColor, SDL_Color &FntColor)
{
	Uint32 perc;

	//calc the percentage:
	perc = ( OrgFntColor.r * 100) / 255;

	if (fontname == "type.fnt" || fontname == "grad6fnt.fnt" || fontname == "vcr.fnt" /*|| fontname == "6point.fnt"*/)
	{
		perc = 100 - perc;
	}

	if (perc < 100){
		FntColor.r	= (Uint8)((double)FColor.r 	+ ((((double)BColor.r - (double)FColor.r)	* (double)perc)/100));
		FntColor.g	= (Uint8)((double)FColor.g	+ ((((double)BColor.g - (double)FColor.g)	* (double)perc)/100));
		FntColor.b	= (Uint8)((double)FColor.b	+ ((((double)BColor.b - (double)FColor.b)	* (double)perc)/100));
		return true;
	}
	return false;
}

/**
 * At the moment there is a lot of overhead in reloading the font each time,
 * These new functions are a start at preventing that...
 */
bool Font::GetFontColor(SDL_Color FColor, SDL_Color OrgFntColor, SDL_Color &FntColor)
{
	Uint32 perc;

	//calc the percentage:
	perc = ((OrgFntColor.r) * 100) / 255;

//	perc = 100 - perc;

	if ( fontname != "grad6fnt.fnt" && fontname != "vcr.fnt" ){
		perc = 100 - perc;
	}

	if (fontname == "help.fnt")
		printf ("perc = %u\n", perc);


	FntColor.r	= (Uint8)(((double)FColor.r * (double)perc)/(double)100);
	FntColor.g	= (Uint8)(((double)FColor.g * (double)perc)/(double)100);
	FntColor.b	= (Uint8)(((double)FColor.b * (double)perc)/(double)100);

	return true;
}


/**
 * Draw a colored character to a surface
 *
 * @note it's a DEBUGGING FUNCTION
 *
 * @param Character caractere to draw
 */
void Font::drawCHAR(const char Character)
{
	//Uint32 i;
	SDL_Color OrgColor/*, FontColor, BGcolor*/;
	SDL_Rect* src_rect;

	printf ("Printf font contents, char = %c, FontName = %s\n", Character, this->fontname.c_str());


	src_rect = const_cast<SDL_Rect*>(&chrdest[Character]);
	for (Uint32 y = 0; y < chrdest[0].h; y++){
		for (int x = 0; x < src_rect->w; x++){
			// Get the pixel from our source surface
			SDLLayer::get_pixel ( fontimg, OrgColor, src_rect->x + x, src_rect->y + y );
			printf ("%03i;", OrgColor.r);
		}
		printf ("\n");
	}
//	destr.x += src_rect->w+1;
}

void Font::UseAntiAliasing(bool status)
{
	lnkOptions.use_anitaliasing	= status;
}

void Font::underline(bool status)
{
	lnkOptions.underline = status;
}

void Font::double_underline(bool status)
{
	lnkOptions.double_underline	= status;
}

/*
Uint32 Font::Font(const std::string& text) const {
    Uint32 wdt = 0;
    Uint32 i;

    for (i = 0; text[i] != '\0'; i++){
        wdt += chrdest[text[i]].w+1;
    }
    return wdt;
}*/

/**
 * Draw a colored text to a surface
 *
 * This function gets the background color from the dest surface so
 * this function will not work if you draw over the same text each
 * time without clearing the background !!!
 *
 * I think this functions works correctly with the following fonts
 * (don't really know about the rest):
 * type.fnt
 * scorefnt.fnt
 * grad6fnt.fnt
 *
 * @param text is the text to draw to the surface.
 * @param dest is the destination surface
 * @param FGcolor is the font color
 * @param BGcolor is the background of the text
 * @param startx is the xpos on the surface where the text will be drawn
 * @param starty is the ypos on the surface where the text will be drawn
 */
void Font::drawText(const string& text, SDL_Surface *SrcSurf, Uint32 SrcStartx, Uint32 SrcStarty, SDL_Surface *DestSurf, SDL_Color FGcolor, Uint32 DestStartx, Uint32 DestStarty)
{
	Uint32 i;
	SDL_Rect destr;
	destr.x = DestStartx;
	destr.y = DestStarty;
	SDL_Color OrgColor, FontColor, BGcolor;
	SDL_Rect* src_rect;


	for( i = 0; text[i] != '\0'; i++ )
	{
		// We check if it's <0 to keep special chars
		int index = text[i];
		if (text[i]<0) index += 256;
		src_rect = const_cast<SDL_Rect*>(&chrdest[index]);
		// original
		//src_rect = const_cast<SDL_Rect*>(&chrdest[text[i]]);

		for (int x = 0; x < src_rect->w; x++){
			for (Uint32 y = 0; y < chrdest[0].h; y++){
				// Get the pixel from our source surface
				SDLLayer::get_pixel( fontimg, OrgColor, src_rect->x + x, src_rect->y + y );
				if (OrgColor.r != 0 || OrgColor.g != 0 || OrgColor.b != 0){
					// convert the org color to the correct font color
					if ( lnkOptions.use_anitaliasing ){
						// This is the normal thing to do (get the color of the pixel to draw in the anti aliasing way)
						SDLLayer::get_pixel ( SrcSurf, BGcolor, SrcStartx + destr.x + x, SrcStarty + destr.y + y );
						if (GetFontColor (FGcolor, BGcolor, OrgColor, FontColor))
							SDLLayer::set_pixel ( DestSurf, FontColor, destr.x + x, destr.y + y );
					}else{
						if (fontname == "scorefnt.fnt" ){
							memcpy (&FontColor, &FGcolor, sizeof (SDL_Color));
							SDLLayer::set_pixel ( DestSurf, FontColor, destr.x + x, destr.y + y );
						}else{
							if (GetFontColor (FGcolor, OrgColor, FontColor))
								SDLLayer::set_pixel ( DestSurf, FontColor, destr.x + x, destr.y + y );
						}
					}
				}
			}
		}
		destr.x += src_rect->w+1;
	}

	// Handle underlining
	if (lnkOptions.underline){
		Uint16 Length = destr.x -DestStartx;
		SDLLayer::draw_h_line ( DestSurf, FGcolor, 0, DestSurf->h - 4, Length, 1 );
	}

	// Handle double underlining
	if (lnkOptions.double_underline){
		Uint16 Length = destr.x -DestStartx;
		SDLLayer::draw_h_line ( DestSurf, FGcolor, 0, DestSurf->h - 4, Length, 1 );
		SDLLayer::draw_h_line ( DestSurf, FGcolor, 0, DestSurf->h - 2, Length, 1 );
	}
}

/**
 * @param FontName Name of the font to load
 */
void Font::Load(string FontName)
{
	VFile* fontfile;
	Uint16 wpos;
	Uint16 hpos;
	Uint16 cdata;
	Uint16 nchars;
	Uint8 fnheight;
	Uint8 fnmaxw;
	Uint32 fntotalw;
	Uint32 ypos;
	Uint32 i;
	Uint32 pos;
	Uint32 curchar;
	Uint8		data;
	SDL_Surface* OrgFonImg = 0;



	this->fontname = FontName;

#ifdef USE_FONTCACHE
	if (fontCache.Get (this->fontname, &this->fontimg, this->chrdest)){
		return;
	}
#endif //USE_FONTCACHE

	//logger->debug("%s line %i: Load the font: %s\n\n", __FILE__, __LINE__, FontName.c_str());
	fontfile = VFSUtils::VFS_Open(fontname.c_str());
	if (0 == fontfile) {
		string s("Unable to load font ");
		s += fontname;
		throw(runtime_error(s));
	}

	fontfile->seekSet(8);
	fontfile->readWord(&wpos, 1);
	fontfile->readWord(&cdata, 1);
	fontfile->readWord(&hpos, 1);
	fontfile->seekCur(2);
	fontfile->readWord(&nchars, 1);
	nchars = SDL_Swap16(nchars); // Yes, even on LE systems.
	fontfile->readByte(&fnheight, 1);
	fontfile->readByte(&fnmaxw, 1);

	nchars++;

	vector<Uint8> wchar(nchars);
	vector<Uint8> hchar(nchars<<1);

	vector<Uint16> dataoffsets(nchars);
	fontfile->readWord(&dataoffsets[0], nchars);

	fontfile->seekSet(wpos);
	fontfile->readByte(&wchar[0], nchars);
	fontfile->seekSet(hpos);
	fontfile->readByte(&hchar[0], nchars<<1);

	chrdest.resize(nchars);

	fntotalw = 0;
	for( i = 0 ; i<nchars; i++ ) {
		chrdest[i].x = fntotalw;
		chrdest[i].y = 0;
		chrdest[i].h = fnheight;
		chrdest[i].w = wchar[i];
		fntotalw += wchar[i];
	}
	vector<Uint8> chardata(fnheight*fntotalw);

	for( curchar = 0; curchar < nchars; curchar++ ) {
		fontfile->seekSet(dataoffsets[curchar]);
		for( ypos = hchar[curchar<<1]; ypos < (Uint32)(hchar[curchar<<1]+hchar[(curchar<<1)+1]); ypos++ ) {
			pos = chrdest[curchar].x+ypos*fntotalw;
			for( i = 0; i < wchar[curchar]; i+=2 ) {
				fontfile->readByte( &data, 1 );
#if 1
				/* Each 4 bits contain a index to the pallete */
				chardata[pos+i] = data&0xf;
				if( i+1<wchar[curchar] )
					chardata[pos+i+1] = (data>>4);
#else
				/* Each 4 bits contain a index to the pallete, convert them to 0 or 1 */
				chardata[pos+i] = (data&0xb)!=0?1:0;
				if( i+1<wchar[curchar] )
					chardata[pos+i+1] = (data>>4)!=0?1:0;
			//printf ("Data = %i\n", chardata[pos+i+1]);
#endif
			}
		}
	}

	SDL_FreeSurface(fontimg);
	fontimg = NULL;

	OrgFonImg = SDL_CreateRGBSurfaceFrom(&chardata[0], fntotalw, fnheight, 8, fntotalw, 0, 0, 0, 0);

	if (this->fontname == "6point.fnt" || this->fontname == "8point.fnt" || this->fontname == "3point.fnt")
		SDL_SetColors(OrgFonImg, font_pal2, 0, 16);
	else if ( this->fontname == "12metfnt.fnt")
		SDL_SetColors(OrgFonImg, font_pal3, 0, 16);
	else
		SDL_SetColors(OrgFonImg, font_pal1, 0, 16);

//	SDL_SetColorKey(OrgFonImg, SDL_SRCCOLORKEY, 0);

	fontimg = SDL_DisplayFormat(OrgFonImg);

	SDL_FreeSurface(OrgFonImg);
	OrgFonImg = NULL;


#ifdef USE_FONTCACHE
	fontCache.Add (this->fontname, this->fontimg,  chrdest);
#endif
	VFSUtils::VFS_Close(fontfile);
}

Uint32 Font::calcTextWidth(const string& text) const
{
    Uint32 wdt = 0;
    Uint32 i;

    for (i = 0; text[i] != '\0'; i++){
    	int index = text[i];
    	if (text[i]<0) index += 256;
        wdt += chrdest[index].w+1;
    }
    return wdt;
}

Font::Font(const string& fontname) : SHPBase(fontname), fontimg(0)
{
	// Set the image pointer to NULL
	fontimg = 0;

	lnkOptions.use_anitaliasing	= true;
	lnkOptions.underline		= false;
	lnkOptions.double_underline	= false;

	this->Load(fontname);
}
