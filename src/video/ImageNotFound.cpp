#include "ImageNotFound.h"

#include <string>
#include <stdexcept>

using std::string;
using std::runtime_error;

ImageNotFound::ImageNotFound(const string& msg) :
	std::runtime_error(msg)
{
}
