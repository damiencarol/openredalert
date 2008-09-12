// ExternalFiles.cpp
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

#include <algorithm>
#include <cstdio>
#include <cstdarg>
#include <cctype>
#include <cerrno>
#include <string>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
// #include <unistd.h>   // <-- Is this really needed ?
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

#include "SDL/SDL_endian.h"

#include "video/Renderer.h"
#include "externalvfs.h"
#include "include/common.h"

#if _MSC_VER && _MSC_VER < 1300
using namespace std;
#else
using std::string;
#endif

namespace ExtPriv {
struct OpenFile {
    FILE *file;
    size_t size;
    std::string path;
};
typedef std::map<size_t, OpenFile> openfiles_t;
openfiles_t openfiles;
FILE* fcaseopen(std::string* path, const char* mode, Uint32 caseoffset = 0) throw();
bool isdir(const string& path);
}

using namespace ExtPriv; // XXX:  This compiles, namespace ExtPriv {...} doesn't.

#ifdef __MORPHOS__
ExternalFiles::ExternalFiles(const char *defpath) : defpath("PROGDIR:") {
}
#else
ExternalFiles::ExternalFiles(const char *defpath) : defpath(defpath) {
}
#endif


/**
 */
ExternalFiles::~ExternalFiles() 
{
    // printf ("%s line %i: WARING external files destructor\n", __FILE__, __LINE__);
}

/**
 * Load an archive by this filename
 * 
 * @param fname file name of the archive
 * @return <code>true</code> if the loading complete successfully else <code>false</code>
 */
bool ExternalFiles::loadArchive(const char *fname)
{
    string pth(fname);
    if ("." == pth || "./" == pth) {
#ifdef __MORPHOS__
	path.push_back("PROGDIR:");
#else
        path.push_back("./");
#endif
        return true;
    }
    if (isRelativePath(fname)) {
#ifdef __MORPHOS__
	pth = defpath + fname;
#else
        pth = defpath + "/" + fname;
#endif
    } else {
        pth = fname;
    }
    if ('/' != pth[pth.length() - 1]) {
        pth += "/";
    }
    if (!isdir(pth)) {
        return false;
    }
    path.push_back(pth);
    return true;
}

Uint32 ExternalFiles::getFile(const char *fname, const char* mode)
{
    ExtPriv::OpenFile newFile;
    FILE *f;
    Uint32 i;
    string filename;
    size_t size, fnum;

    if (mode[0] != 'r') {
        filename = fname;
		//printf ("%s line %i: open file %s\n", __FILE__, __LINE__, fname);
        f = fopen(filename.c_str(), mode);
        if (f != NULL) {
            newFile.file = f;
            // We'll just ignore file sizes for files being written for now.
            newFile.size = 0;
            newFile.path = filename;
            fnum = (size_t)f;
            openfiles[fnum] = newFile;
            return fnum;
        } // Error condition hanled at end of function
    }
    for (i = 0; i < path.size(); ++i) {
        filename = path[i] + fname;
        f = fcaseopen(&filename, mode, path[i].length());
        if (f != NULL) {
            fseek(f, 0, SEEK_END);
            size = ftell(f);
            fseek(f, 0, SEEK_SET);
            newFile.file = f;
            newFile.size = size;
            newFile.path = filename;

            fnum = (size_t)f;
            openfiles[fnum] = newFile;
            return fnum;
        }
    }

    return (Uint32)-1;
}

/**
 */
void ExternalFiles::releaseFile(Uint32 file)
{
    fclose(openfiles[file].file);
    openfiles.erase(file);
}

Uint32 ExternalFiles::readByte(Uint32 file, Uint8 *databuf, Uint32 numBytes)
{
    return fread(databuf, 1, numBytes, openfiles[file].file);
}

