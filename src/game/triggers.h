// Triggers.h
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

#ifndef TRIGGERS_H_
#define TRIGGERS_H_

#include <string>

#include "SDL/SDL_types.h"

#include "game/RA_Tigger.h"

#define TRIGGER_NON_REPEATING                   0   // Fires when the events happen and destroys the trigger
#define TRIGGER_REPEAT_ONCE_ALL                 1   // Fires when all instances of the trigger (e.g. celltriggers, triggers on units, etc.) have their events happen, then destroys the trigger.
#define TRIGGER_FREE_REPEAT                     2   // Fires every time the events happen; Never destroys itself



//
// Available Trigger Events
//
#define TRIGGER_EVENT_NO_EVENT                  0
#define TRIGGER_EVENT_ENTERED_BY                1
#define TRIGGER_EVENT_SPIED_BY                  2
#define TRIGGER_EVENT_THIEVED_BY                3
#define TRIGGER_EVENT_DISCOVERED_BY             4
#define TRIGGER_EVENT_HOUSE_DISCOVERED          5
#define TRIGGER_EVENT_ATTACKED                  6
#define TRIGGER_EVENT_DESTROYED                 7
#define TRIGGER_EVENT_ANY_EVENT                 8
#define TRIGGER_EVENT_ALL_UNITS_DESTROYED       9
#define TRIGGER_EVENT_ALL_BUILDINGS_DESTROYED   10
#define TRIGGER_EVENT_ALL_DESTROYED             11
#define TRIGGER_EVENT_CREDITS_EXCEED            12
#define TRIGGER_EVENT_TIME_ELAPSED              13  // 1/10th min
#define TRIGGER_EVENT_MISSION_TIMER_EXPIRED     14
#define TRIGGER_EVENT_DESTROYED_NR_BUILDINGS    15
#define TRIGGER_EVENT_DESTROYED_NR_UNITS        16
#define TRIGGER_EVENT_NO_FACTORIES_LEFT         17
#define TRIGGER_EVENT_CIVILIANS_EVACUATED       18
#define TRIGGER_EVENT_BUILD_BUILDING_TYPE       19
#define TRIGGER_EVENT_BUILD_UNIT_TYPE           20
#define TRIGGER_EVENT_BUILD_INFANTRY_TYPE       21
#define TRIGGER_EVENT_BUILD_AIRCRAFT_TYPE       22
#define TRIGGER_EVENT_LEAVES_MAP                23
#define TRIGGER_EVENT_ZONE_ENTRY                24
#define TRIGGER_EVENT_CROSSED_HORIZONTAL_LINE   25
#define TRIGGER_EVENT_CROSSED_VERTICAL_LINE     26
#define TRIGGER_EVENT_GLOBAL_IS_SET             27
#define TRIGGER_EVENT_GLOBAL_IS_CLEAR           28
#define TRIGGER_EVENT_DESTROYED_FAKES_ALL       29
#define TRIGGER_EVENT_LOW_POWER                 30
#define TRIGGER_EVENT_ALL_BRIDGES_DESTROYED     31
#define TRIGGER_EVENT_BUILDING_EXISTS           32


#define TRIGGER_ACTION_NO_ACTION                0
#define TRIGGER_ACTION_WINNER_IS                1
#define TRIGGER_ACTION_LOSER_IS                 2
#define TRIGGER_ACTION_PRODUCTION_BEGINS        3
#define TRIGGER_ACTION_CREATE_TEAM              4
#define TRIGGER_ACTION_DESTROY_TEAM             5
#define TRIGGER_ACTION_ALL_TO_HUNT              6
#define TRIGGER_ACTION_REINFORCEMENTS           7
#define TRIGGER_ACTION_DROP_ZONE_FLARE          8   // WAYPOINT
#define TRIGGER_ACTION_FIRE_SALE                9
#define TRIGGER_ACTION_PLAY_MOVIE               10
#define TRIGGER_ACTION_TEXT                     11  // ID number
#define TRIGGER_ACTION_DESTROY_TRIGGER          12
#define TRIGGER_ACTION_AUTOCREATE_BEGINS        13
// There is no trigger action number 14.
#define TRIGGER_ACTION_ALLOW_WIN                15
#define TRIGGER_ACTION_REVEAL_MAP               16
#define TRIGGER_ACTION_REVEAL_AROUND_WAYPOINT   17
#define TRIGGER_ACTION_REVEAL_ZONE_OF_WAYPOINT  18
#define TRIGGER_ACTION_PLAY_SOUND               19
#define TRIGGER_ACTION_PLAY_MUSIC               20
#define TRIGGER_ACTION_PLAY_SPEECH              21
#define TRIGGER_ACTION_FORCE_TRIGGER            22
#define TRIGGER_ACTION_TIMER_START              23
#define TRIGGER_ACTION_TIMER_STOP               24
#define TRIGGER_ACTION_TIMER_EXTEND             25
#define TRIGGER_ACTION_TIMER_SHORTEN            26
#define TRIGGER_ACTION_TIMER_SET                27
#define TRIGGER_ACTION_GLOBAL_SET               28
#define TRIGGER_ACTION_GLOBAL_CLEAR             29
#define TRIGGER_ACTION_AUTO_BASE_BUILDING       30
#define TRIGGER_ACTION_GROW_SHROUD_ONE_STEP     31
#define TRIGGER_ACTION_DESTROY_BUILDING         32
#define TRIGGER_ACTION_ADD_1TIME_SPEC_WEAPON    33
#define TRIGGER_ACTION_ADD_SPEC_WEAPON          34
#define TRIGGER_ACTION_PREFERRED_TARGET         35
#define TRIGGER_ACTION_LAUNCH_NUKES             36


