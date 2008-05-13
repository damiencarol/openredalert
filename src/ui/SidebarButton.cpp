// SidebarButton.cpp
// 1.2

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

#include "SidebarButton.h"

#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <string>

#include "SDL/SDL_types.h"

#include "include/config.h"
#include "video/ImageCache.h"
#include "video/ImageNotFound.h"
#include "Sidebar.h"

using std::string;
using std::runtime_error;
using std::vector;
using std::replace;

namespace pc
{
	extern ConfigType Config;
}

SidebarButton::SidebarButton(Sint16 x, Sint16 y, const char* picname, Uint8 f,
		const char* theatre, Uint8 pal) :
	pic(0), function(f), palnum(pal), theatre(theatre), using_fallback(false)
{
	picloc.x = x;
	picloc.y = y;
	ChangeImage(picname);

	FallbackLabel.setColor(0xff, 0xff, 0xff);
	FallbackLabel.SetFont("grad6fnt.fnt");
	FallbackLabel.UseAntiAliasing(false);
}

SidebarButton::~SidebarButton()
{
	if (using_fallback)
	{
		SDL_FreeSurface(pic);
		using_fallback = false;
	}
}

void SidebarButton::ChangeImage(const char* fname)
{
	ChangeImage(fname, 0, 1);
}

void SidebarButton::ChangeImage(const char* fname, Uint8 number)
{
	ChangeImage(fname, number, 1);
}

void SidebarButton::ChangeImage(const char* fname, Uint8 number, Uint8 side)
{
	const char* name;
	char goldname[32];
	Uint32 slen = strlen(fname);

	// Error checking
	if (side != 1 && side != 2){
		return;
	}

	if (using_fallback)
	{
		SDL_FreeSurface(pic);
		using_fallback = false;
	}

	if (pc::sidebar->isOriginalType() || pc::Config.gamenum == GAME_RA)
	{
		name = fname;
	}
	else
	{

		// TODO _stricmp(
		//if (slen>8 && strcmp(("icon.shp"), (fname+slen-8))==0) {		
		if (slen>8 && strcmp(("icon.shp"), (fname+slen-8))==0)
		{
			strcpy(goldname, fname);
			goldname[slen-6] = 'N';
			goldname[slen-5] = 'H';
			goldname[slen-3] = theatre[0];
			goldname[slen-2] = theatre[1];
			goldname[slen-1] = theatre[2];
		}
		else
		{
			sprintf(goldname, "H%s", fname);
		}
		name = goldname;
	}

	try
	{
		picnum = pc::imgcache->loadImage(name);
		picnum += number;
		picnum |= palnum<<11;
		if (pc::Config.gamenum == GAME_TD)
		{
			pic = pc::imgcache->getImage(picnum).image;
		}
		else if (pc::Config.gamenum == GAME_RA)
		{
			if (side == 1){
				pic = pc::imgcache->getImage(picnum, 0).image;
			} else {
				pic = pc::imgcache->getImage(picnum, 1).image;
			}
		}
	}
	catch(ImageNotFound&)
	{
		// This is only a temporary fix; the real solution is
		// to add a creation function to the imagecache, 
		// which would ahandle creation and would mean 
		// the imagecache would be responsible for deletion.
		// A nice side effect is that those changes will
		// be acompanied with a way to index created images
		// so we only create the fallback images once.
		pic = Fallback(fname);
	}

	picloc.w = pic->w;
	picloc.h = pic->h;

	if (picloc.h > 100)
	{
		picloc.h >>=2;
		//pic->h >>= 2;
	}
}

SDL_Surface* SidebarButton::Fallback(const char* fname)
{
	SDL_Surface* ret;
	Uint32 width, height;
	Uint32 slen = strlen(fname);
	char* iname = new char[slen-7];

	using_fallback = true; // Ensures that the surface created gets destroyed later.
	fallbackfname = fname;

	strncpy(iname, fname, slen-8);
	iname[slen-8] = 0;
	width = pc::sidebar->getGeom().bw;
	height = pc::sidebar->getGeom().bh;
	ret	= SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, width, height, 16, 0, 0, 0, 0);
	SDL_FillRect(ret, NULL, 0);
	//pc::sidebar->getFont()->drawText(iname,ret,0,0);
	FallbackLabel.Draw(iname, ret, 0, 0);
	delete[] iname;
	return ret;
}

void SidebarButton::ReloadImage()
{
	//If we are using fallback we must redraw
	if (using_fallback)
	{
		SDL_FreeSurface(pic);
		pic = Fallback(fallbackfname);
	}
	else
	{
		//else the image cache takes care of it
		pic = pc::imgcache->getImage(picnum).image;
	}
}

SDL_Surface* SidebarButton::getSurface() const
{
	return pic;
}

SDL_Rect SidebarButton::getRect() const
{
	return picloc;
}

Uint8 SidebarButton::getFunction() const
{
	return this->function;
}
