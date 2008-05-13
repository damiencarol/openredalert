#ifndef ACTIONEVENTQUEUE_H
#define ACTIONEVENTQUEUE_H

#include <queue>
#include <vector>

#include "SDL/SDL_types.h"

#include "game/Comp.h"

class ActionEvent;
class Comp;


class ActionEventQueue
{
public:
    ActionEventQueue();
    ~ActionEventQueue();
    void scheduleEvent(ActionEvent * ev);
    void runEvents();
    Uint32 getElapsedTime();
    Uint32 getCurtick();
private:
    Uint32 starttick;
    std::priority_queue<ActionEvent*, std::vector<ActionEvent*>, Comp> eventqueue;
};

#endif //ACTIONEVENTQUEUE_H
