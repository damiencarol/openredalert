#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

#include "SDL/SDL_types.h"

using std::string;

/** 
 * @TODO Replace this class with a std::pair<string, Uint32>
 */
class Message
{
public:
    Message(const string& msg, Uint32 deltime) ;
    const char *getMessage() const ;
    bool expired(Uint32 time) const ;
private:
    string message;
    Uint32 deltime;
};

#endif //MESSAGE_H
