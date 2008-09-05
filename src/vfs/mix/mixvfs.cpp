// mixvfs.cpp
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

#include "mixvfs.h"

#include <cmath>
#include <string>

#include "blowfish.h"
#include "ws-key.h"
#include "include/Logger.h"
#include "include/fcnc_endian.h"
#include "vfs/vfs.h"
#include "vfs/VFile.h"

using std::map;
using std::vector;
using std::string;
using std::min;

using namespace MIXPriv;

extern Logger * logger;

MIXFiles::MIXFiles()
{
}

MIXFiles::~MIXFiles()
{
	// Unload all MIX archives
    unloadArchives();
}

/**
 * Load a specifique archive
 */
bool MIXFiles::loadArchive(const char *fname)
{
    VFile* file = 0;

    //
    //logger->debug("Loading archive [%s]\n", fname);

    // Try to load the archive file
    file = VFSUtils::VFS_Open(fname);
    if (file == 0) {
        return false;
    }
    mixfiles.push_back(file);
    readMIXHeader(file);
    return true;
}

/**
 * Unload all MIX archives
 */
void MIXFiles::unloadArchives() {
    Uint32 i;
    for (i = 0; i < mixfiles.size(); ++i) {
    	VFSUtils::VFS_Close(mixfiles[i]);
    }
    mixfiles.resize(0);
    mixheaders.clear();
}

/**
 * Search a file from a name ???
 */
Uint32 MIXFiles::getFile(const char *fname)
{
    VFile *myvfile;
    mixheaders_t::iterator epos;
    openfiles_t::iterator of;
    OpenFile newfile;
    Uint32 id;

    // Calcul ID of the string ???
    id = calcID(fname);

    epos = mixheaders.find(id);
    if (mixheaders.end() == epos) {
        return (Uint32)-1;
    }
    myvfile = mixfiles[epos->second.filenum];

    newfile.id = id;
    newfile.pos = 0;

    openfiles_t::const_iterator ofe = openfiles.end();
    do {
    	// @todo Rewrite this loop.
        of = openfiles.find(id++);
    } while (ofe != of);
    id--;

    openfiles[id] = newfile;

    return id;
}

/**
 * Remove a file in the archive
 */
void MIXFiles::releaseFile(Uint32 file)
{
	// Remove 'file' in the archive
    openfiles.erase(file);
}

/**
 * Function to calculate a idnumber from a filename
 *
 * @param the filename
 * @return the id.
 */
Uint32 MIXFiles::calcID(const char *fname)
{
    Uint32 calc;
    int i;
    char buffer[13];
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32 tmpswap;
#endif

    for (i=0; *fname!='\0' && i<12; i++){
        buffer[i]=toupper(*(fname++));
    }
    while(i<13){
        buffer[i++]=0;
    }

    calc=0;
    for(i=0;buffer[i]!=0;i+=4) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        tmpswap = SDL_Swap32(*(long *)(buffer+i));
        calc=ROL(calc)+tmpswap;
#else
        calc=ROL(calc)+(*(long *)(buffer+i));
#endif
    }
    return calc;
}

/**
 * Decodes RA/TS Style MIX headers. Assumes you have already checked if
 *  header is encrypted and that mix is seeked to the start of the WSKey
 *
 * @param mix pointer to vfile for the mixfile
 * @param header pointer to header object that will store the mix's header
 * @param tscheck if equal to check_ts, will check if mix is from Tiberian Sun.
 * @return pointer to MixRecord
 */
