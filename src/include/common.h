#ifndef COMMON_H
#define COMMON_H

#include <algorithm>
#include <cstring>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using std::vector;

//#include "video/MessagePool.h"
//#include "mapsini.h"


/* This file contains data types and constants that are used by multiple
 * classes, in possibly different files
 */

class CnCMap;
class UnitAndStructurePool;

class WeaponsPool;
namespace Dispatcher {
    class Dispatcher;
}

// Forward dcls for client only things.
class SoundEngine;
class INIFile;
class MessagePool;
class SHPImage;
class Sidebar;
class Input;
class ImageCache;
class Renderer;
class Cursor;
class PauseMenu;
class PlayerPool;
class MissionMapsClass;

// Pointers to the instance of commonly used objects.  Try to maintain the
// client/server seperation, so when it comes to the real split things will go
// easier.

// Used by both client and server
namespace p {
//    extern ActionEventQueue* aequeue;
    extern CnCMap* ccmap;
    extern UnitAndStructurePool* uspool;
    extern PlayerPool* ppool;
    extern WeaponsPool* weappool;
    extern Dispatcher::Dispatcher* dispatcher;
}
// Note we pass by value because we would otherwise copy anyway
//shared_ptr<INIFile> GetConfig(std::string name);
INIFile* GetConfig(std::string name);
void CleanConfig (void);


#include "config.h"

// These ifdefs are for compile-time enforcement rather than building at this
// time.  They should only get defined in .cpp files before including common.h.
/* #ifdef FREECNC_CLIENT */
// Client only
namespace pc {
	extern Renderer* renderer;
	extern SoundEngine* sfxeng;
	extern MessagePool* msg;
	extern std::vector<SHPImage *>* imagepool;
	extern ImageCache* imgcache;
	extern Sidebar * sidebar;
	extern Cursor* cursor;
	extern Input* input;
	//extern ConfigType Config;
	//extern MissionMapsClass *MissionsMapdata;
	extern bool quit;
	//extern ai *Ai;
	extern PauseMenu *PauseMenu;
}
/* #endif FREECNC_CLIENT */

#ifdef _MSC_VER
#define strcasecmp(str1, str2) _stricmp((str1), (str2))
#define strncasecmp(str1, str2, count) _strnicmp((str1), (str2), (count))
#endif

using std::max;
using std::min;
using std::string;

const string VERSION = "V0.47 beta";

// Bounded by colours.  This will change later
const Uint8 MAXPLAYERS = 6;



const Uint16 FULLHEALTH = 256;

extern int mapscaleq;

/* From math.h when __USE_BSD || defined __USE_XOPEN is defined
 * Not sure what is needed for MSVC though, so I'm sticking these here
 * for now
 */
#ifdef M_PI
#undef M_PI
#endif
#define M_PI   3.14159265358979323846
#ifdef M_PI_2
#undef M_PI_2
#endif
#define M_PI_2 1.57079632679489661923



/// @TODO: This shouldn't be here


/// @TODO: This shouldn't be here
struct powerinfo_t {
    Uint16 power;
    Uint16 drain;
    bool powered;
};

/// Same as strdup but uses C++ style allocation
/// @TODO: Obviate this function
inline char* cppstrdup(const char* s) {
	if (s == NULL){
		char* r = new char[5];
		r[0] = '\0';
		r[1] = '\0';
		return r;
	}else{
		char* r = new char[strlen(s)+2];
		return strcpy(r,s);
	}
}

inline bool isRelativePath(const char *p) {
#ifdef _WIN32
    return ((strlen(p) == 0) || p[1] != ':') && p[0] != '\\' && p[0] != '/';
#else
    return p[0] != '/';
#endif
}

/// @TODO Stringify this funciton
std::vector<char*> splitList(char* line, char delim);

/// @TODO Stringify this funciton
char* stripNumbers(const char* src);

const std::string& determineBinaryLocation(const std::string& launchcmd);

const std::string& getBinaryLocation();


#endif /* COMMON_H */
