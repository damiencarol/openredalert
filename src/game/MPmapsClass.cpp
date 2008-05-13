#include "MPmapsClass.h"

#include <string>

#include "include/Logger.h"
#include "vfs/vfs.h"
#include "vfs/VFile.h"
#include "include/config.h"

using std::string;

extern Logger * logger;

MPmapsClass::MPmapsClass()
{
	this->readMapData();
}

MPmapsClass::~MPmapsClass()
{
}

bool MPmapsClass::getMapDescription (unsigned int Index, string & description)
{
	if ( Index < MapDescriptions.size() ){
		description = MapDescriptions[Index];
		return true;
	}
	return false;
}

bool MPmapsClass::getMapName(unsigned int Index, string & Name)
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
void MPmapsClass::readMapData (void)
{
	VFile		*MapFile;
	char		Line[255];
	std::string	tmpString;
	Uint32		pos, pos1;

	// I am not sure how the maps from td work so...
 	if (getConfig().gamenum != GAME_RA){
		return;
 	}

 	// Open the MP map description file "missions.pkt"
	MapFile = VFSUtils::VFS_Open("missions.pkt");
	if(MapFile == NULL) {
		logger->error("Unable to locate mission.pkt file!\n");
		return;
	}

	// While there are lines Read one and save
	while (MapFile->getLine(Line, sizeof (Line)))
	{
		// Copy the line
		tmpString = Line; 
		
		// TODO DEBUG
		//logger->debug("[missions.pkt] : %s", tmpString.c_str());

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
