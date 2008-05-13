#ifndef CURSORPOOL_H
#define CURSORPOOL_H

#include <map>
#include <vector>

#include "SDL/SDL_types.h"

using std::map;
using std::vector;

struct CursorInfo;
class INIFile;

class CursorPool {
private:
	std::vector<CursorInfo*> cursorpool;
	std::map<std::string, Uint16> name2index;
	INIFile * cursorini;
public:
	CursorPool(const char* ininame);
	~CursorPool();
	CursorInfo* getCursorByName(const char* name);
};

#endif //CURSORPOOL_H
