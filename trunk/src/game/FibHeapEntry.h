#ifndef FIBHEAPENTRY_H
#define FIBHEAPENTRY_H

#include "SDL/SDL_types.h"

class TileRef;

class FibHeapEntry {	
public:
    FibHeapEntry(TileRef * val, Uint32 k);
    TileRef * getValue();

    Uint32& getKey();

    void setKey(Uint32 k);
private:
    TileRef * lnkTileRef;
    Uint32 key;
};
#endif //FIBHEAPENTRY_H
