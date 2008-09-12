// common.cpp
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

#include <iostream>
#include <string>
#include <cstdio>
#include <cctype>

#include "game/Ai.h"
#include "include/common.h"
#include "misc/INIFile.h"
#include "video/Renderer.h"
#include "game/ActionEventQueue.h"
#include "ui/PauseMenu.h"
#include "video/GraphicsEngine.h"
#include "game/RedAlertDataLoader.h"
#include "include/Logger.h"
#include "game/Dispatcher.h"

using std::cout; 
using std::map; 
using std::string;

typedef struct TiniFile
{
	string filename;
	INIFile *inifile;
} TiniFile;


namespace {
	string binloc;
}

/** In game objects */
namespace p {
	/** Action event queue in game */
	ActionEventQueue	*aequeue = 0;
	/** Current map in game */
	CnCMap			*ccmap = 0;
	/** Unit and structure pool in gmae */
	UnitAndStructurePool	*uspool = 0;
	/** Pool of players */
	PlayerPool*		ppool = 0;
	/** Pool of weapon data */
	WeaponsPool*	weappool = 0;
	/** Dispatcher of the game */
	Dispatcher* 	dispatcher = 0;
	/** */
	vector<TiniFile>	Setting;
	/** Custom loader for loading weapons data in originals archives files */
	RedAlertDataLoader * raLoader;
}

/** 
 * Check if a inifile was already loaded in the p::Setting list
 * 
 * if not this function loads it in the list
 * We pass by value because we could copy anyway
 */
INIFile* GetConfig(string name) 
{
	TiniFile TempIniFile;

	// transform all lettres from capitals to lower caracteres
	// ex : "XYZ" -> "xyz"
	transform(name.begin(), name.end(), name.begin(), tolower);

	for (Uint32 i = 0; i < p::Setting.size(); i++){
		if (p::Setting[i].filename == name){
			return p::Setting[i].inifile;
		}
	}

	// Not found, read new inifile
	TempIniFile.inifile = new INIFile(name.c_str());
	TempIniFile.filename = name;
	p::Setting.push_back (TempIniFile);
	return TempIniFile.inifile;
}

void CleanConfig()
{
	
	for (Uint32 i = 0; i < p::Setting.size(); i++){
		delete p::Setting[i].inifile;
	}
	p::Setting.clear();
}


/** 
 * Client only
 */
namespace pc {
	Renderer		*renderer;
	/** SoundEngine of the game */
	SoundEngine		*sfxeng = 0;
	GraphicsEngine	*gfxeng = 0;
	MessagePool		*msg = 0;
	vector<SHPImage *>	*imagepool = 0;
	ImageCache		*imgcache = 0;
	Sidebar			*sidebar = 0;
	Cursor			*cursor = 0;
	Input			*input = 0;
	//MissionMapsClass	*MissionsMapdata = 0;
	bool 			quit = false;
	ConfigType		Config;
	Ai				*ai;	
}

// Server only
namespace ps {
}

extern Logger* logger;

// Server only
std::vector<char*> splitList(char* line, char delim)
{
    vector<char*> retval;
    char* tmp;
    Uint32 i,i2;
    tmp = NULL;
    if (line != NULL) {
        tmp = new char[16];
        memset(tmp,0,16);
        for (i=0,i2=0;line[i]!=0x0;++i) {
            if ( (i2>=16) || (tmp != NULL && (line[i] == delim)) ) {
                retval.push_back(tmp);
                tmp = new char[16];
                memset(tmp,0,16);
                i2 = 0;
            } else {
                tmp[i2] = line[i];
                ++i2;
            }
        }
        retval.push_back(tmp);
    }
    return retval;
}

/**
 * Change "foo123" to "foo"
 */
char* stripNumbers(const char* src)
{
    char* dest;
    Uint16 i;
    for (i=0;i<strlen(src);++i) {
        if (src[i] <= '9') {
            break;
        }
    }
    dest = new char[i+1];
    strncpy(dest,src,i);
    dest[i] = 0;
    return dest;
}

char normalise_delim(char c) {
    if ('\\' == c) {
        return '/';
    }
    return c;
}

/**
 * @todo Something's not right, but this works better.
 */
const string& determineBinaryLocation(const string& launchcmd) {
    string path(launchcmd);

    transform(path.begin(), path.end(), path.begin(), normalise_delim);
    string::size_type delim = path.find_last_of('/');

    if (string::npos == delim) {
        return binloc = ".";
    }
    return binloc = path.substr(0, delim);
}

const string& getBinaryLocation() {
    return binloc;
}
