// SidebarButton.cpp
// 1.5

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

#include "SidebarButton.h"

#include "SDL/SDL_types.h"

#include "Sidebar.h"
#include "video/ImageCache.h"
#include "video/ImageCacheEntry.h"
#include "video/ImageNotFound.h"

namespace pc {
	extern ImageCache* imgcache;
	extern Sidebar* sidebar;
}

/**
 * Create a SidebarButton
 */
SidebarButton::SidebarButton(int x, int y, const string& picname, Uint8 func,
		const string& theatre, Uint8 pal) :
	pic(0), function(func), palnum(pal), theatre(theatre), using_fallback(false)
{
	
	picloc.x = x;
	picloc.y = y;
	
	ChangeImage(picname);

	FallbackLabel.setColor(0xff, 0xff, 0xff);
	FallbackLabel.SetFont("grad6fnt.fnt");
	FallbackLabel.UseAntiAliasing(false);
}

/**
 */
SidebarButton::~SidebarButton()
{
	if (using_fallback)
	{
		SDL_FreeSurface(pic);
		using_fallback = false;
	}
}

/**
 * @param fname
 */
void SidebarButton::ChangeImage(const string& fname)
{
	ChangeImage(fname, 0, 1);
}

void SidebarButton::ChangeImage(const string& fname, Uint8 number)
{
	ChangeImage(fname, number, 1);
}

void SidebarButton::ChangeImage(const string& fname, Uint8 number, Uint8 side)
{
	// Error checking
	if (side != 1 && side != 2)
	{
		return;
	}

	if (using_fallback)
	{
		SDL_FreeSurface(pic);
		using_fallback = false;
	}

	try
	{
		picnum = pc::imgcache->loadImage(fname.c_str());
		picnum += number;
		picnum |= palnum<<11;
		
		if (side == 1){
			pic = pc::imgcache->getImage(picnum, 0).image;
		} else {
			pic = pc::imgcache->getImage(picnum, 1).image;
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

SDL_Surface* SidebarButton::Fallback(const string& fname)
{
	Uint32 width, height;

	using_fallback = true; // Ensures that the surface created gets destroyed later.
	fallbackfname = fname.c_str();

	string iname = fname.substr(0, 8);
	

	width = pc::sidebar->getGeom().bw;
	height = pc::sidebar->getGeom().bh;
	SDL_Surface* ret = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCCOLORKEY, width, height, 16, 0, 0, 0, 0);
	SDL_FillRect(ret, NULL, 0);
	//pc::sidebar->getFont()->drawText(iname,ret,0,0);
	FallbackLabel.Draw(iname, ret, 0, 0);
	
	return ret;
}

/**
 */
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

/**
 * @return surface with the picture of the button
 */
SDL_Surface* SidebarButton::getSurface() const
{
	return pic;
}

SDL_Rect SidebarButton::getRect() const
{
	return picloc;
}

unsigned int SidebarButton::getFunction() const
{
	return function;
}
