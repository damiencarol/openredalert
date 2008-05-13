#include "WSAError.h"

#include <string>
#include <stdexcept>

using std::string;
using std::runtime_error;

WSAError::WSAError(const string& msg) :
	std::runtime_error(msg)
{
}
