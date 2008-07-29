// ExternalFiles.h
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

#ifndef EXTERNALVFS_H
#define EXTERNALVFS_H

#include <string>
#include <vector>

#include "SDL/SDL_types.h"

#include "vfs/archive.h"

using std::string;
using std::vector;

/**
 * Object that provide methodes to load/read/write external files (not in original archives of the game
 */
class ExternalFiles : public Archive
{
public:
	ExternalFiles(const char *defpath);
	~ExternalFiles();
	
	const char *getArchiveType() const;
	bool loadArchive(const char *fname);
	/** Can't use default argument as we need exact type signature for inheritence. */
	Uint32 getFile(const char* fname);
	Uint32 getFile(const char *fname, const char* mode);
	void releaseFile(Uint32 file);

	Uint32 readByte(Uint32 file, Uint8 *databuf, Uint32 numBytes);
	Uint32 readWord(Uint32 file, Uint16 *databuf, Uint32 numWords);
	Uint32 readThree(Uint32 file, Uint32 *databuf, Uint32 numThrees);
	Uint32 readDWord(Uint32 file, Uint32 *databuf, Uint32 numDWords);
	char *readLine(Uint32 file, char *databuf, Uint32 buflen);

	Uint32 writeByte(Uint32 file, const Uint8 *databuf, Uint32 numBytes);
	Uint32 writeWord(Uint32 file, const Uint16 *databuf, Uint32 numWords);
	Uint32 writeThree(Uint32 file, const Uint32 *databuf, Uint32 numThrees);
	Uint32 writeDWord(Uint32 file, const Uint32 *databuf, Uint32 numDWords);
	void writeLine(Uint32 file, const char *databuf);
	int vfs_printf(Uint32 file, const char* fmt, va_list ap);
	void flush(Uint32 file);

	void seekSet(Uint32 file, Uint32 pos);
	void seekCur(Uint32 file, Sint32 pos);

	Uint32 getPos(Uint32 file) const;
	Uint32 getSize(Uint32 file) const;
	const char* getPath(Uint32 file) const;

private:
	string defpath;
	vector<string> path;
};

#endif //EXTERNALVFS_H
