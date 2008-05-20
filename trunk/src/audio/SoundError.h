
#ifndef SOUNDERROR_H
#define SOUNDERROR_H

#include <string>
#include <stdexcept>

using std::string;
using std::runtime_error;

class SoundError : public runtime_error
{
public:    

    SoundError(const string& msg) ;
};

#endif //SOUNDERROR_H
