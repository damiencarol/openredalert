// ActionEventQueue.cpp
// 1.0

//    This file is part of OpenRedAlert.
//
//    OpenRedAlert is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    OpenRedAlert is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with OpenRedAlert.  If not, see <http://www.gnu.org/licenses/>.

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
 * Schedules  event for later execution.
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