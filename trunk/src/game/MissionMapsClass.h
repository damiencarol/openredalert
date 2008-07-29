// MissionMapsClass.h
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

#ifndef MISSIONMAPSCLASS_H
#define MISSIONMAPSCLASS_H

#include <string>
#include <vector>

#include "SDL/SDL_types.h"

using std::string;
using std::vector;

/**
 *  Mission map class
 */
class MissionMapsClass {
public:
	MissionMapsClass();	
	
	string getGdiMissionMap(Uint32 missionNumber);
	string getNodMissionMap(Uint32 missionNumber);

private:
	void readMissionData();

	vector<string> Mapdata;
	vector<string> NodMissionMaps;
	vector<string> GdiMissionMaps;
	vector<string> MissionBriefing;
	vector<string> MultiPlayerMapNames;
};

#endif //MISSIONMAPSCLASS_H