Uint32 ExternalFiles::readWord(Uint32 file, Uint16 *databuf, Uint32 numWords)
{
    Uint32 numRead;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32 i;
#endif

    numRead = fread(databuf, 2, numWords, openfiles[file].file);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    for( i = 0; i < numRead; i++ ) {
        databuf[i] = SDL_Swap16(databuf[i]);
    }
#endif

    return numRead;
}

/**
 */
Uint32 ExternalFiles::readThree(Uint32 file, Uint32 *databuf, Uint32 numThrees)
{
    Uint32 numRead;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN

    Uint32 i;
#endif

    numRead = fread(databuf, 3, numThrees, openfiles[file].file);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN

    for( i = 0; i < numRead; i++ ) {
        databuf[i] = SDL_Swap32(databuf[i]);
        databuf[i]<<=8;
    }
#endif

    return numRead;
}

Uint32 ExternalFiles::readDWord(Uint32 file, Uint32 *databuf, Uint32 numDWords)
{
    Uint32 numRead;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN

    Uint32 i;
#endif

    numRead = fread(databuf, 4, numDWords, openfiles[file].file);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN

    for( i = 0; i < numRead; i++ ) {
        databuf[i] = SDL_Swap32(databuf[i]);
    }
#endif

    return numRead;
}

char *ExternalFiles::readLine(Uint32 file, char *databuf, Uint32 buflen)
{
    return fgets(databuf, buflen, openfiles[file].file);
}

Uint32 ExternalFiles::writeByte(Uint32 file, const Uint8* databuf, Uint32 numBytes)
{
    return fwrite(databuf, 1, numBytes, openfiles[file].file);
}

/**
 */
Uint32 ExternalFiles::writeWord(Uint32 file, const Uint16 *databuf, Uint32 numWords)
{
    Uint32 numWrote;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN

    Uint16* tmp = new Uint16[numWords];
    Uint32 i;

    for( i = 0; i < numWords; i++ ) {
        tmp[i] = SDL_Swap16(databuf[i]);
    }

    numWrote = fwrite(tmp, 2, numWords, openfiles[file].file);
	if (tmp != NULL){
		delete[] tmp;
    }
	tmp = NULL;
#else

    numWrote = fwrite(databuf, 2, numWords, openfiles[file].file);
#endif

    return numWrote;
}

/**
 */
Uint32 ExternalFiles::writeThree(Uint32 file, const Uint32 *databuf, Uint32 numThrees)
{
    Uint32 numWrote;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN

    Uint32* tmp = new Uint32[numThrees];
    Uint32 i;

    for( i = 0; i < numThrees; i++ ) {
        tmp[i] = SDL_Swap32(databuf[i]);
        tmp[i]<<=8;
    }
    numWrote = fwrite(tmp, 3, numThrees, openfiles[file].file);
	if (tmp != NULL){
		delete[] tmp;
    }
	tmp = NULL;
#else

    numWrote = fwrite(databuf, 3, numThrees, openfiles[file].file);
#endif

    return numWrote;
}

Uint32 ExternalFiles::writeDWord(Uint32 file, const Uint32 *databuf, Uint32 numDWords)
{
    Uint32 numWrote;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32 i;
    Uint32* tmp = new Uint32[numDWords];

    for( i = 0; i < numDWords; i++ ) {
        tmp[i] = SDL_Swap32(databuf[i]);
    }
    numWrote = fwrite(tmp, 4, numDWords, openfiles[file].file);
	if (tmp != NULL){
		delete[] tmp;
    }
	tmp = NULL;
#else

    numWrote = fwrite(databuf, 4, numDWords, openfiles[file].file);
#endif

    return numWrote;
}

void ExternalFiles::writeLine(Uint32 file, const char *databuf)
{
    fputs(databuf, openfiles[file].file);
}

int ExternalFiles::vfs_printf(Uint32 file, const char* fmt, va_list ap)
{
    int ret;
    ret = vfprintf(openfiles[file].file, fmt, ap);
    return ret;
}

