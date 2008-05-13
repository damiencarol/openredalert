#include "GameError.h"

#include <string>
#include <stdexcept>

using std::string;
using std::runtime_error;

GameError::GameError(const string& msg) :
	std::runtime_error(msg)
{
}
