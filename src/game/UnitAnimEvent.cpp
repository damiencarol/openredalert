// UnitAnimEvent.cpp
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

#include "UnitAnimEvent.h"

#include <cmath>

#include "SDL/SDL_types.h"
#include "UnitOrStructure.h"
#include "ActionEventQueue.h"
#include "game/Unit.h"
#include "Unit.h"

namespace p {
	extern ActionEventQueue * aequeue;
}

UnitAnimEvent::UnitAnimEvent(Uint32 p, Unit* un) : ActionEvent(p)
{
    //logger->debug("UAE cons: this:%p un:%p\n",this,un);
    this->un = un;
    un->referTo();
    scheduled = NULL;
}

UnitAnimEvent::~UnitAnimEvent()
{
    //logger->debug("UAE dest: this:%p un:%p sch:%p\n",this,un,scheduled);
    if (scheduled != NULL) {
        p::aequeue->scheduleEvent(scheduled);
    }
    un->unrefer();
}

void UnitAnimEvent::setSchedule(UnitAnimEvent* e)
{
    //logger->debug("Scheduling an event. (this: %p, e: %p)\n",this,e);
    if (scheduled != NULL) {
        scheduled->setSchedule(NULL);
        scheduled->stop();
    }
    scheduled = e;
}

void UnitAnimEvent::stopScheduled()
{
    if (scheduled != NULL) {
        scheduled->stop();
    }
}

void UnitAnimEvent::update()
{
}