void ExternalFiles::flush(Uint32 file) {
    fflush(openfiles[file].file);
}

void ExternalFiles::seekSet(Uint32 file, Uint32 pos)
{
    fseek(openfiles[file].file, pos, SEEK_SET);
}

void ExternalFiles::seekCur(Uint32 file, Sint32 pos)
{
    fseek(openfiles[file].file, pos, SEEK_CUR);
}

Uint32 ExternalFiles::getPos(Uint32 file) const
{
    // @todo Abstract this const implementation of operator[].
    openfiles_t::const_iterator i = openfiles.find(file);
    if (openfiles.end() != i) {
        return ftell(i->second.file);
    }
    // @todo Throw an exception in a later iteration of code cleanup.
    return 0;
}

Uint32 ExternalFiles::getSize(Uint32 file) const {
    // @todo Abstract this const implementation of operator[].
    openfiles_t::const_iterator i = openfiles.find(file);
    if (openfiles.end() != i) {
        return i->second.size;
    }
    // @todo Throw an exception in a later iteration of code cleanup.
    return 0;
}

const char* ExternalFiles::getPath(Uint32 file) const {
    // @todo Abstract this const implementation of operator[].
    openfiles_t::const_iterator i = openfiles.find(file);
    if (openfiles.end() != i) {
        return i->second.path.c_str();
    }
    // @todo Throw an exception in a later iteration of code cleanup.
    return 0;
}

const char *ExternalFiles::getArchiveType() const 
{
        return "external file";
}

Uint32 ExternalFiles::getFile(const char* fname) {return getFile(fname, "rb");}
namespace ExtPriv {

FILE* fcaseopen(string* name, const char* mode, Uint32 caseoffset) throw()
{
FILE* ret;

	if (name == NULL || mode == NULL)
		return NULL;

//	printf ("%s line %i: Open file %s, mode = %s\n", __FILE__, __LINE__, name->c_str(), mode);

    ret = fopen(name->c_str(), mode);
    if (NULL != ret) {
        return ret;
    }
#if defined (_WIN32) || defined (__MORPHOS__)
    return NULL;
#else
    string& fname = *name;
    // Try all other case.  Assuming uniform casing.
    Uint32 i;
    // Skip over non-alpha chars.
    // @todo These are the old style text munging routines that are a) consise
    // and b) doesn't work with UTF8 filenames.
    for (i=caseoffset;i<fname.length()&&!isalpha(fname[i]);++i);
    if (islower(fname[i])) {
        transform(fname.begin()+caseoffset, fname.end(), fname.begin()+caseoffset, toupper);
    } else {
        transform(fname.begin()+caseoffset, fname.end(), fname.begin()+caseoffset, tolower);
    }
    ret = fopen(fname.c_str(), mode);
    if (NULL != ret) {
        return ret;
    }
    // @todo Try other tricks like "lower.EXT" or "UPPER.ext"
    return NULL;
#endif
}

bool isdir(const string& path) {
#ifdef _WIN32
    DWORD length = GetCurrentDirectory(0, 0);
    char* orig_path = new char[length];
    GetCurrentDirectory(length, orig_path);
    if (!SetCurrentDirectory(path.c_str())) {
		if (orig_path != NULL)
			delete[] orig_path;
		orig_path = NULL;
        return false;
    }
    SetCurrentDirectory(orig_path);
	if (orig_path != NULL)
		delete[] orig_path;
	orig_path = NULL;
    return true;
#elif defined (__MORPHOS__)
        struct stat fileinfo;
	stat( path.c_str(), &fileinfo );
	return S_ISDIR( fileinfo.st_mode );
#else
    int curdirfd = open("./", O_RDONLY);
    if (-1 == curdirfd) {
        return false;
    }
    if (-1 == chdir(path.c_str())) {
        return false;
    }
    fchdir(curdirfd);
    close(curdirfd);
#endif
    return true;
}

} // namespace ExtPriv
