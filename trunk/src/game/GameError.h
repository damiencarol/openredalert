#ifndef GAMEERROR_H
#define GAMEERROR_H

#include <stdexcept>

#include "include/config.h"

using std::runtime_error;


class GameError : public runtime_error 
{
public:
	GameError(const string& msg) ;
};
#endif
