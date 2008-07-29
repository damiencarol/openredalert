// MultiPlayerMaps.cpp
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

#include "MultiPlayerMaps.h"

#include <string>

#include "include/Logger.h"
#include "vfs/vfs.h"
#include "vfs/VFile.h"
#include "include/config.h"

using std::string;

extern Logger * logger;

MultiPlayerMaps::MultiPlayerMaps()
{
	this->readMapData();
}

MultiPlayerMaps::~MultiPlayerMaps()
{
}

bool MultiPlayerMaps::getMapDescription (unsigned int Index, string & description)
{
	if ( Index < MapDescriptions.size() ){
		description = MapDescriptions[Index];
		return true;
	}
	return false;
}

bool MultiPlayerMaps::getMapName(unsigned int Index, string & Name)
{
	if ( Index < MapNames.size() ){
		Name = MapNames[Index];
		return true;
	}
	return false;
}

/**
 * Read information of Multi-Player maps from file "missions.pkt" in mix archives
 */
void MultiPlayerMaps::readMapData()
{
	VFile*	MapFile;
	char	Line[255];
	string	tmpString;
	Uint32	pos;
	Uint32	pos1;

	// Test the ABANDON1.mpr file
	MapFile = VFSUtils::VFS_Open("ABANDON1.INI");
	// Return with error
	if (MapFile == 0) 
	{
		// Logg it
		logger->error("Unable to locate ABANDON1.INI file!\n");
	}
	else
	{	// Logg it
		logger->note("Load ABANDON1.INI file\n");
		MapNames.push_back(string("ABANDON1"));
		MapDescriptions.push_back(string("Abandoned Battlefield (Med)"));										
	}



 	// Open the MP map description file "missions.pkt"
	MapFile = VFSUtils::VFS_Open("missions.pkt");
	// Return with error
	if (MapFile == 0) {
		// Logg it
		logger->error("Unable to locate mission.pkt file!\n");
		return;
	}

	// While there are lines Read one and save
	while (MapFile->getLine(Line, sizeof (Line)))
	{
		// Copy the line
		tmpString = Line; 

		int i = 0;
		while (tmpString[i] != '\0'){
			if (tmpString[i] == '[' || tmpString[i] == ']'){
				tmpString.erase(i,i+1);
			}
			i++;
		}

		// Save MP map Name
		if ((pos = tmpString.find (".INI",0)) != (Uint32)string::npos){
			//tmpString.erase (pos, pos+3);
			if (pos < tmpString.size()){
				MapNames.push_back (tmpString.substr (0, pos));
				//logger->debug("MP mpa found: %s\n", tmpString.substr (0, pos).c_str());
			}
		}

		// Save MP map Description
		if ((pos = tmpString.find ("=",0)) != (Uint32)string::npos){
			if ((pos1 = tmpString.find (")",0)) != (Uint32)string::npos){
				//tmpString.erase (pos, pos+3);
				if (pos < tmpString.size()){
					MapDescriptions.push_back (tmpString.substr (pos+1, tmpString.size()-(pos+1)-2));
					//logger->debug("MP map name: %s\n", tmpString.substr (pos+1, tmpString.size()-(pos+1)-2).c_str());
				}
			}
		}
	}
}
