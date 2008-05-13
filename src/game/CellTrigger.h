#ifndef CELLTRIGGER_H
#define CELLTRIGGER_H

#include <string>

#include "SDL/SDL_types.h"

using std::string;

class CellTrigger {
public:
    string name;
    Uint16 cellpos;
    Uint16 x; Uint16 y;
};

#endif //CELLTRIGGER_H
