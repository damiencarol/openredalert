// MultiPlayerMaps.h
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

#ifndef MULTIPLAYERMAPS_H
#define MULTIPLAYERMAPS_H

#include <string>
#include <vector>

using std::string;
using std::vector;

/** 
 * Multiplayer map class
 */
class MultiPlayerMaps
{
public:
	MultiPlayerMaps();
    ~MultiPlayerMaps();
    
    bool getMapDescription(unsigned int Index, string & String);

    /**
     * Return a map name. 
     */
    bool getMapName(unsigned int Index, string & Name);

private:
	/** Name of maps */
    vector<string> MapNames;
    /** Description of maps */
    vector<string> MapDescriptions;

    /**
     * Read information of Multi-Player maps from file "missions.pkt" in mix archives. 
     */
    void readMapData();
};

#endif //MULTIPLAYERMAPS_H