#define REDALERT_MOVIE_0    "AAGUN"
#define REDALERT_MOVIE_1    "MIG"
#define REDALERT_MOVIE_2    "SFROZEN"
#define REDALERT_MOVIE_3    "AIRFIELD"
#define REDALERT_MOVIE_4    "BATTLE"
#define REDALERT_MOVIE_5    "BMAP"
#define REDALERT_MOVIE_6    "BOMBRUN"
#define REDALERT_MOVIE_7    "DPTHCHRG"
#define REDALERT_MOVIE_8    "GRVESTNE"
#define REDALERT_MOVIE_9    "MONTPASS"
#define REDALERT_MOVIE_10   "MTNKFACT"
#define REDALERT_MOVIE_11   "CRONTEST"
#define REDALERT_MOVIE_12   "OILDRUM"
#define REDALERT_MOVIE_13   "ALLYEND"
#define REDALERT_MOVIE_14   "RADRRAID"
#define REDALERT_MOVIE_15   "SHIPYARD"
#define REDALERT_MOVIE_16   "SHORBOMB"
#define REDALERT_MOVIE_17   "SITDUCK"
#define REDALERT_MOVIE_18   "SLNTSRVC"
#define REDALERT_MOVIE_19   "SNOWBASE"
#define REDALERT_MOVIE_20   "EXECUTE"
#define REDALERT_MOVIE_21   "REDINTRO"
#define REDALERT_MOVIE_22   "NUKESTOK"
#define REDALERT_MOVIE_23   "V2ROCKET"
#define REDALERT_MOVIE_24   "SEARCH"
#define REDALERT_MOVIE_25   "BINOC"
#define REDALERT_MOVIE_26   "ELEVATOR"
#define REDALERT_MOVIE_27   "FROZEN"
#define REDALERT_MOVIE_28   "MCV"
#define REDALERT_MOVIE_29   "SHIPSINK"
#define REDALERT_MOVIE_30   "SOVMCV"
#define REDALERT_MOVIE_31   "TRINITY"
#define REDALERT_MOVIE_32   "ALLYMORF"
#define REDALERT_MOVIE_33   "APCESCPE"
#define REDALERT_MOVIE_34   "BRDGTILT"
#define REDALERT_MOVIE_35   "CRONFAIL"
#define REDALERT_MOVIE_36   "STRAFE"
#define REDALERT_MOVIE_37   "DESTROYR"
#define REDALERT_MOVIE_38   "DOUBLE"
#define REDALERT_MOVIE_39   "FLARE"
#define REDALERT_MOVIE_40   "SNSTRAFE"
#define REDALERT_MOVIE_41   "LANDING"
#define REDALERT_MOVIE_42   "ONTHPRWL"
#define REDALERT_MOVIE_43   "OVERRUN"
#define REDALERT_MOVIE_44   "SNOWBOMB"
#define REDALERT_MOVIE_45   "SOVCEMET"
#define REDALERT_MOVIE_46   "TAKE_OFF"
#define REDALERT_MOVIE_47   "TESLA"
#define REDALERT_MOVIE_48   "SOVIET8"
#define REDALERT_MOVIE_49   "SPOTTER"
#define REDALERT_MOVIE_50   "ALLY1"
#define REDALERT_MOVIE_51   "ALLY2"
#define REDALERT_MOVIE_52   "ALLY4"
#define REDALERT_MOVIE_53   "SOVFINAL"
#define REDALERT_MOVIE_54   "ASSESS"
#define REDALERT_MOVIE_55   "SOVIET10"
#define REDALERT_MOVIE_56   "DUD"
#define REDALERT_MOVIE_57   "MCV_LAND"
#define REDALERT_MOVIE_58   "MCVBRDGE"
#define REDALERT_MOVIE_59   "PERISCOP"
#define REDALERT_MOVIE_60   "SHORBOM1"
#define REDALERT_MOVIE_61   "SHORBOM2"
#define REDALERT_MOVIE_62   "SOVBATL"
#define REDALERT_MOVIE_63   "SOVTSTAR"
#define REDALERT_MOVIE_64   "AFTRMATH"
#define REDALERT_MOVIE_65   "SOVIET11"
#define REDALERT_MOVIE_66   "MASASSLT"
#define REDALERT_MOVIE_67   "ENGLISH"
#define REDALERT_MOVIE_68   "SOVIET1"
#define REDALERT_MOVIE_69   "SOVIET2"
#define REDALERT_MOVIE_70   "SOVIET3"
#define REDALERT_MOVIE_71   "SOVIET4"
#define REDALERT_MOVIE_72   "SOVIET5"
#define REDALERT_MOVIE_73   "SOVIET6"
#define REDALERT_MOVIE_74   "SOVIET7"
#define REDALERT_MOVIE_75   "PROLOG"
#define REDALERT_MOVIE_76   "AVERTED"
#define REDALERT_MOVIE_77   "COUNTDWN"
#define REDALERT_MOVIE_78   "MOVINGIN"
#define REDALERT_MOVIE_79   "ALLY10"
#define REDALERT_MOVIE_80   "ALLY12"
#define REDALERT_MOVIE_81   "ALLY5"
#define REDALERT_MOVIE_82   "ALLY6"
#define REDALERT_MOVIE_83   "ALLY8"
#define REDALERT_MOVIE_84   "TANYA1"
#define REDALERT_MOVIE_85   "TANYA2"
#define REDALERT_MOVIE_86   "ALLY10B"
#define REDALERT_MOVIE_87   "ALLY11"
#define REDALERT_MOVIE_88   "ALLY14"
#define REDALERT_MOVIE_89   "ALLY9"
#define REDALERT_MOVIE_90   "SPY"
#define REDALERT_MOVIE_91   "TOOFAR"
#define REDALERT_MOVIE_92   "SOVIET12"
#define REDALERT_MOVIE_93   "SOVIET13"
#define REDALERT_MOVIE_94   "SOVIET9"
#define REDALERT_MOVIE_95   "BEACHEAD"
#define REDALERT_MOVIE_96   "SOVIET14"
#define REDALERT_MOVIE_97   "SIZZLE"
#define REDALERT_MOVIE_98   "SIZZLE2"
//Counterstrike onlyu
#define REDALERT_MOVIE_99   "ANTEND"
#define REDALERT_MOVIE_100  "ANTINTRO"


