// BuildingAnimEvent.h
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

#ifndef BUILDINGANIMEVENT_H
#define BUILDINGANIMEVENT_H

#include "ActionEvent.h"
#include "anim_nfo.h"
#include "animinfo_t.h"

class BAttackAnimEvent;
class RefineAnimEvent;
class Structure;
class StructureType;

/** 
 * This is the base class for all structure animations 
 */
class BuildingAnimEvent : public ActionEvent
{
public:
	/**
	 * @param p the priority of the event
	 * @param str the structure to which the animation is to be applied
	 * @param mode a numberic constant that is unique for each structure animation event.  Used for chaining events
	 */
	BuildingAnimEvent(Uint32 p, Structure * str, Uint8 mode);

	/** cleans up, checks for a scheduled event and runs it if there is */
	virtual ~BuildingAnimEvent();

	/** Passes control over to the anim_func (defined in derived classes) */
	virtual void run();

	/**
	 * Cleanly terminates the animation Structure animations should always be stopped using this function
	 * as it ensures that any scheduled animations are run. (Although you may have reasons not to, such as to avoid
	 * this side-effect)
	 */
	virtual void stop();

	/** checks whether the structure has been critically damaged */
	virtual void updateDamaged();

	/**
	 * \note Brief note to explain my thinking behind this: Originally, (until I knew better) the code used protected members
	 * to pass data between the generic building animation class and the
	 * actual animation classes.  This was changed in favour of passing
	 * a pointer to a struct from the run function to be modified by the derived class. \par
	 * Some classes needed their own updateDamaged function, which either
	 * needs to update the anim_data structure or read from it. \par
	 * Looking through those functions, at a later date from when I first
	 * wrote this code, I think I can resolve this issue nicely.
	 */

	//@{
	/// if a class overrides the updateDamaged method it should be a friend
	friend class RefineAnimEvent;
	friend class ProcAnimEvent;
	friend class DoorAnimEvent;
	friend class BAttackAnimEvent;
	//@}

	/**
	 * Sets the next animation to run when the current animation finishes
	 * Since the code for the AttackAnimEvent is different to the others
	 * there needs to be seperate code to handle scheduling Attack events
	 */
	void setSchedule(BAttackAnimEvent * ea, bool attack);
	void setSchedule(BuildingAnimEvent * e);

	virtual void update();

	anim_nfo anim_data;
private:
	//Structure * lnkStructure;
	Structure * strct;

	bool layer2;
	bool toAttack;
	BuildingAnimEvent * e;
	BAttackAnimEvent * ea;

protected:
	/** all derived classes must define this function */
	virtual void anim_func(anim_nfo * data) = 0;

	/** retrieve some constant data from the structure */
	animinfo_t getaniminfo();

	/** this is needed as strct->getType() returns UnitOrStructureType* */
	StructureType * getType();
};

#endif //BUILDINGANIMEVENT_H
