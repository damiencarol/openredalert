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

