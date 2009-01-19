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
#include "misc/config.h"
#include "misc/INIFile.h"

#ifdef __MSVC__
#include <windows.h>
#else
#if __GNUC__
#include <dirent.h>
#endif
#endif

using std::string;

extern Logger * logger;

/**
 */
MultiPlayerMaps::MultiPlayerMaps()
{
    this->readMapData();
}

/**
 */
MultiPlayerMaps::MultiPlayerMaps(const MultiPlayerMaps& orig)
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

    // Try to load file in /data/maps/
    loadMapsFolder();

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

		//int i = 0;		while (tmpString[i] != '\0')	//VS runtime checks wont like if you check on something that position.... it will trigger an assert
		for (unsigned int i = 0 ; i < tmpString.size(); i++)
		{
			if (tmpString[i] == '[' || tmpString[i] == ']')
			{
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

void MultiPlayerMaps::loadMapsFolder()
{

#ifdef __MSVC__

    string path = ".\\data\\maps\\";

    string searchPattern = "*.mpr";
    string fullSearchPath = path + searchPattern;

    WIN32_FIND_DATA FindData;
    HANDLE hFind;

    hFind = FindFirstFile(fullSearchPath.c_str(), &FindData);
    if( hFind == INVALID_HANDLE_VALUE )
    {
        //cout << "Error searching directory\n";
        //return -1;
    }

    do
    {
        string filePath = path + FindData.cFileName;
        // Read the map file
        INIFile* customMultiPlayerMap  = new INIFile(filePath.c_str());
        if (customMultiPlayerMap != 0)
        {
            string name = string(FindData.cFileName);
            string rawName = name.substr(0, name.length() - 4);
            logger->debug("Load %s map...\n", rawName.c_str());

            // Get the name of the multi-player map
            string theName = customMultiPlayerMap->readString("Basic", "Name");

            // Logg it
            logger->note("Multi-Player map file %s was loaded successfully\n", theName.c_str());

            MapNames.push_back(rawName);
            MapDescriptions.push_back(theName);
        }

        //ifstream in( filePath.c_str() );
        //if( in )
        //{
            // do stuff with the file here
        //}
        //else
        //{
        //    cout << "Problem opening file " << FindData.cFileName << "\n";
        //}
    }
    while( FindNextFile(hFind, &FindData) > 0 );

    //if( GetLastError() != ERROR_NO_MORE_FILES )
    //{
    //    cout << "Something went wrong during searching\n";
    //}
#else
#if __GNUC__
    DIR *dpdf;
    struct dirent *epdf;
    dpdf = opendir("./data/maps/");
    if (dpdf != 0)
    {
        while (epdf = readdir(dpdf))
        {

            // Check that extenstion is ".MPR" or ".mpr"
            string fileName = string(epdf->d_name);
            // Check that the file is "????"
            if (fileName.length() > 4)
            {
                string ext = fileName.substr(fileName.length()-4, 4);
                string rawName = fileName.substr(0, fileName.length() - 4);


                if (ext == ".MPR" || ext == ".mpr")
                {
                //printf("Filename: %s\n",rawName.c_str());
               // printf("ext: %s\n", ext.c_str());

                    // Read the map file
                    INIFile* customMultiPlayerMap = new INIFile(rawName + ext);
                    if (customMultiPlayerMap != 0)
                    {
                        // Logg it
                        logger->note("Multi-Player map file %s.MPR \"%s\" was loaded successfully\n", rawName.c_str(), string(customMultiPlayerMap->readString("Basic", "Name", "CUSTOM MAP" + MapDescriptions.size())).c_str());

                        MapNames.push_back(rawName);
                        MapDescriptions.push_back(customMultiPlayerMap->readString("Basic", "Name", "CUSTOM MAP" + MapDescriptions.size()));
                    }
                }
            }
        }
    }
#endif
#endif
}
