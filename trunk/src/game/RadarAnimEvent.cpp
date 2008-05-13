// RadarAnimEvent.cpp
// 1.3

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

#include "RadarAnimEvent.h"

#include <stdexcept>
#include <string>

#include "include/config.h"
#include "include/dispatcher.h"
#include "include/Input.h"
#include "video/ImageCache.h"
#include "include/Logger.h"
#include "audio/SoundEngine.h"
#include "include/PlayerPool.h"
#include "include/UnitAndStructurePool.h"
#include "game/Unit.h"
#include "vfs/vfs.h"
#include "include/sdllayer.h"
#include "ui/Sidebar.h"


using std::string;
using std::runtime_error;

namespace pc {
    extern ConfigType Config;
}


RadarAnimEvent::RadarAnimEvent(Uint8 mode, bool* minienable, Uint32 radar)
    : ActionEvent(1), minienable(minienable), radar(radar)
{
	// Get the mode
	this->mode = mode;
	
    switch (mode) {
    case 0:
        frame = 0;
        framend = 20;
        break;
    case 1:
        //*minienable = false;
        frame = 20;
        framend = 30;
        break;
    default:
        frame = 0;
    }


	if (pc::sidebar->sbar != NULL) {
		// Draw grey box where radar was
		// SDL_FillRect(pc::sidebar->sbar, dest, SDL_MapRGB(pc::sidebar->sbar->format, 0x0a, 0x0a, 0x0a));

		if (pc::Config.gamenum == GAME_TD){
			sdlRadar = pc::imgcache->getImage(pc::sidebar->radarlogo).image;
		} else {
			sdlRadar = pc::imgcache->getImage(pc::sidebar->radarlogo,1).image;
		}
	}
    p::aequeue->scheduleEvent(this);
}

void RadarAnimEvent::run()
{
	//SDL_Rect *dest = &pc::sidebar->radarlocation;
	SDL_Surface *radarFrame;
	SDL_Rect dest;
	dest.x = pc::sidebar->radarlocation.x;
	dest.y = pc::sidebar->radarlocation.y;
	dest.h = pc::sidebar->radarlocation.h;
	dest.w = pc::sidebar->radarlocation.w;

    if (frame <= framend) {

        //If the sidebar is null don't even bother
        if (pc::sidebar->sbar != NULL) {
            // Draw radar loading frame
            radarFrame = pc::imgcache->getImage(radar, frame).image;
            dest.h = radarFrame->h;
            //SDL_FillRect(pc::sidebar->sbar, &dest, SDL_MapRGB(pc::sidebar->sbar->format, 0x0a, 0x0a, 0x0a));
			SDL_BlitSurface(radarFrame, NULL, pc::sidebar->sbar, &dest);
        }

        ++frame;
        p::aequeue->scheduleEvent(this);
    	pc::sidebar->DrawSpecialIcons();
    } else {
        if (mode == 0) {
		// We got radar
		if (pc::sidebar->sbar != NULL) {
			printf ("%s line %i: Draw last radar image (radar up, mode = %i)\n", __FILE__, __LINE__, mode);
			radarFrame = pc::imgcache->getImage(pc::sidebar->radarlogo, 1).image;
			SDL_SetColorKey(radarFrame,SDL_SRCCOLORKEY,  0xffffff);
			dest.h = radarFrame->h;
			SDL_BlitSurface( radarFrame, NULL, pc::sidebar->sbar, &dest);
		}
		*minienable = true;
	}else{
		// We don't have radar anymore
		if (pc::sidebar->sbar != NULL) {
			printf ("%s line %i: Draw last radar image (radar down, mode = %i)\n", __FILE__, __LINE__, mode);
			radarFrame = pc::imgcache->getImage(radar, 0).image;
			SDL_SetColorKey(radarFrame,SDL_SRCCOLORKEY, 0xffffff);
			dest.h = radarFrame->h;
			SDL_BlitSurface( radarFrame, NULL, pc::sidebar->sbar, &dest);
		}
		*minienable = false;
	}
        pc::sidebar->radaranimating = false;
        pc::sidebar->radaranim = NULL;
    	pc::sidebar->DrawSpecialIcons();
        delete this;
        return;
    }
}