MixRecord* MIXFiles::decodeHeader(VFile* mix, MixHeader* header, tscheck_ tscheck)
{
    Uint8 WSKey[80];        // 80-byte Westwood key
    Uint8 BFKey[56];        // 56-byte blow fish key
    Uint8 Block[8];         // 8-byte block to store blowfish stuff in
    Cblowfish bf;
    Uint8 *e;
    MixRecord* mindex;
    //bool aligned = true;

    mix->readByte(WSKey, 80);
    get_blowfish_key((const Uint8 *)&WSKey, (Uint8 *)&BFKey);
    bf.set_key((const Uint8 *)&BFKey, 56);
    mix->readByte(Block, 8);

    bf.decipher(&Block, &Block, 8);

    // Extract the header from Block
    memcpy(&header->c_files, &Block[0], sizeof(Uint16));
    memcpy(&header->size, &Block[sizeof(Uint16)], sizeof(Uint32));
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    header->c_files = SDL_Swap16(header->c_files);
    header->size	= SDL_Swap32(header->size);
#endif

    // Decrypt all indexes
    const int m_size = sizeof(MixRecord) * header->c_files;
    const int m_f = m_size + 5 & ~7;
    mindex = new MixRecord[header->c_files];
    e = new Uint8[m_f];
    //fread(e, m_f, 1, mix);
    mix->readByte(e, m_f);
    memcpy(mindex, &Block[6], 2);
    bf.decipher(e, e, m_f);

    memcpy(reinterpret_cast<Uint8 *>(mindex) + 2, e, m_size - 2);
    delete[] e;

    for (int i = 0; i < header->c_files; i++) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        mindex[i].id = SDL_Swap32(mindex[i].id);
        mindex[i].offset = SDL_Swap32(mindex[i].offset);
        mindex[i].size = SDL_Swap32(mindex[i].size);
#endif
#if 0
        if (check_ts == tscheck) {
            if (mindex[i].offset & 0xf)
                aligned = false;
            if (mindex[i].id == TS_ID)
                game = game_ts;
        }
#endif
        /* 92 = 4 byte flag + 6 byte header + 80 byte key + 2 bytes (?) */
        mindex[i].offset += 92 + m_f; /* re-center offset to be absolute offset */
    }
    /*
     if (aligned) game = game_ts;
    */
    return mindex;
}


/**
 * read the mixheader
 */
void MIXFiles::readMIXHeader(VFile *mix)
{
    MIXEntry mentry;
    MixHeader header;
    MixRecord *m_index = NULL;
    game_t game;
    Uint32 i;
    Uint32 flags;

    // Read header
    mix->readWord(&header.c_files, 1);
    mix->readDWord(&header.size, 1);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
// Don't know if this is needed.
//    header.flags = SDL_Swap32(header.flags);
#endif

    flags = header.c_files | header.size << 16;

    game = which_game(flags);
    if (game == game_ra) {
        //fseek(mix, -2, SEEK_CUR);
        mix->seekCur(-2);
        if (flags & mix_encrypted) {
            m_index = decodeHeader(mix, &header, check_ts);
        } else { /* mix is not encrypted */
            bool aligned = true;
            mix->seekSet(4);
            mix->readWord(&header.c_files, 1);
            mix->readDWord(&header.size, 1);

            const int m_size = sizeof(MixRecord) * header.c_files;
            m_index = new MixRecord[header.c_files];
            mix->readByte((Uint8 *)m_index, m_size);
            for (i = 0; i < header.c_files; i++) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
                m_index[i].id = SDL_Swap32(m_index[i].id);
                m_index[i].size = SDL_Swap32(m_index[i].size);
                m_index[i].offset = SDL_Swap32(m_index[i].offset);
#endif

                if (m_index[i].offset & 0xf)
                    aligned = false;
                if (m_index[i].id == TS_ID)
                    game = game_ts;
                m_index[i].offset += 4 + sizeof(MixHeader) + m_size;
            }
            if (aligned)
                game = game_ts;
        }
    } else if (game_td == game) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        mix->seekSet(0);
        mix->readWord(&header.c_files, 1);
        mix->readDWord(&header.size, 1);
#endif

        const int m_size = sizeof(MixRecord) * header.c_files;
        m_index = new MixRecord[header.c_files];
        //fread(reinterpret_cast<Uint8 *>(m_index), m_size, 1, mix);
        mix->readByte((Uint8 *)m_index, m_size);
        for (i = 0; i < header.c_files; i++) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            m_index[i].id = SDL_Swap32(m_index[i].id);
            m_index[i].offset = SDL_Swap32(m_index[i].offset);
            m_index[i].size = SDL_Swap32(m_index[i].size);
#endif
            /* 6 = 6 byte header - no other header/flags or keys in TD mixes */
            m_index[i].offset += 6 + m_size;
        }
    }
    for (i = 0; i < header.c_files; ++i) {
        mentry.filenum = (Uint8)mixfiles.size()-1;
        mentry.offset = m_index[i].offset;
        mentry.size = m_index[i].size;
        mixheaders[m_index[i].id] = mentry;
    }
    delete[] m_index;
}

Uint32 MIXFiles::readByte(Uint32 file, Uint8 *databuf, Uint32 numBytes)
{
    Uint32 numRead;
    Uint32 id, pos;
    MIXEntry me;

    id = openfiles[file].id;
    pos = openfiles[file].pos;

    me = mixheaders[id];

    mixfiles[me.filenum]->seekSet(me.offset+pos);

    numRead = min(numBytes, (me.size-pos));
    numRead = mixfiles[me.filenum]->readByte(databuf, numRead);
    openfiles[file].pos += numRead;
    return numRead;
}

