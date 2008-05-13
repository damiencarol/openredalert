#include "KeyNotFound.h"

#include <string>
#include <stdexcept>

using std::string;
using std::runtime_error;

INI::KeyNotFound::KeyNotFound(const string& msg) :
	std::runtime_error(msg)
{
}
