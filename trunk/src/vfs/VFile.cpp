// VFile.cpp
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

#include "VFile.h"

#include "SDL/SDL_types.h"

#include "archive.h"

VFile::VFile(Uint32 filenum, Archive * arch)
{
	this->filenum = filenum;
	this->archive = arch;
}

VFile::~VFile()
{
	archive->releaseFile(filenum);
}

Uint32 VFile::readByte(Uint8 * databuf, Uint32 numBytes)
{
	return archive->readByte(filenum, databuf, numBytes);
}

Uint32 VFile::readWord(Uint16 * databuf, Uint32 numWords)
{
	return archive->readWord(filenum, databuf, numWords);
}

Uint32 VFile::readThree(Uint32 * databuf, Uint32 numThrees)
{
	return archive->readThree(filenum, databuf, numThrees);
}

Uint32 VFile::readDWord(Uint32 * databuf, Uint32 numDWords)
{
	return archive->readDWord(filenum, databuf, numDWords);
}

char * VFile::getLine(char * string, Uint32 buflen)
{
	return archive->readLine(filenum, string, buflen);
}

Uint32 VFile::writeByte(Uint8 * databuf, Uint32 numBytes)
{
	return archive->writeByte(filenum, databuf, numBytes);
}

Uint32 VFile::writeWord(Uint16 * databuf, Uint32 numWords)
{
	return archive->writeWord(filenum, databuf, numWords);
}

Uint32 VFile::writeThree(Uint32 * databuf, Uint32 numThrees)
{
	return archive->writeThree(filenum, databuf, numThrees);
}

Uint32 VFile::writeDWord(Uint32 * databuf, Uint32 numDWords)
{
	return archive->writeDWord(filenum, databuf, numDWords);
}

void VFile::writeLine(const char * string)
{
	archive->writeLine(filenum, string);
}

int VFile::vfs_printf(const char * fmt, ...)
{
	int ret;
	va_list ap;
	va_start(ap, fmt);
	ret = archive->vfs_printf(filenum, fmt, ap);
	va_end(ap);
	return ret;
}

int VFile::vfs_vprintf(const char * fmt, va_list ap)
{
	return archive->vfs_printf(filenum, fmt, ap);
}

void VFile::flush()
{
	archive->flush(filenum);
}

void VFile::seekSet(Uint32 pos)
{
	archive->seekSet(filenum, pos);
}

void VFile::seekCur(Sint32 offset)
{
	archive->seekCur(filenum, offset);
}

Uint32 VFile::fileSize()
{
	return archive->getSize(filenum);
}

Uint32 VFile::tell()
{
	return archive->getPos(filenum);
}

const char * VFile::getPath()
{
	return archive->getPath(filenum);
}
