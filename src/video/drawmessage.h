#ifndef DRAWMESSAGE_H_
#define DRAWMESSAGE_H_
#include "Message.h"

struct drawMessage : std::unary_function<void, Message>
{
    drawMessage(RA_Label& label, SDL_Surface* textimg, Uint32& msgy) : label(label), textimg(textimg), msgy(msgy) {}
    void operator()(const Message& msg) {
        label.Draw(msg.getMessage(), textimg, 2, msgy);
        msgy += label.getHeight()+1;
    }
    RA_Label& label;
    SDL_Surface* textimg;
    Uint32& msgy;
};

#endif /*DRAWMESSAGE_H_*/
