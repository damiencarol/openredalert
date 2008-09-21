// TriggerAction.h
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

#ifndef TRIGGERACTION_H
#define TRIGGERACTION_H

#include <string>

#include "SDL/SDL_types.h"

/**
 * Action that execute triggers
 *
 * TRIGGER_ACTION_NO_ACTION                0
 * TRIGGER_ACTION_WINNER_IS                1
 * TRIGGER_ACTION_LOSER_IS                 2
 * TRIGGER_ACTION_PRODUCTION_BEGINS        3
 * TRIGGER_ACTION_CREATE_TEAM              4
 * TRIGGER_ACTION_DESTROY_TEAM             5
 * TRIGGER_ACTION_ALL_TO_HUNT              6
 * TRIGGER_ACTION_REINFORCEMENTS           7
 * TRIGGER_ACTION_DROP_ZONE_FLARE          8   // WAYPOINT
 * TRIGGER_ACTION_FIRE_SALE                9
 * TRIGGER_ACTION_PLAY_MOVIE               10
 * TRIGGER_ACTION_TEXT                     11  // ID number
 * TRIGGER_ACTION_DESTROY_TRIGGER          12
 * TRIGGER_ACTION_AUTOCREATE_BEGINS        13
 // There is no trigger action number 14.
 * TRIGGER_ACTION_ALLOW_WIN                15
 * TRIGGER_ACTION_REVEAL_MAP               16
 * TRIGGER_ACTION_REVEAL_AROUND_WAYPOINT   17
 * TRIGGER_ACTION_REVEAL_ZONE_OF_WAYPOINT  18
 * TRIGGER_ACTION_PLAY_SOUND               19
 * TRIGGER_ACTION_PLAY_MUSIC               20
 * TRIGGER_ACTION_PLAY_SPEECH              21
 * TRIGGER_ACTION_FORCE_TRIGGER            22
 * TRIGGER_ACTION_TIMER_START              23
 * TRIGGER_ACTION_TIMER_STOP               24
 * TRIGGER_ACTION_TIMER_EXTEND             25
 * TRIGGER_ACTION_TIMER_SHORTEN            26
 * TRIGGER_ACTION_TIMER_SET                27
 * TRIGGER_ACTION_GLOBAL_SET               28
 * TRIGGER_ACTION_GLOBAL_CLEAR             29
 * TRIGGER_ACTION_AUTO_BASE_BUILDING       30
 * TRIGGER_ACTION_GROW_SHROUD_ONE_STEP     31
 * TRIGGER_ACTION_DESTROY_BUILDING         32
 * TRIGGER_ACTION_ADD_1TIME_SPEC_WEAPON    33
 * TRIGGER_ACTION_ADD_SPEC_WEAPON          34
 * TRIGGER_ACTION_PREFERRED_TARGET         35
 * TRIGGER_ACTION_LAUNCH_NUKES             36
 *
 * @author Damien Carol (OpenRedAlert)
 * @version 1.0
 * @since r360
 */
class TriggerAction {
public:
	/** Execute the action */
	virtual void execute() = 0;

	/** Return the number of the type of the action */
	int getAction();

	/** Destructor */
	virtual ~TriggerAction();

protected:
	/** Create a TriggerAction with a specified type */
	TriggerAction(int action);

	/** Type of ActionTrigger enumeration */
	enum TRIGGER_TYPE {
		NO_ACTION = 0,
		WINNER_IS = 1,
		LOSER_IS = 2,
		PRODUCTION_BEGINS = 3,
		CREATE_TEAM = 4,
		DESTROY_TEAM = 5,
		ALL_TO_HUNT = 6,
		REINFORCEMENTS = 7,
		/** parameter 3 = WAYPOINT */
		DROP_ZONE_FLARE = 8,
		FIRE_SALE = 9,
		PLAY_MOVIE = 10,
		/** parameter 3 = ID of the text in the file "tutorial.ini" */
		TEXT = 11,
		DESTROY_TRIGGER = 12,
		AUTOCREATE_BEGINS = 13,
		/** There is no trigger action number 14,. */
		XXXX = 14,
		ALLOW_WIN = 15,
		REVEAL_MAP = 16,
		REVEAL_AROUND_WAYPOINT = 17,
		REVEAL_ZONE_OF_WAYPOINT = 18,
		PLAY_SOUND = 19,
		PLAY_MUSIC = 20,
		PLAY_SPEECH = 21,
		FORCE_TRIGGER = 22,
		TIMER_START = 23,
		TIMER_STOP = 24,
		TIMER_EXTEND = 25,
		TIMER_SHORTEN = 26,
		TIMER_SET = 27,
		GLOBAL_SET = 28,
		GLOBAL_CLEAR = 29,
		AUTO_BASE_BUILDING = 30,
		GROW_SHROUD_ONE_STEP = 31,
		DESTROY_BUILDING = 32,
		ADD_1TIME_SPEC_WEAPON = 33,
		ADD_SPEC_WEAPON = 34,
		PREFERRED_TARGET = 35,
		LAUNCH_NUKES = 36
	};

private:
	/** number(=type) of the action */
	int action;
};

#endif //TRIGGERACTION_H
