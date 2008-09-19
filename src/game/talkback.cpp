// Talkback.cpp
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

#include "Talkback.h"

#include <cmath>
#include <map>
#include <string>
#include <vector>

#include "SDL/SDL_types.h"

#include "misc/INIFile.h"
#include "include/Logger.h"
#include "audio/SoundEngine.h"
#include "UnitAndStructurePool.h"
#include "TalkbackType.h"
#include "include/common.h"

using std::map;
using std::string;
using std::vector;

namespace pc {
/** SoundEngine of the game */	
	extern SoundEngine* sfxeng;
    extern ConfigType Config;
}
extern Logger * logger;

map<string, TalkbackType> Talkback::talktype;
bool Talkback::talktype_init;

Talkback::Talkback()
{
    if (!talktype_init) {
        talktype["report"] = TB_report;
        talktype["ack"] = TB_ack;
        talktype["die"] = TB_die;
        talktype["postkill"] = TB_postkill;
        talktype["attackunit"] = TB_atkun;
        talktype["attackstruct"] = TB_atkst;
        talktype_init = true;
    }
}

/**
 */
void Talkback::load(string talkback, INIFile *tbini)
{
    Uint32 keynum;
    INIKey key;

    //logger->debug("Loading %s\n", talkback.c_str());
    //logger->indent();
    try {
        tbini->readKeyValue(talkback.c_str(), 0);
    } catch(...) {
        logger->warning("Could not find talkback \"%s\", reverting to default\n",talkback.c_str());
        talkback = "Generic";
    }

    try {
        for (keynum=0;;++keynum)
        {
            // Still @todo: stringify rest of code
            char* first;
            key=tbini->readKeyValue(talkback.c_str(), keynum);
            first = stripNumbers(key->first.c_str());
            if (string(first) == "include")
            {
                if (key->second != talkback)
                {
                    merge(p::uspool->getTalkback(key->second.c_str()));
                } 
                else
                {
                    logger->warning("skipping self-referential include in %s\n",talkback.c_str());
                }
            } 
            else
            {
                if (string(first) == "delete")
                {
                    TalkbackType tbt = getTypeNum(key->second);
                    if (tbt == TB_invalid) {
                        continue;
                    }
                    talkstore[tbt].clear();
                    continue;
                }
                TalkbackType tbt = getTypeNum(first);
                if (tbt == TB_invalid) {
                    continue;
                }
                pc::sfxeng->LoadSound(key->second.c_str());
                //logger->debug("Pushing back %s\n", key->second.c_str());
                talkstore[tbt].push_back(key->second);
            }
            delete[] first;
        }
    } catch(...) {}
    //logger->unindent();
    //logger->debug("report is %i big\n", talkstore[TB_report].size());
}

/**
 */
const char* Talkback::getRandTalk(TalkbackType type)
{
    vector<string>& talk = getTypeVector(type);
    if (talk.empty()) {
//		logger->warning ("%s line %i: Talkback store empty\n", __FILE__, __LINE__);
        return "";
    }
    double sze = static_cast<double>(talk.size());
    int rnd = static_cast<int>(floor((sze*rand()/(RAND_MAX+1.0))));
    return talk[rnd].c_str();
}

/**
 */
vector<string>& Talkback::getTypeVector(TalkbackType type)
{
	if (talkstore[type].empty()){
		if ( type == TB_atkun || type == TB_atkst ) {
			return talkstore[TB_ack];
		}
		//logger->warning ("%s line %i: Talkback store empty\n", __FILE__, __LINE__);
		return talkstore[type];
	}

	t_talkstore::iterator ret = talkstore.find(type);
	if (ret == talkstore.end()) {
		// @todo : change it assert(0 && "Unknown talkback type");
	}
	return ret->second;
}

/**
 */
void Talkback::merge(Talkback *mergee)
{
    typedef map<string, TalkbackType>::iterator TBI;
    TBI t = talktype.begin();
    TBI end = talktype.end();
    while (t != end) {
        vector<string>& src = mergee->talkstore[t->second];
        vector<string>& dest = talkstore[t->second];
        //logger->debug("Copying %s/%i %i new into %i\n", t->first.c_str(), t->second, src.size(), dest.size());
        copy(src.begin(), src.end(), back_inserter(dest));
        ++t;
    }
}

TalkbackType Talkback::getTypeNum(string name)
{
	// lower the string
    transform(name.begin(), name.end(), name.begin(), tolower);
    typedef map<string, TalkbackType>::const_iterator TBCI;
    
    TBCI tbtype = talktype.find(name);
    if (tbtype == talktype.end()) {
        logger->error("%s line %i: Unknown type: %s\n", __FILE__, __LINE__, name.c_str());
        return TB_invalid;
    }
    return tbtype->second;
}
