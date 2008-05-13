#ifndef PATH_H
#define PATH_H

#include <stack>

#include "SDL/SDL_types.h"

class Unit;

class Path : public std::stack<Uint8>
{
public:
    Path(Unit* unit, Uint32 crBeg, Uint32 crEnd, Uint8 max_dist);
    ~Path();
};

#endif //PATH_H
