#ifndef MESSAGEPOOL_H
#define MESSAGEPOOL_H

#include <list>
#include <string>

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"


#include "ui/RA_Label.h"

class Message;

using std::list;
using std::string;


class MessagePool
{
public:
    MessagePool();
    void setWidth(Uint32 width) ;
    Uint32 getWidth() const ;
    ~MessagePool();
    SDL_Surface *getMessages();
    void postMessage(const std::string& msg);
    void clear();
    void refresh();
private:
    std::list<Message> msglist;
    bool updated;
    SDL_Surface* textimg;
    RA_Label msglabel;
    Uint32 width;
};

#endif
