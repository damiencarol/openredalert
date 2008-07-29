// mixvfs.h
// 1.2

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

#ifndef MIXFILES_H
#define MIXFILES_H

#include "SDL/SDL_types.h"

#include <vector>
#include <map>

#include "vfs/archive.h"
//#include "vfs/VFile.h"

namespace MIXPriv {

template<class T>
inline T ROL(T n) {return ((n << 1) | ((n >> 31) & 1));}

// Magic constants
const unsigned int TS_ID = 0x763c81dd;
const unsigned long int mix_checksum = 0x00010000;
const unsigned long int mix_encrypted = 0x00020000;

/* data type for which game the specific
 * mix file is from. Different from the
 * game definitions in freecnc.h
 */
enum game_t {game_td, game_ra, game_ts};
// d = first 4 bytes of mix file
inline game_t which_game(unsigned int d) {
    return (d == 0 || d == mix_checksum || d == mix_encrypted || d == (mix_encrypted | mix_checksum)) ? game_ra : game_td;
}

/** Information about size and hash of a packed file in mix archive */
struct MixHeader 
{
	/** =hash ?? =calculated id ??? */
    Uint16 c_files;
    /** Sie of the file */
    Uint32 size;
};

struct MixRecord {
    Uint32 id;
    Uint32 offset;
    Uint32 size;
};

/** only 256 mixfiles can be loaded */
struct MIXEntry {
    Uint8 filenum;
    Uint32 offset;
    Uint32 size;
};

struct OpenFile {
    Uint32 id;
    Uint32 pos;
};

enum tscheck_ {check_ts, nocheck_ts};

typedef std::map<Uint32, MIXEntry> mixheaders_t;
typedef std::map<Uint32, OpenFile> openfiles_t;
}


//class MIXTester;
class VFile;

using std::vector;


class MIXFiles : public Archive {
public:
    //friend class MIXTester;
    MIXFiles();
    ~MIXFiles();
    
    const char *getArchiveType() const;
    bool loadArchive(const char *fname);
    void unloadArchives();
    Uint32 getFile(const char *fname);
    void releaseFile(Uint32 file);

    Uint32 readByte(Uint32 file, Uint8 *databuf, Uint32 numBytes);
    Uint32 readWord(Uint32 file, Uint16 *databuf, Uint32 numWords);
    Uint32 readThree(Uint32 file, Uint32 *databuf, Uint32 numThrees);
    Uint32 readDWord(Uint32 file, Uint32 *databuf, Uint32 numDWords);
    char *readLine(Uint32 file, char *databuf, Uint32 buflen);

    void seekSet(Uint32 file, Uint32 pos);
    void seekCur(Uint32 file, Sint32 pos);

    Uint32 getPos(Uint32 file) const;
    Uint32 getSize(Uint32 file) const;

    const char* getPath(Uint32 file) const;
    
private:
    Uint32 calcID(const char* fname );
    void readMIXHeader(VFile* mix);
    MIXPriv::MixRecord* decodeHeader(VFile* mix, MIXPriv::MixHeader* header,
            MIXPriv::tscheck_ tscheck);

    vector<VFile*> mixfiles;
    MIXPriv::mixheaders_t mixheaders;

    MIXPriv::openfiles_t openfiles;
};

#endif //MIXFILES_H
