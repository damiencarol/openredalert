// UAttackAnimEvent.h
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

#ifndef UATTACKANIMEVENT_H
#define UATTACKANIMEVENT_H

#include "SDL/SDL_types.h"

#include "UnitAnimEvent.h"

class Unit;
class UnitOrStructure;

/**
 * 
 */
class UAttackAnimEvent : public UnitAnimEvent
{
public:
	UAttackAnimEvent(Uint32 p, Unit *un);
	virtual ~UAttackAnimEvent();
	void stop();
	virtual void update();
	virtual void run();
private:
	Unit* un;
	bool stopping;
	Uint8 waiting;
	UnitOrStructure* target;
	bool UsePrimaryWeapon;
};

#endif //UATTACKANIMEVENT_H
