// FibHeapEntry.cpp
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

#include "FibHeapEntry.h"

#include "SDL/SDL_types.h"

#include "TileRef.h"

FibHeapEntry::FibHeapEntry(TileRef* val, Uint32 k) 
{
	lnkTileRef = val;
	key = k;
}

TileRef* FibHeapEntry::getValue() 
{
	return lnkTileRef;
}

void FibHeapEntry::setKey(Uint32 k) 
{
	key = k;
}

Uint32& FibHeapEntry::getKey() 
{
	return key;
}
