#ifndef ACTIONEVENT_H
#define ACTIONEVENT_H

#include "SDL/SDL_types.h"

class Comp;

/** 
 * An abstract class which all actionevents must extend.
 *  
 * The run must be implemented. 
 */
class ActionEvent {
public:
    friend class Comp;
    ActionEvent(Uint32 p);


    void addCurtick(Uint32 curtick);
    virtual void run();

    void setDelay(Uint32 p) ;

    Uint32 getPrio() ;

    virtual ~ActionEvent();
    virtual void stop();
private:
    Uint32 prio;
    Uint32 delay;
};

#endif //ACTIONEVENT_H
