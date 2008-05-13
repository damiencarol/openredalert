#ifndef KEYNOTFOUND_H
#define KEYNOTFOUND_H

#include <string>
#include <stdexcept>

using std::string;
using std::runtime_error;

namespace INI
{

class KeyNotFound : public std::runtime_error
{
public:
	KeyNotFound(const string& msg);
};

}

#endif //KEYNOTFOUND_H