Uint32 MIXFiles::readWord(Uint32 file, Uint16 *databuf, Uint32 numWords)
{
    Uint32 numRead;
    Uint32 id, pos;
    MIXEntry me;

    id = openfiles[file].id;
    pos = openfiles[file].pos;

    me = mixheaders[id];

    mixfiles[me.filenum]->seekSet(me.offset+pos);

    numRead = min(numWords, ((me.size-pos)>>1));
    numRead = mixfiles[me.filenum]->readWord(databuf, numRead);
    openfiles[file].pos += numRead<<1;
    return numRead;
}

Uint32 MIXFiles::readThree(Uint32 file, Uint32 *databuf, Uint32 numThrees)
{
    Uint32 numRead;
    Uint32 id, pos;
    MIXEntry me;

    id = openfiles[file].id;
    pos = openfiles[file].pos;

    me = mixheaders[id];

    mixfiles[me.filenum]->seekSet(me.offset+pos);

    numRead = min(numThrees, ((me.size-pos)/3));
    numRead = mixfiles[me.filenum]->readThree(databuf, numRead);
    openfiles[file].pos += numRead*3;
    return numRead;
}

Uint32 MIXFiles::readDWord(Uint32 file, Uint32 *databuf, Uint32 numDWords)
{
    Uint32 numRead;
    Uint32 id, pos;
    MIXEntry me;

    id = openfiles[file].id;
    pos = openfiles[file].pos;

    me = mixheaders[id];

    mixfiles[me.filenum]->seekSet(me.offset+pos);

    numRead = min(numDWords, ((me.size-pos)>>2));
    numRead = mixfiles[me.filenum]->readDWord(databuf, numRead);
    openfiles[file].pos += numRead<<2;
    return numRead;
}

char *MIXFiles::readLine(Uint32 file, char *databuf, Uint32 buflen)
{
    Uint32 numRead;
    Uint32 id, pos;
    MIXEntry me;
    char *retval;

    id = openfiles[file].id;
    pos = openfiles[file].pos;

    me = mixheaders[id];

    mixfiles[me.filenum]->seekSet(me.offset+pos);

    numRead = min(buflen-1, me.size-pos);
    if( numRead == 0 ) {
        return NULL;
    }
    retval = mixfiles[me.filenum]->getLine(databuf, numRead+1);
    openfiles[file].pos += (Uint32)strlen(databuf);
    return retval;
}

void MIXFiles::seekSet(Uint32 file, Uint32 pos)
{
    openfiles[file].pos = pos;
    if( openfiles[file].pos > mixheaders[openfiles[file].id].size ) {
        openfiles[file].pos = mixheaders[openfiles[file].id].size;
    }
    mixfiles[mixheaders[openfiles[file].id].filenum]->seekSet(openfiles[file].pos+mixheaders[openfiles[file].id].offset);
}

void MIXFiles::seekCur(Uint32 file, Sint32 pos)
{
    openfiles[file].pos += pos;
    if( openfiles[file].pos > mixheaders[openfiles[file].id].size ) {
        openfiles[file].pos = mixheaders[openfiles[file].id].size;
    }
    mixfiles[mixheaders[openfiles[file].id].filenum]->seekSet(openfiles[file].pos+mixheaders[openfiles[file].id].offset);
}


Uint32 MIXFiles::getPos(Uint32 file) const
{
    // @todo Abstract this const version of operator[]
    map<Uint32, MIXPriv::OpenFile>::const_iterator i = openfiles.find(file);
    if (openfiles.end() != i) {
        return i->second.pos;
    } else {
        // @todo Throw an exception in a later iteration of code cleanup.
        return 0;
    }
}

Uint32 MIXFiles::getSize(Uint32 file) const
{
    // @todo Abstract this const version of operator[]
    openfiles_t::const_iterator i = openfiles.find(file);
    if (openfiles.end() != i) {
        mixheaders_t::const_iterator i2 = mixheaders.find(i->second.id);
        if (mixheaders.end() != i2) {
            return i2->second.size;
        }
    }
    // @todo Throw an exception in a later iterator of code cleanup.
    return 0;
}

const char* MIXFiles::getPath(Uint32 file) const
{
    return NULL;
}

const char* MIXFiles::getArchiveType() const
{
	return "mix archive";
}
