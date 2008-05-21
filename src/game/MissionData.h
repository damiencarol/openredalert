// MissionData.h
// 1.0

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

#ifndef MISSIONDATA_H
#define MISSIONDATA_H

#include "SDL/SDL_types.h"

class MissionData {
public:
	~MissionData();
	MissionData();
	
	/** the name of the map */
	char* mapname;
	/** movie played after failed mission */
	char* losemov;
	/** movie played after completed mission */
	char* winmov;
	/** Specific music to play for this mission.*/
	char* theme;
	/** The house of the player (Greece, USSR, England, etc.) */
	char* player;
	/** the actionmovie */
	char* action;
	/** the briefing movie */
	char* brief;
	/** the theater of the map (eg. winter) */
	char* theater;
	/** True if it's the last mission */
	bool endOfGame; 
};

#endif //MISSIONDATA_H
