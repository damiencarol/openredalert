#ifndef MPMAPSCLASS_H
#define MPMAPSCLASS_H

#include <string>
#include <vector>

using std::string;
using std::vector;

/** 
 * Multiplayer map class
 */
class MPmapsClass {
public:
    MPmapsClass();
    ~MPmapsClass();
    
    bool getMapDescription(unsigned int Index, std::string & String);

    /**
     * Return a map name. 
     */
    bool getMapName(unsigned int Index, std::string & Name);

private:
    std::vector < std::string > MapNames;
    std::vector < std::string > MapDescriptions;

    /**
     * Read information of Multi-Player maps from file "missions.pkt" in mix archives. 
     */
    void readMapData(void);
};

#endif //MPMAPSCLASS_H

