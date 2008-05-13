#include "LoadMapError.h"

#include <string>
#include <stdexcept>

using std::string;
using std::runtime_error;

LoadMapError::LoadMapError(const string& msg) :
	std::runtime_error(msg)
{
}
