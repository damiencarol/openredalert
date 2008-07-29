// VFile.h
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

#ifndef VFILE_H
#define VFILE_H

#include <cstdarg> // for use fct like 'printf'

#include "SDL/SDL_types.h"

class Archive;

/**
 * Virtual file class.
 * 
 * All virtual files can be opened for reading, but only external files
 *  can be written to since it's mostly used for loading graphics,
 *  sound, etc. data.
 */
class VFile {
public:
    VFile(Uint32 filenum, Archive * arch);
    ~VFile();


    Uint32 readByte(Uint8 * databuf, Uint32 numBytes);
    Uint32 readWord(Uint16 * databuf, Uint32 numWords);
    Uint32 readThree(Uint32 * databuf, Uint32 numThrees);
    Uint32 readDWord(Uint32 * databuf, Uint32 numDWords);

    /** Read a line of file */
    char * getLine(char * string, Uint32 buflen);

    Uint32 writeByte(Uint8 * databuf, Uint32 numBytes);
    Uint32 writeWord(Uint16 * databuf, Uint32 numWords);
    Uint32 writeThree(Uint32 * databuf, Uint32 numThrees);
    Uint32 writeDWord(Uint32 * databuf, Uint32 numDWords);
    void writeLine(const char * string);
    int vfs_printf(const char * fmt,...);
    int vfs_vprintf(const char * fmt, va_list ap);
    void flush();

    void seekSet(Uint32 pos);
    void seekCur(Sint32 offset);
    Uint32 fileSize();
    Uint32 tell();

    const char * getPath();
private:
    Uint32 filenum;
    Archive * archive;
};

#endif //VFILE_H
