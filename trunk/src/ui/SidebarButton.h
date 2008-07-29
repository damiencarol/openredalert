// SidebarButton.h
// 1.2

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

#ifndef SIDEBARBUTTON_H
#define SIDEBARBUTTON_H

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

#include "RA_Label.h"

#define LEFT_SIDE_BUTTON 1
#define RIGHT_SIDE_BUTTON 2

/**
 * Button of the Sidebar
 */
class SidebarButton {
public:
    SidebarButton(Sint16 x, Sint16 y, const char* fname, Uint8 func,
            const char* theatre, Uint8 palnum);
    ~SidebarButton();
    
    void ChangeImage(const char* fname);
    void ChangeImage(const char* fname, Uint8 number);
    void ChangeImage(const char* fname, Uint8 number,  Uint8 side);
    SDL_Surface* getSurface() const ;
    SDL_Rect getRect() const ;
    Uint8 getFunction() const ;
    SDL_Surface* Fallback(const char* fname);

    void ReloadImage();
private:
    Uint32 picnum;
    SDL_Surface *pic;
    Uint8 function; 
    Uint8 palnum;
    const char* theatre;

    bool using_fallback;
    const char *fallbackfname;
    
    /** Destination rect (on the sidebar) */
    SDL_Rect picloc;	

	RA_Label FallbackLabel;
};

#endif //SIDEBARBUTTON_H
