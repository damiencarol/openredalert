// RadarAnimEvent.cpp
// 1.3

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

#include "RadarAnimEvent.h"

#include <stdexcept>
#include <string>

#include "ActionEventQueue.h"
#include "video/ImageCache.h"
#include "video/ImageCacheEntry.h"
#include "video/SHPImage.h"
#include "ui/Sidebar.h"
#include "game/pside.h"
#include "game/Player.h"

using std::string;
using std::runtime_error;

namespace p {
	extern ActionEventQueue* aequeue;
}

/**
 * Build an anim 
 * 
 * @param mode If mode = 0 => RADAR ON 
 *             If mode = 1 => RADAR OFF
 * @param isBad If isBad = true => Player is bad side 
 *              If isBad = false => Player is good side
 */
RadarAnimEvent::RadarAnimEvent(Uint8 mode, Sidebar* theSidebar)
    : ActionEvent(1)
{
	// Set the sidebar
	this->sidebar = theSidebar;
	
	// Get the mode
	this->mode = mode;
	
	// Set the start and end number in the anim 
    switch (mode) {
    case 0:
        frame = 0;
        framend = 25;
        break;
    case 1:
        frame = 22;
        framend = 42;
        break;
    default:
        frame = 0;
    }


    // Load the image for logo BAD
    logoRadarBad = new SHPImage("uradrfrm.shp", -1);
    
    // Load the image for logo GOOD
    logoRadarGood = new SHPImage("nradrfrm.shp", -1);
    
    
    // Load the image for anim BAD
    animImagesBad = new SHPImage("ussrradr.shp", -1);
	
    // Load the image for anim GOOD
    animImagesGood = new SHPImage("natoradr.shp", -1);
    

    // Schedule this
    p::aequeue->scheduleEvent(this);
}

/**
 * Run the anim
 */
void RadarAnimEvent::run()
{
	SDL_Surface* radarFrame = 0;
	SDL_Surface* shadow = 0;
	
	SDL_Rect dest;
	dest.x = sidebar->radarlocation.x;
	dest.y = sidebar->radarlocation.y;
	dest.h = sidebar->radarlocation.h;
	dest.w = sidebar->radarlocation.w;

    if (frame <= framend) 
    {
        //If the sidebar is null don't even bother
        if (sidebar->sbar != 0) 
        {
        	// If the player of the sidebar is GOOD or BAD
        	if ((sidebar->getPlayer()->getSide() &~PS_MULTI) == PS_BAD)
        	{
        		animImagesBad->getImage(frame, &radarFrame, &shadow, 0);
        	} else {
        		animImagesGood->getImage(frame, &radarFrame, &shadow, 0);
        	}
        	//SDL_free 
        	dest.h = radarFrame->h;
            //SDL_FillRect(pc::sidebar->sbar, &dest, SDL_MapRGB(pc::sidebar->sbar->format, 0x0a, 0x0a, 0x0a));
			SDL_BlitSurface(radarFrame, 0, sidebar->sbar, &dest);
        }

        ++frame;
        // Set delay of 1 ?s between 2 images
        setDelay(2);
        
        p::aequeue->scheduleEvent(this);
        // Draw special icons
    	sidebar->DrawSpecialIcons();
    } else {
        if (mode == 0) 
        {
        	// We got radar
        	if (sidebar->sbar != 0) 
        	{
        		//printf ("%s line %i: Draw last radar image (radar up, mode = %i)\n", __FILE__, __LINE__, mode);
        		// If the player of the sidebar is GOOD or BAD
        		if ((sidebar->getPlayer()->getSide() &~PS_MULTI) == PS_BAD)
        		{
        			logoRadarBad->getImage(1, &radarFrame, &shadow, 0);
        		} else {
        			logoRadarGood->getImage(1, &radarFrame, &shadow, 0);
        		}
        		//radarFrame = pc::imgcache->getImage(pc::sidebar->radarlogo, 1).image;
        		SDL_SetColorKey(radarFrame,SDL_SRCCOLORKEY,  0xffffff);
        		dest.h = radarFrame->h;
        		SDL_BlitSurface(radarFrame, 0, sidebar->sbar, &dest);
        	}
        } else {
        	// We don't have radar anymore
        	if (sidebar->sbar != 0) 
        	{
        		//printf ("%s line %i: Draw last radar image (radar down, mode = %i)\n", __FILE__, __LINE__, mode);
        		// If the player of the sidebar is GOOD or BAD
        		if ((sidebar->getPlayer()->getSide() &~PS_MULTI) == PS_BAD)
        		{
        			animImagesBad->getImage(0, &radarFrame, &shadow, 0);
        		} else {
        			animImagesGood->getImage(0, &radarFrame, &shadow, 0);
        		}
        		SDL_SetColorKey(radarFrame,SDL_SRCCOLORKEY, 0xffffff);
        		dest.h = radarFrame->h;
        		SDL_BlitSurface(radarFrame, 0, sidebar->sbar, &dest);
        	}
        }
        sidebar->radaranimating = false;
        sidebar->radaranim = 0;
    	sidebar->DrawSpecialIcons(); // Refresh
        delete this;
        return;
    }
}