//
// Available Red Alert Songs
//

//  0   BIGF226M    Bigfoot
//  1   CRUS226M    Crush
//  2   FAC1226M    Face the Enemy 1
//  3   FAC2226M    Face the Enemy 2
//  4   HELL226M    Hell March
//  5   RUN1226M    Run For Your Life
//  6   SMSH226M    Smash
//  7   TREN226M    Trenches
//  8   WORK226M    Workmen
//  9   DENSE_R     Dense
// 10   FOGGER1A    Fogger
// 11   MUD1A       Mud
// 12   RADIO2      Radio 2
// 13   ROLLOUT     Roll Out
// 14   SNAKE       Snake
// 15   TERMINAT    Terminate
// 16   TWIN        Twin
// 17   VERTOR1A    Vector
// 18   MAP         Map Selection theme
// 19   SCORE       Score screen theme
// 20   INTRO       Intro theme
// 21   CREDITS     End Credits theme

class TriggerAction;
class UnitOrStructure;

using std::string;

/**
 * Trigger in RedAlert missions
 */
class RA_Tiggers
{
public:
    string         		name;
    int                 repeatable;
    /** which country trigger applies to */
    int                 country;
    /** 
     * When to activate trigger 
     * 0 = Only trigger event 1 must be true
     * 1 = Triggger event one and two must be true
     * 2 = Either the first or the second trigger event
     *      must be true (activate all associated action on trigger1 
     *      or trigger2)
     * 3 = Either the first or the second trigger event must be true 
     *      (activate action 1 for trigger1, activate action2 for 
     *      trigger2)
     */
    int                 activate;
    /** which actions are triggered, 
     * 0 == only one trigger action, 
     * 1 == both trigger actions are activated
     */
    int                 actions;
    /** first trigger */
    RA_Tigger           trigger1;
    /** second trigger */
    RA_Tigger           trigger2;       
    /** first trigger action */
    TriggerAction*    action1;  
    /** second trigger action,  If this trigger has more than 
     * one trigger action associated with an event, then the 
     * second trigger action is held here.*/
    TriggerAction*    action2;     
    /** var to remember if the trigger has already been exectured */
	bool				hasexecuted;
};



//
// Forward declarations
//
void PrintTrigger ( RA_Tiggers Trigger );
void HandleTriggers ( UnitOrStructure *UnitOrStructure, int Event, int param = 0 );
void HandleGlobalTrigger ( int Event, int param );
void CheckCellTriggers ( Uint32 pos );
//void ExecuteTriggerAction ( unsigned int Event, Uint8 ActionNr, RA_Tiggers *Trigger );

void ExecuteTriggerAction(TriggerAction* action);
string getTriggerEventNameByNumber(Uint8 number);

#endif //TRIGGERS_H_
