// MissionMapsClass.cpp
// 1.4

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

#include "MissionMapsClass.h"

#include <string>

#include "SDL/SDL_types.h"

#include "include/Logger.h"
#include "vfs/vfs.h"
#include "vfs/VFile.h"
#include "include/config.h"

using std::string;

extern Logger * logger;

MissionMapsClass::MissionMapsClass() 
{
	readMissionData();
}

string MissionMapsClass::getGdiMissionMap(Uint32 missionNumber)
{
	// If the index required is < 
	if (missionNumber < GdiMissionMaps.size()) {
		return GdiMissionMaps[missionNumber];
	}
	return NULL;
}

string MissionMapsClass::getNodMissionMap(Uint32 missionNumber)
{
	//
	if (missionNumber < NodMissionMaps.size()) {
		return NodMissionMaps[missionNumber];
	}
	return NULL;
}

void MissionMapsClass::readMissionData() 
{
	VFile *MapFile;
	char Line[255];
	string tmpString;
	Uint32 pos;
	Uint32 pos2;

	// I am not sure how the maps from td work so...
	if (getConfig().gamenum != GAME_RA) {
		return;
	}
	// get the offset and size of the binfile along with a 
	// pointer to it
	//binfile = mixes->getOffsetAndSize(binname, &offset, &size);
	MapFile = VFSUtils::VFS_Open("mission.ini");
	
	// Check if the file exist
	if (MapFile == 0) {
		logger->error("Unable to locate mission.ini file!\n");
		return;
	}
	
	// Parse all line of the file
	while (MapFile->getLine(Line, sizeof (Line))) 
	{
		// Get the string
		tmpString = Line;

		//memset (Line, '\0', sizeof (Line));

		int i = 0;
		while (tmpString[i] != '\0') {
			if (tmpString[i] == '[' || tmpString[i] == ']') {
				tmpString.erase(i, i+1);
			}
			i++;
		}
		// Check if the mission is availlable
		VFile* tmp = VFSUtils::VFS_Open(tmpString.c_str());
		
		// Does it exist ?
		if (tmp != 0)
		{
			// Close the file
			VFSUtils::VFS_Close(tmp);
		
			// For now we don't support the mission objective strings
			if (((pos = tmpString.find(".INI", 0)) != (Uint32)string::npos)
					&& 
				((pos2 = tmpString.find("A", 0)) != (Uint32)string::npos)) 
			{
				// remove ".ini" at the end of the string
				tmpString.erase(pos, pos+4);
				
				// If it's soviets mission
				if ((pos = tmpString.find("SCU", 0)) != (Uint32)string::npos)
				{
					// Add the mission in Allies Mission list
					NodMissionMaps.push_back(tmpString);
				} else {
					// Add the mission in Soviets Mission list
					GdiMissionMaps.push_back(tmpString);
				}
			}
		}
	}
}
