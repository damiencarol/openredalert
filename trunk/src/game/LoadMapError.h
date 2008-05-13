#ifndef LOADMAPERROR_H
#define LOADMAPERROR_H

#include <string>
#include <stdexcept>

using std::string;
using std::runtime_error;

class LoadMapError : public runtime_error
{
public:
	LoadMapError(const string& msg) ;
};

#endif //LOADMAPERROR_H
