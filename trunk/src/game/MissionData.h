// MissionData.h
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

#ifndef MISSIONDATA_H
#define MISSIONDATA_H

#include <string>

using std::string;

/**
 * Represent all information contains in the [Base] section of the inifile of the map
 */
class MissionData
{
public:
    MissionData();
    ~MissionData();

    /** the name of the map */
    string mapname;
    /** movie played after failed mission */
    string losemov;
    /** movie played after completed mission */
    string winmov;
    /** Specific music to play for this mission.*/
    string theme;
    /** The house of the player (Greece, USSR, England, etc) */
    string player;
    /** the actionmovie */
    string action;
    /** the briefing movie */
    string brief;
    /** the theater of the map (can be "SNOW", "WINTER" or "INTERIOR") */
    string theater;
    /** True if it's the last mission */
    bool endOfGame;
};

#endif //MISSIONDATA_H
