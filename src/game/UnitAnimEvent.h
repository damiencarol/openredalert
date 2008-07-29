// UnitAnimEvent.h
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

#ifndef UNITANIMEVENT_H
#define UNITANIMEVENT_H

#include "SDL/SDL_types.h"

#include "ActionEvent.h"

class Unit;

/**
 * Base Class for all Unit anim
 */
class UnitAnimEvent : public ActionEvent {
public:
	UnitAnimEvent(Uint32 p, Unit* un);
	virtual ~UnitAnimEvent();
	void setSchedule(UnitAnimEvent* e);
	void stopScheduled();
	virtual void stop() = 0;
	virtual void update();
	virtual void run() = 0;
private:
	Unit* un;
	UnitAnimEvent* scheduled;
};

#endif //UNITANIMEVENT_H
