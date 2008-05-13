#ifndef MESSAGE_H
#define MESSAGE_H

#include <list>
#include <string>

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

//#include "common.h"
//#include "Font.h"
//#include "ui/RA_Label.h"

using std::string;

//class RA_Label;

/** 
 * @TODO Replace this class with a std::pair<string, Uint32>
 */
class Message
{
public:
    Message(const std::string& msg, Uint32 deltime) : message(msg), deltime(deltime) {}
    const char *getMessage() const { return message.c_str(); }
    bool expired(Uint32 time) const { return time > deltime; }
private:
    std::string message;
    Uint32 deltime;
};

#endif //MESSAGE_H
