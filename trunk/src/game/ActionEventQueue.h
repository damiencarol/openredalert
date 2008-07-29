// ActionEventQueue.h
// 1.0

//    This file is part of OpenRedAlert.
//
//    OpenRedAlert is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, version 2 of the License.
//
//    OpenRedAlert is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with OpenRedAlert.  If not, see <http://www.gnu.org/licenses/>.

#ifndef ACTIONEVENTQUEUE_H
#define ACTIONEVENTQUEUE_H

#include <queue>
#include <vector>

#include "SDL/SDL_types.h"

#include "game/Comp.h"

class ActionEvent;

using std::vector;
using std::priority_queue;

/**
 * Manager of all ActionEvent in the game
 * 
 * This object call ActionEvent with priority
 * 
 * @todo add methode start to backup the time and start from 0 the time elapsed (possible trigger bug)
 */
class ActionEventQueue
{
public:
    ActionEventQueue();
    ~ActionEventQueue();
    
    /** Schedule an ActionEvent in the queue */
    void scheduleEvent(ActionEvent * ev);
    void runEvents();
    /** Get time elapsed beetween the creation of the queue and now */
    Uint32 getElapsedTime();
    Uint32 getCurtick();
    
private:
	/** First tick backup */
    Uint32 starttick;
    /** Queue of all ActionEvent which waiting to be called by this ActionEventQueue */
    priority_queue<ActionEvent*, vector<ActionEvent*>, Comp> eventqueue;
};

#endif //ACTIONEVENTQUEUE_H
