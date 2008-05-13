#ifndef IMAGENOTFOUND_H
#define IMAGENOTFOUND_H

#include <string>
#include <stdexcept>

using std::string;
using std::runtime_error;

class ImageNotFound : public runtime_error
{
public:

	ImageNotFound(const string& msg) ;
};

#endif
