
#include "ActionEventQueue.h"

#include "SDL/SDL_timer.h"

#include "include/Logger.h"
#include "ActionEvent.h"
#include "Comp.h"

/**
 * Constructor, starts the timer
 */
ActionEventQueue::ActionEventQueue()
{
	// Save the current time
    starttick = SDL_GetTicks();
}

/** 
 * Destructor, removes the timer and empties the ActionEventQueue 
 */
ActionEventQueue::~ActionEventQueue()
{
    ActionEvent *ev;
    // Free all action event
    while( !eventqueue.empty() ) {
        ev = eventqueue.top();
        eventqueue.pop();
		if (ev != NULL)
			delete ev;
		ev = NULL;
    }
}

/** 
 * Schedules  event for later ececution.
 * 
 * @param a class containing the action to run.
 */
void ActionEventQueue::scheduleEvent(ActionEvent *ev)
{
    ev->addCurtick(getCurtick());
    eventqueue.push(ev);
}

/** 
 * Run all events in the actionqueue.
 */
void ActionEventQueue::runEvents()
{
    Uint32 curtick = getCurtick();
    
    // run all events in the queue with a prio lower than curtick
    while( !eventqueue.empty() && eventqueue.top()->getPrio() <= curtick ) {
        eventqueue.top()->run();
        eventqueue.pop();
    }
}

Uint32 ActionEventQueue::getElapsedTime()
{
    return SDL_GetTicks() - starttick;
}

Uint32 ActionEventQueue::getCurtick()
{
    return (SDL_GetTicks() - starttick) >> 5;
}
