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
