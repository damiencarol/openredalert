#include "Message.h"

#include <string>

#include "SDL/SDL_types.h"

using std::string;

Message::Message(const string & msg, Uint32 deltime)
{
    message = msg;
    deltime = deltime;
}

const char * Message::getMessage() const
{
    return message.c_str();
}

bool Message::expired(Uint32 time) const
{
    return time > deltime;
}

