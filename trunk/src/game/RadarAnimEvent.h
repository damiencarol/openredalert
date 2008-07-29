// RadarAnimEvent.h
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

#ifndef RADARANIMEVENT_H
#define RADARANIMEVENT_H

#include "SDL/SDL_types.h"

#include "ActionEvent.h"

class SHPImage;
class Sidebar;

/**
 * 
 */
class RadarAnimEvent : public ActionEvent
{
public:
    /**
     * if mode = 0 => RADAR ON
     * if mode = 1 => RADAR OFF
     */
    RadarAnimEvent(Uint8 mode, Sidebar* sidebar);
    void run();

private:
    Uint8 mode;
    Uint8 frame;
    Uint8 framend;

    Sidebar* sidebar;

    SHPImage* animImagesBad;
    SHPImage* logoRadarBad;

    SHPImage* animImagesGood;
    SHPImage* logoRadarGood;
};

#endif //RADARANIMEVENT_H
