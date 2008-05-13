#ifndef INIFILE_H
#define INIFILE_H

#include <map>
#include <string>
#include <vector>
#include <list>

#include "SDL/SDL_types.h"
#include "IniEntry.h"
#include "INIKey.h"

using std::string;
using std::map;
using std::vector;
using std::list;


/**
 * Parses inifiles.
 * 
 * @TODO port the whole inifile class to the use of the new Inidata..
 * 
 * @TODO zx64 has a parser written using a few bits from boost.  
 * It's much faster and uses templates to simplify a lot of the code.
 * The only problem is integrating the bits of boost it uses into
 * the tree. For the interested: http://freecnc.sf.net/parse.tar.bz2
 * 
 * @TODO It's probably worth pooling INIFile instances so we only need 
 * to parse them once.
 */
class INIFile
{
public:
    explicit INIFile(const char* filename);
    ~INIFile();
 
    
    /// @TODO Would be nice if there was a version that returned a non-copy.
    char* readString(const char* section, const char* value);
    char* readString(const char* section, const char* value, const char* deflt);

    int readInt(const char* section, const char* value, Uint32 deflt);
    int readInt(const char* section, const char* value);

    float readFloat(const char* section, const char* value);
    float readFloat(const char* section, const char* value, float deflt);

    INIKey readKeyValue(const char* section, Uint32 keynum);
    INIKey readIndexedKeyValue(const char* section, Uint32 keynum, const char* prefix=0);
    std::string readSection(Uint32 secnum);

    int readYesNo(const char* section, const char* value, const char* defaut);
    
    static int splitStr(std::list<std::string>& L, const std::string& seq,
    		const std::string& _1cdelim, bool keeptoken, bool _removews);
    static int splitStr(std::list<std::string>& l, const std::string& seq, char s1,
    		char s2, bool keeptok);
    
    /** Function to test if a section is in the inifile */
    bool isSection(string section);
    
private:
//    std::map<std::string, INISection> inidata;
    std::map<std::string, INISection> Inidata;  // new for testing (making a unsorted list)
};

#endif //INIFILE_H
