#include "VideoError.h"

#include <string>
#include <stdexcept>

using std::string;
using std::runtime_error;

VideoError::VideoError(const string& msg) :
	std::runtime_error(msg)
{
}
