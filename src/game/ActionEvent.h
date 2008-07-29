// ActionEvent.h
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

#ifndef ACTIONEVENT_H
#define ACTIONEVENT_H

#include "SDL/SDL_types.h"

class Comp;

/** 
 * An abstract class which all actionevents must extend.
 *  
 * The run must be implemented. 
 */
class ActionEvent 
{
public:
    friend class Comp;
    ActionEvent(Uint32 p);

    void addCurtick(Uint32 curtick);
    virtual void run();

    void setDelay(Uint32 p);

    Uint32 getPrio();

    virtual ~ActionEvent();
    virtual void stop();
    
private:
    Uint32 prio;
    Uint32 delay;
};

#endif //ACTIONEVENT_H
