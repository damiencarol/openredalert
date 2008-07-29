// Triggers.cpp
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

#include "triggers.h"

#include <cstdlib>
#include <cstring>
#include <string>

#include "include/Logger.h"
#include "include/config.h"
#include "audio/SoundEngine.h"
#include "CnCMap.h"
#include "UnitAndStructurePool.h"
#include "Unit.h"
#include "PlayerPool.h"
#include "RA_Tigger.h"
#include "Player.h"
#include "TriggerAction.h"
#include "TextTriggerAction.h"
#include "RawTriggerAction.h"
#include "GlobalSetTriggerAction.h"

namespace p {
	extern CnCMap* ccmap;
	extern UnitAndStructurePool* uspool;
}
namespace pc {
    extern ConfigType Config;
    extern SoundEngine* sfxeng;
}
extern Logger * logger;

/** Global variables for triggers */
bool GlobalVar[100];

/** 
 * Check that the event parameters are correct
 * 
 * for TRIGGER_EVENT_SPIED_BY
 * - value = house num of the spy
 *  
 * @param Event the event that was triggerd
 * @param param1 the parameter1 needed for the event
 * @param param2 the parameter2 needed for the event
 * @param value the value that the event has now
 * @returns void
 */
bool CheckEvent(unsigned int Event, int param1, int param2, unsigned int Eventparam, int value)
{
    switch (Event)
    {
    	//
        // No Events (ONLY THROW BY FORCE TRIGGER) 
    	//
        case TRIGGER_EVENT_NO_EVENT:
        	if (Event == Eventparam){
        		return true;
        	}
        	break;
        //
        // value = HouseNum of the player
        //
        case TRIGGER_EVENT_SPIED_BY:
        	if (Event == Eventparam){
        		if (param2 == value) {
        			return true;
        		}
        	}
        	break;
        //
        // value = HouseNum of the player
        //
        case TRIGGER_EVENT_DISCOVERED_BY:
        	// @todo implemente this
        	break;
        //
        // value = HouseNum of the player
        //
        case TRIGGER_EVENT_ATTACKED:
        	if (Event == Eventparam){
        		if (value == param2){
        			return true;
        		}
        	}
        	break;
        // 
        //
        case TRIGGER_EVENT_DESTROYED:        	        	
        	if (Event == Eventparam){
        		return true;
        	}
        	break;
        // 
        // ALL EVENT but NO "NO_EVENT"
        case TRIGGER_EVENT_ANY_EVENT:
        	if (Eventparam != TRIGGER_EVENT_NO_EVENT){
        		return true;
        	}
        	break;   	
        case TRIGGER_EVENT_MISSION_TIMER_EXPIRED:
        case TRIGGER_EVENT_NO_FACTORIES_LEFT:
            return false;
            break;

        // Events with parameters
        case TRIGGER_EVENT_TIME_ELAPSED:
            if (param2 <= value)
                return true;
            break;

        // Unit of a house enter somewhere
        // param2 = num of the global var
        case TRIGGER_EVENT_ENTERED_BY:
        	if (Event == Eventparam){
        		if (param2 == value){
        			return true;
        		}
        	}
        	break;
        // Unhandled events
        case TRIGGER_EVENT_THIEVED_BY:
        case TRIGGER_EVENT_HOUSE_DISCOVERED:
        case TRIGGER_EVENT_ALL_UNITS_DESTROYED:
        case TRIGGER_EVENT_ALL_BUILDINGS_DESTROYED:
        case TRIGGER_EVENT_ALL_DESTROYED:
        case TRIGGER_EVENT_CREDITS_EXCEED:
        case TRIGGER_EVENT_DESTROYED_NR_BUILDINGS:
        case TRIGGER_EVENT_DESTROYED_NR_UNITS:
        case TRIGGER_EVENT_CIVILIANS_EVACUATED:
        case TRIGGER_EVENT_BUILD_BUILDING_TYPE:
        case TRIGGER_EVENT_BUILD_UNIT_TYPE:
        case TRIGGER_EVENT_BUILD_INFANTRY_TYPE:
        case TRIGGER_EVENT_BUILD_AIRCRAFT_TYPE:
        case TRIGGER_EVENT_LEAVES_MAP:
        case TRIGGER_EVENT_ZONE_ENTRY:
        case TRIGGER_EVENT_CROSSED_HORIZONTAL_LINE:
        case TRIGGER_EVENT_CROSSED_VERTICAL_LINE:
        	break;
        //
        // param2 = num of the global var
        //
        case TRIGGER_EVENT_GLOBAL_IS_SET:
        	if (GlobalVar[param2] == true){
        		return true;
        	}
        	break;
        //
        // param2 = num of the global var
        //
        case TRIGGER_EVENT_GLOBAL_IS_CLEAR:
        	if (GlobalVar[param2] == false){
        		return true;
        	}
        	break;
        case TRIGGER_EVENT_DESTROYED_FAKES_ALL:
        	printf ("%s line %i: CheckParameters, Event %i not handled jet -> skip ckeck\n", __FILE__, __LINE__, Event);
        	return false;
        	break;
        case TRIGGER_EVENT_LOW_POWER:
        {
        	// Check if a player have low power (<=> PowerUsed > Power)
        	// param2 = player to check
        	printf ("%s line %i: CheckParameters, Event TRIGGER_EVENT_LOW_POWER try to analysis\n", __FILE__, __LINE__);
        	// The player to check
        	Player* pl = 0;
        	pl = p::ppool->getPlayer((Uint8)param2);
        	printf ("%s line %i: CheckParameters, TRIGGER_EVENT_LOW_POWER analysis player %s\n", __FILE__, __LINE__, pl->getName());
        	if (pl->getPowerUsed() > pl->getPower())
        	{
        		printf ("%s line %i: CheckParameters, Event TRIGGER_EVENT_LOW_POWER decide TRUE\n", __FILE__, __LINE__);        		        	
        		return true;
        	}
        	else
        	{
        		printf ("%s line %i: CheckParameters, Event TRIGGER_EVENT_LOW_POWER decide FALSE\n", __FILE__, __LINE__);        		
        		return false;
        	}
        	break;
        }
        case TRIGGER_EVENT_ALL_BRIDGES_DESTROYED:
        case TRIGGER_EVENT_BUILDING_EXISTS:
            printf ("%s line %i: CheckParameters, Event %i not handled jet -> skip ckeck\n", __FILE__, __LINE__, Event);
            return false;
            break;

        default:
            printf ("%s line %i: CheckParameters, Event %i not supported\n", __FILE__, __LINE__, Event);
            break;
    }

    return false;
}

/**
 * 
 */
/*
bool CheckOtherEvent(unsigned int Event, int param1, int param2, int value)
{
    switch (Event)
    {
        case TRIGGER_EVENT_NO_EVENT:
        case TRIGGER_EVENT_SPIED_BY:
        case TRIGGER_EVENT_DISCOVERED_BY:
        case TRIGGER_EVENT_ATTACKED:
        case TRIGGER_EVENT_DESTROYED:
        case TRIGGER_EVENT_ANY_EVENT:
        case TRIGGER_EVENT_MISSION_TIMER_EXPIRED:
        case TRIGGER_EVENT_NO_FACTORIES_LEFT:
        case TRIGGER_EVENT_TIME_ELAPSED:
        case TRIGGER_EVENT_ENTERED_BY:
        case TRIGGER_EVENT_THIEVED_BY:
        case TRIGGER_EVENT_HOUSE_DISCOVERED:
        case TRIGGER_EVENT_ALL_UNITS_DESTROYED:
        case TRIGGER_EVENT_ALL_BUILDINGS_DESTROYED:
        case TRIGGER_EVENT_ALL_DESTROYED:
        case TRIGGER_EVENT_CREDITS_EXCEED:
        case TRIGGER_EVENT_DESTROYED_NR_BUILDINGS:
        case TRIGGER_EVENT_DESTROYED_NR_UNITS:
        case TRIGGER_EVENT_CIVILIANS_EVACUATED:
        case TRIGGER_EVENT_BUILD_BUILDING_TYPE:
        case TRIGGER_EVENT_BUILD_UNIT_TYPE:
        case TRIGGER_EVENT_BUILD_INFANTRY_TYPE:
        case TRIGGER_EVENT_BUILD_AIRCRAFT_TYPE:
        case TRIGGER_EVENT_LEAVES_MAP:
        case TRIGGER_EVENT_ZONE_ENTRY:
        case TRIGGER_EVENT_CROSSED_HORIZONTAL_LINE:
        case TRIGGER_EVENT_CROSSED_VERTICAL_LINE:
        	return false;
        	break;
        // !!!
        case TRIGGER_EVENT_GLOBAL_IS_SET:
        	if (GlobalVar[param2] == true)
        		return true;
        	else
        		return false;
        	break;
        // !!!
        case TRIGGER_EVENT_GLOBAL_IS_CLEAR:
        	if (GlobalVar[param2] == false)
        		return true;
        	else
        		return false;
        	break;
        case TRIGGER_EVENT_DESTROYED_FAKES_ALL:
        	printf ("%s line %i: CheckOtherEvent, Event %i not handled jet -> skip ckeck\n", __FILE__, __LINE__, Event);
        	return false;
        	break;
        case TRIGGER_EVENT_LOW_POWER:
        {
        	// Check if a player have low power (<=> PowerUsed > Power)
        	// param2 = player to check
        	printf ("%s line %i: CheckParameters, Event TRIGGER_EVENT_LOW_POWER try to analysis\n", __FILE__, __LINE__);
        	// The player to check
        	Player* pl = p::ppool->getPlayer((Uint8)param2);
        	printf ("%s line %i: CheckParameters, TRIGGER_EVENT_LOW_POWER analysis player %s\n", __FILE__, __LINE__, pl->getName());
        	if (pl->getPowerUsed() > pl->getPower())
        	{
        		printf ("%s line %i: CheckParameters, Event TRIGGER_EVENT_LOW_POWER decide TRUE\n", __FILE__, __LINE__);
        		return true;
        	} else {
        		printf ("%s line %i: CheckParameters, Event TRIGGER_EVENT_LOW_POWER decide FALSE\n", __FILE__, __LINE__);
        		return false;
        	}
        	break;
        }     	        
        case TRIGGER_EVENT_ALL_BRIDGES_DESTROYED:
        case TRIGGER_EVENT_BUILDING_EXISTS:
            printf ("%s line %i: CheckOtherEvent, Event %i not handled jet -> skip ckeck\n", __FILE__, __LINE__, Event);
            return false;
            break;

        default:
            printf ("%s line %i: CheckOtherEvent, Event %i not supported\n", __FILE__, __LINE__, Event);
            break;
    }

    return false;
}
*/
/*
bool CheckSecondTriggerEvent(int TriggerNumb, RA_Tiggers  *Trigger)
{
	int EventToCheck;
	int param2;
	
	if (TriggerNumb == 1){
		EventToCheck	= Trigger->trigger1.event;
		param2		= Trigger->trigger1.param2;
	}else{
		EventToCheck	= Trigger->trigger2.event;
		param2		= Trigger->trigger2.param2;
	}

	switch (EventToCheck)
	{
		case TRIGGER_EVENT_GLOBAL_IS_SET:
			if (GlobalVar[param2] == true){
				//printf ("PARAMCHECK OKE|\n");
				return true;
			}
			break;
		case TRIGGER_EVENT_GLOBAL_IS_CLEAR:
			if (!GlobalVar[param2] == false){
				//printf ("PARAMCHECK OKE|\n");
				return true;
			}
			break;
		default:
			printf ("%s line %i: Unhandled trigger event = %i\n", __FILE__, __LINE__, EventToCheck);
			break;
	}
	return false;
}
*/
/** 
 * Handle UnitOrStructure triggers
 * 
 * @param UnitOrStructure the unit or structure that has the event happening
 * @param Event the event that was caused for this unit or structure
 * @param param this is a event parameter ( doesn't have to be used )
 * @returns void
 */
void HandleTriggers(UnitOrStructure* UnitOrStructure, int Event, int param)
{
	string AssociatedTriggerName = "None";
	RA_Tiggers* AssociatedTrigger = 0;
	//int			value = 0;
	
	logger->debug("HandleTriggers with event=%s param=%d\n", 
			getTriggerEventNameByNumber(Event).c_str(), 
			param);
	
	// Check if there are someone behind this trigger :)
    if (UnitOrStructure == 0){
    	logger->warning("%s line %i: No structure defined ! \n", __FILE__, __LINE__);
        return;
    }
    

    // Get the trigger name from the unit or structure
    AssociatedTriggerName = UnitOrStructure->getTriggerName();
    // If they are no trigger then escape
    if (AssociatedTriggerName == "None"){
        return;
    }
    logger->debug("Handle trigger [%s]\n", AssociatedTriggerName.c_str());
    
    //
    // Find the associated trigger in the map
    //
    AssociatedTrigger = p::ccmap->getTriggerByName(AssociatedTriggerName);
    if (AssociatedTrigger == 0){
        printf ("Trigger not found %s\n", AssociatedTrigger->name.c_str());
        return;
    }

#if 0
    int countrynr = p::ppool->getHouseNumByPlayerNum(UnitOrStructure->getOwner());

    // Check if the trigger was meant for us
    if (countrynr != AssociatedTrigger->country){
        return;
    }
#endif
    
//    printf ("%s line %i: Trigger name = %s, unitorstructurename = %c%c%c\n", __FILE__, __LINE__, AssociatedTriggerName.c_str(), UnitOrStructure->getType()->getTName()[0], UnitOrStructure->getType()->getTName()[1], UnitOrStructure->getType()->getTName()[2]);
    
    // Check if already done 	
  	switch (AssociatedTrigger->repeatable)
  	{
  	case TRIGGER_NON_REPEATING:
  		//printf("%s line %i: NON REPEATING TRIGGER, %i\n", __FILE__, __LINE__, AssociatedTrigger->repeatable);
  		if (AssociatedTrigger->hasexecuted == true){
  			//printf("ALREADY DONE !!! (RETURN)\n");
  			return;
  		}
  		break;
  	case TRIGGER_REPEAT_ONCE_ALL:
  		//printf("%s line %i: SEMI REPEATING %i\n", __FILE__, __LINE__, AssociatedTrigger->repeatable);
  		break;
  	case TRIGGER_FREE_REPEAT:
  		//printf ("%s line %i: FREE REPEATING TRIGGER %i\n", __FILE__, __LINE__, AssociatedTrigger->repeatable);
  		break;
  	}

    switch (AssociatedTrigger->activate)
    {
        // Only trigger event 1 must be true
        case 0:
        	logger->debug("case 0\n");
            if (CheckEvent(AssociatedTrigger->trigger1.event,
            			   AssociatedTrigger->trigger1.param1,
            			   AssociatedTrigger->trigger1.param2,
            			   Event,
            			   param) == true)
            {
            	logger->debug("EVENT CHECK IS OK\n");
            	// Set that Trigger is executed
            	AssociatedTrigger->hasexecuted = true;
            	
            	// Execute Action 1
                ExecuteTriggerAction(AssociatedTrigger->action1);
                // If actions = 1 execute Action 2
                if (AssociatedTrigger->actions == 1){
                    ExecuteTriggerAction(AssociatedTrigger->action2);
                }
            }
            break;
            
        // EVENT 1 & EVENT 2  ==> ACTION 1
        // Triggger event one and two must be true
        //
        case 1:
        	logger->debug("case 1\n");
        	if ((CheckEvent(AssociatedTrigger->trigger1.event,
        				   AssociatedTrigger->trigger1.param1,
        				   AssociatedTrigger->trigger1.param2,
        				   Event,
        				   param) == true) &&
        		(CheckEvent(AssociatedTrigger->trigger2.event,
        				   AssociatedTrigger->trigger2.param1,
        				   AssociatedTrigger->trigger2.param2,
        				   Event,
        				   param) == true))
        	{
        	
        				               
        	            
			/*if (AssociatedTrigger->trigger1.event == Event){
				// Trigger 1 is already oke, check trigger 2
				if (CheckpaSecondTriggerEvent (2, AssociatedTrigger)){
					ExecuteTriggerAction (Event, 1, AssociatedTrigger );
					if (AssociatedTrigger->actions == 1){
						ExecuteTriggerAction (Event, 2, AssociatedTrigger );
					}
				}
			}else if (AssociatedTrigger->trigger2.event == Event){
				// Trigger 2 is already oke, check trigger 1
				if (CheckSecondTriggerEvent (1, AssociatedTrigger)){
					ExecuteTriggerAction (Event, 1, AssociatedTrigger );
					if (AssociatedTrigger->actions == 1){
						ExecuteTriggerAction (Event, 2, AssociatedTrigger );
					}
				}
			}else
				printf ("%s line %i: ERROR, this should not happen\n", __FILE__, __LINE__);
*/
            //printf ("%s line %i: @todo: ********************trigger event1 and trigger event2 must be true check: T1 = %i, T2 = %i, param = %i *******************\n", __FILE__, __LINE__, AssociatedTrigger->trigger1.event, AssociatedTrigger->trigger2.event, value);
        	}
        	break;
        // Either the first or the second trigger event must be true (activate all associated action on trigger1 or trigger2)
        case 2:
        	logger->debug("case 2\n");
        	            
        	/*
            if (AssociatedTrigger->trigger1.event == Event){
                ExecuteTriggerAction (Event, 1, AssociatedTrigger );
                if (AssociatedTrigger->actions == 1){
                    ExecuteTriggerAction (Event, 2, AssociatedTrigger );
                }
            }
            if (AssociatedTrigger->trigger2.event == Event){
                ExecuteTriggerAction (Event, 1, AssociatedTrigger );
                if (AssociatedTrigger->actions == 1){
                    ExecuteTriggerAction (Event, 2, AssociatedTrigger );
                }
            }*/
            break;
        // Either the first or the second trigger event must be true (activate action 1 for trigger1, activate action2 for trigger2)
        case 3:
        	logger->debug("case 3\n");
        	            
        	/*
            if (AssociatedTrigger->trigger1.event == Event){
                ExecuteTriggerAction (Event, 1, AssociatedTrigger );
            }
            if (AssociatedTrigger->trigger2.event == Event){
                ExecuteTriggerAction (Event, 2, AssociatedTrigger );
            }
*/
            break;
        default:
            printf ("%s line %i: Invalid trigger activation\n", __FILE__, __LINE__);
            break;
    }
}

/** 
 * @Handle global triggers
 * 
 * @param Event the event that was caused for this unit or structure
 * @param param the parameter that goes with the event (example time eleapsed)
 * @returns void
 */
void HandleGlobalTrigger(int Event, int value)
{
	RA_Tiggers  *Trigger;
	int         TriggNumb = 0;
	
	
	// Debug
	printf("HandleGlobalTrigger with event=%s param=%d\n", 
				getTriggerEventNameByNumber(Event).c_str(), 
				value);

	// For each trigger
	while ((Trigger = p::ccmap->getTriggerByNumb(TriggNumb)) != 0)
	{
		TriggNumb++;

		if (Trigger == 0){
			return;
		}
		//logger->debug("TRIG = [%s]  (activate=%d)\n", Trigger->name.c_str(), Trigger->activate);

		//if (Trigger->name == "EINS")
		//		continue;
		//if (Trigger->name == "REVL")
		//		continue;
		//if (Trigger->name == "RSPD")
		//		continue;
				
		// Check if already done 	
		switch (Trigger->repeatable)
		{
		case TRIGGER_NON_REPEATING:
			printf("%s line %i: NON REPEATING TRIGGER, %i\n", __FILE__, __LINE__, Trigger->repeatable);
			if (Trigger->hasexecuted == true){
				printf("ALREADY DONE !!! (RETURN)\n");
				return;
			}
			break;
		case TRIGGER_REPEAT_ONCE_ALL:
			printf("%s line %i: SEMI REPEATING %i\n", __FILE__, __LINE__, Trigger->repeatable);
			break;
		case TRIGGER_FREE_REPEAT:
			printf ("%s line %i: FREE REPEATING TRIGGER %i\n", __FILE__, __LINE__, Trigger->repeatable);
			break;
		}


		switch (Trigger->activate)
		{
			// Only trigger event 1 must be true
			case 0:
			{
				// If the event is not true then continue to the next trigger
				if (!CheckEvent(Trigger->trigger1.event,
							Trigger->trigger1.param1, 
							Trigger->trigger1.param2, 
							Event, 
							value))
						continue;
				
				logger->debug("TRIG [%s] Event check ok \n", Trigger->name.c_str());
				Trigger->hasexecuted = true;
										
				ExecuteTriggerAction(Trigger->action1);
				if (Trigger->actions == 1){
					ExecuteTriggerAction(Trigger->action2);
				}
				
				break;
			}
			// Triggger event one and two must be true
			case 1:
				/*if (Trigger->trigger1.event == Event && !CheckParameters (Event, Trigger->trigger1.param1, Trigger->trigger1.param2, value)){
					continue;
				}
				if (Trigger->trigger2.event == Event  && !CheckParameters (Event, Trigger->trigger2.param1, Trigger->trigger2.param2, value)){
					continue;
				}
				if (Trigger->trigger1.event == Event){
					// Trigger 1 is already oke, check trigger 2
					if (CheckSecondTriggerEvent (2, Trigger)){
						//printf ("Execute trigger action\n");
						ExecuteTriggerAction ( Event, 1, Trigger );
						if (Trigger->actions == 1){
							ExecuteTriggerAction ( Event, 2, Trigger );
						}
					}
				}else if (Trigger->trigger2.event == Event){
					// Trigger 2 is already oke, check trigger 1
					if (CheckSecondTriggerEvent (1, Trigger)){
						//printf ("Execute trigger action\n");
						ExecuteTriggerAction ( Event, 1, Trigger );
						if (Trigger->actions == 1){
							ExecuteTriggerAction ( Event, 2, Trigger );
						}
					}

				}else
					printf ("%s line %i: ERROR, this should not happen\n", __FILE__, __LINE__);
				//printf ("%s line %i: @todo: ********************trigger event1 and trigger event2 must be true check: T1 = %i, T2 = %i, param = %i *******************\n", __FILE__, __LINE__, Trigger->trigger1.event, Trigger->trigger2.event, value);
				*/break;
			
			// Either the first or the second trigger event must be 
			// true (activate all associated action on trigger1 or 
			// trigger2)
			case 2:
				/*
				if (Trigger->trigger1.event == Event){
					if (Trigger->trigger1.event == TRIGGER_EVENT_TIME_ELAPSED && Trigger->trigger1.param2 < value)
						continue;
					ExecuteTriggerAction ( Event, 1, Trigger );
					if (Trigger->actions == 1){
						ExecuteTriggerAction ( Event, 2, Trigger );
					}
				}
				if (Trigger->trigger2.event == Event){
					if (Trigger->trigger2.event == TRIGGER_EVENT_TIME_ELAPSED && Trigger->trigger2.param2 < value)
						continue;
					ExecuteTriggerAction ( Event, 1, Trigger );
					if (Trigger->actions == 1){
						ExecuteTriggerAction ( Event, 2, Trigger );
					}
				}*/
				break;
			// Either the first or the second trigger event must be true (activate action 1 for trigger1, activate action2 for trigger2)
			case 3:/*
				if (Trigger->trigger1.event == Event){
					if (Trigger->trigger1.event == TRIGGER_EVENT_TIME_ELAPSED && !CheckParameters (Event, Trigger->trigger1.param1, Trigger->trigger1.param2, value))
						continue;
					ExecuteTriggerAction ( Event, 1, Trigger );
				}
				if (Trigger->trigger2.event == Event){
					if (Trigger->trigger2.event == TRIGGER_EVENT_TIME_ELAPSED && Trigger->trigger2.param2 < value)
						continue;
					ExecuteTriggerAction ( Event, 2, Trigger );
				}*/
				break;
			default:
				printf ("%s line %i: Invalid trigger activation\n", __FILE__, __LINE__);
				break;
		}
	}
}


void CheckCellTriggers(Uint32 pos)
{
	//UnitOrStructure *unitOrStructure;
	Unit            *unit;
	RA_Tiggers      *Trigger;

/*
    unitOrStructure = p::uspool->getUnitOrStructureAt(pos,0);

    if (unitOrStructure == NULL)
        return;

    if (unitOrStructure->getType()->isStructure())
        return;

    unit = (Unit*) unitOrStructure;
*/
    unit = p::uspool->getGroundUnitAt(pos);

	if (unit == 0){
    	unit = p::uspool->getFlyingAt(pos);
	}
	if (unit == 0){
		return;
	}


    if (unit->getTriggerName() == "None")
    {
        return;
    }

    Trigger = p::ccmap->getTriggerByName(unit->getTriggerName().c_str());

    if (Trigger == 0){
        return;
    }
    int countrynr = p::ppool->getHouseNumByPlayerNum(unit->getOwner());

    // Check if the trigger was meant for us
    if (countrynr != Trigger->country)
        return;

    switch (Trigger->repeatable){
        case TRIGGER_NON_REPEATING:
            printf ("%s line %i: NON REPEATING TRIGGER, %i\n", __FILE__, __LINE__, Trigger->repeatable);
            break;

        case TRIGGER_REPEAT_ONCE_ALL:
            printf ("%s line %i: SEMI REPEATING %i\n", __FILE__, __LINE__, Trigger->repeatable);
            break;

        case TRIGGER_FREE_REPEAT:
            printf ("%s line %i: FREE REPEATING TRIGGER %i\n", __FILE__, __LINE__, Trigger->repeatable);
            break;
    }

    printf ("%s line %i: Celltrigger triggered, trigger is %s, trigger country = %i, side = %i\n", __FILE__, __LINE__, Trigger->name.c_str(), Trigger->country, countrynr);

    // player->getPlayerNum()
    // player->getName()

#if 1
    PrintTrigger ( *Trigger );
#endif
}


void ExecuteTriggerAction(TriggerAction* action)
//void ExecuteTriggerAction(unsigned int Event, Uint8 ActionNr, RA_Tiggers *Trigger )
{
    /*unsigned int    Action;
    int             parameter;
    RA_Tiggers      *Trig;
    Structure       *str;
    std::string     TriggerName;
    int             TeamNr;
    RA_Teamtype     *Team;
    Uint32 Waypoint = 0;*/ // For reveal around waypoint/zone

	// Check if the trigger should be executed (if it is not 
    // repeatable and has already exectued once it should not 
    // exectute again)
	/*if (!Trigger->repeatable && Trigger->hasexecuted){
		return;
	}

	// Set the has executed var now (to prevent inf loops)
	Trigger->hasexecuted = true;
	p::ccmap->setTriggerByName(Trigger->name.c_str(), Trigger);


	if (ActionNr == 1){
		Action = Trigger->action1->getAction();
	}else if (ActionNr == 2){
		Action = Trigger->action2->getAction();
	}else {
		printf("error ActionNr != 1 or 2 ERRROR !!! \n");
		return;
	}
	*/
	if (action == 0) {
		logger->error("action = NULL\n!!!!");
	}
	//printf("TRIGGER->action : %d\n", action->getAction());
	
    switch (action->getAction())
    {
        case TRIGGER_ACTION_NO_ACTION:
        	logger->error ("%s line %i: ***TRIGGER_ACTION_NO_ACTION***\n", __FILE__, __LINE__);
        	action->execute();
        	break;
        case TRIGGER_ACTION_WINNER_IS:
            logger->error ("%s line %i: ***TRIGGER_ACTION_WINNER_IS***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_LOSER_IS:
        {
        	RawTriggerAction* actTrig = 0;
        	actTrig = dynamic_cast<RawTriggerAction*>(action);
        	
        	logger->error ("%s line %i: ***TRIGGER_ACTION_LOSER_IS = %d ***\n", __FILE__, __LINE__, actTrig->getParam3());
        	
        	p::ppool->playerDefeated(        	    
        		p::ppool->getPlayer(
        			p::ppool->getPlayerNumByHouseNum(actTrig->getParam3())
        		)
        	);
        	break;
        }
        case TRIGGER_ACTION_PRODUCTION_BEGINS:
            logger->error ("%s line %i: ***TRIGGER_ACTION_PRODUCTION_BEGINS***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_CREATE_TEAM:
            logger->error ("%s line %i: ***TRIGGER_ACTION_CREATE_TEAM***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_DESTROY_TEAM:
            logger->error ("%s line %i: ***TRIGGER_ACTION_DESTROY_TEAM***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_ALL_TO_HUNT:
            logger->error ("%s line %i: ***TRIGGER_ACTION_ALL_TO_HUNT***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_REINFORCEMENTS:
        	{
        		// The action to execute
        		RawTriggerAction* actTrig = 0;
        		actTrig = dynamic_cast<RawTriggerAction*>(action);
        		
        		//TeamNr = Trigger->action1->param1;
        		int TeamNr = actTrig->getParam1();
        		RA_Teamtype* Team = 0;
        		Team = p::ccmap->getTeamtypeByNumb(TeamNr);
        		if (Team != NULL)
        		{
        			logger->warning ("Reinforcement team = %s\n", Team->tname.c_str());
        			p::uspool->createReinforcements(Team);
        		}
        		// Play the reinforcements have arrived tune
        		pc::sfxeng->PlaySound(pc::Config.Reinforcements);
			}
        	break;            
        case TRIGGER_ACTION_DROP_ZONE_FLARE:
            logger->error ("%s line %i: ***TRIGGER_ACTION_DROP_ZONE_FLARE***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_FIRE_SALE:
            logger->error ("%s line %i: ***TRIGGER_ACTION_FIRE_SALE***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_PLAY_MOVIE:
            logger->error ("%s line %i: ***TRIGGER_ACTION_PLAY_MOVIE***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_TEXT:
        	// Log it
        	logger->error ("%s line %i: ***TRIGGER_ACTION_TEXT***\n", __FILE__, __LINE__);
        	// Execute the action
        	action->execute();
            break;
        case TRIGGER_ACTION_DESTROY_TRIGGER:
            logger->error ("%s line %i: ***RIGGER_ACTION_DESTROY_TRIGGER***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_AUTOCREATE_BEGINS:
            logger->error ("%s line %i: ***TRIGGER_ACTION_AUTOCREATE_BEGINS***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_ALLOW_WIN:
            logger->error ("%s line %i: ***TRIGGER_ACTION_ALLOW_WIN***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_REVEAL_MAP:
        	//logger->error ("%s line %i: ***TRIGGER_ACTION_REVEAL_MAP***\n", __FILE__, __LINE__);
        	//ppool->getLPlayer()->revealAroundWaypoint(Uint32 Waypoint);
        	p::ppool->getLPlayer()->setVisBuild(Player::SOB_SIGHT, true);
        	break;
        case TRIGGER_ACTION_REVEAL_AROUND_WAYPOINT:
        {
        	logger->error ("%s line %i: ***TRIGGER_ACTION_REVEAL_AROUND_WAYPOINT***\n", __FILE__, __LINE__);
        	        	
        	// The action to execute
        	RawTriggerAction* actionTrig = 0;
        	actionTrig = dynamic_cast<RawTriggerAction*>(action);
        	int Waypoint = actionTrig->getParam3();
        	logger->error ("%s line %i: ***TRIGGER_ACTION_REVEAL_AROUND_WAYPOINT***, waypoint1 == %u\n", __FILE__, __LINE__, Waypoint);
        	p::ppool->getLPlayer()->revealAroundWaypoint(Waypoint);
        	break;
        }
        case TRIGGER_ACTION_REVEAL_ZONE_OF_WAYPOINT:
        	/// This is incorrect this funtion should reveal a complete zone, two possibilities
        	/// 1. Everything around the waypoint where one would be able to walk / drive
        	/// 2. If the waypoint is over a place where one can't walk/drive reveal all places on the map where one can't walk/drive
        	/*if (ActionNr == 1){
        	 * 			Waypoint = Trigger->action1->param3;
        	 * 		}else if (ActionNr == 2){
        	 * 			Waypoint = Trigger->action2->param3;
        	 * 		}
        	 * 		p::ppool->getLPlayer()->revealAroundWaypoint(Waypoint);*/
        	logger->error ("%s line %i: ***TRIGGER_ACTION_REVEAL_ZONE_OF_WAYPOINT***\n", __FILE__, __LINE__);
        	break;
        case TRIGGER_ACTION_PLAY_SOUND:
            logger->error ("%s line %i: ***TRIGGER_ACTION_PLAY_SOUND***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_PLAY_MUSIC:
            logger->error ("%s line %i: ***TRIGGER_ACTION_PLAY_MUSIC***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_PLAY_SPEECH:
            logger->error ("%s line %i: ***TRIGGER_ACTION_PLAY_SPEECH***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_FORCE_TRIGGER:
        {
        	logger->error("%s line %i: ***TRIGGER_ACTION_FORCE_TRIGGER***\n", __FILE__, __LINE__);
        	
        	RawTriggerAction* actTrig = 0;
        	actTrig = dynamic_cast<RawTriggerAction*>(action);
        	
        	int parameter = actTrig->getParam2();

        	RA_Tiggers* Trig =  p::ccmap->getTriggerByNumb(parameter);

        	// Set that Trigger is executed
        	Trig->hasexecuted = true;
        	
        	// Execute Action 1
        	ExecuteTriggerAction(Trig->action1);
        	// If actions = 1 execute Action 2
        	if (Trig->actions == 1){
        		ExecuteTriggerAction(Trig->action2);
        	}
            logger->warning("%s line %i: Force trigger %s\n", __FILE__, __LINE__, Trig->name.c_str());
            break;
        }
        case TRIGGER_ACTION_TIMER_START:
            logger->error ("%s line %i: ***TRIGGER_ACTION_TIMER_START***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_TIMER_STOP:
            logger->error ("%s line %i: ***TRIGGER_ACTION_TIMER_STOP***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_TIMER_EXTEND:
            logger->error ("%s line %i: ***TRIGGER_ACTION_TIMER_EXTEND***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_TIMER_SHORTEN:
            logger->error ("%s line %i: ***TRIGGER_ACTION_TIMER_SHORTEN***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_TIMER_SET:
            logger->error ("%s line %i: ***TRIGGER_ACTION_TIMER_SET***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_GLOBAL_SET:
        {
        	// Log it
        	logger->debug("%s line %i: ***TRIGGER_ACTION_GLOBAL_SET***\n", __FILE__, __LINE__);
        	// Executed the action
        	action->execute();        	
            break;
        }
        // Trigger to clear a global variable
        case TRIGGER_ACTION_GLOBAL_CLEAR:
        {
        	// Log it
        	logger->debug("%s line %i: ***TRIGGER_ACTION_GLOBAL_CLEAR***\n", __FILE__, __LINE__);
        	// Execute the action
        	action->execute();
        	break;
        }
        case TRIGGER_ACTION_AUTO_BASE_BUILDING:
            logger->error ("%s line %i: ***TRIGGER_ACTION_AUTO_BASE_BUILDING***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_GROW_SHROUD_ONE_STEP:
            logger->error ("%s line %i: ***TRIGGER_ACTION_GROW_SHROUD_ONE_STEP***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_DESTROY_BUILDING:
        	//break;
        	//PrintTrigger ( *Trigger );
			/*for (unsigned int i = 0; i < p::uspool->getNumbStructures(); i++){
				str = p::uspool->getStructure(i);
				if (str == NULL || !str->isAlive()){
					continue;
				}
				TriggerName = str->getTriggerName();
				for (unsigned int j=0; j<TriggerName.length(); ++j){
					TriggerName[j]=toupper(TriggerName[j]);
				}
				if (TriggerName == Trigger->name)
				{
					/// @todo DANGEROUS HACK
					/// this is a dangerous hack
					/// possibility off recursive function calls,
					/// possibility off infinite loops
					str->applyDamage(255, NULL, NULL);
				}
			}*/
            //logger->error ("%s line %i: **********************Unhandled trigger action**********************\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_ADD_1TIME_SPEC_WEAPON:
            logger->error ("%s line %i: ***TRIGGER_ACTION_ADD_1TIME_SPEC_WEAPON***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_ADD_SPEC_WEAPON:
            logger->error ("%s line %i: ***TRIGGER_ACTION_ADD_SPEC_WEAPON***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_PREFERRED_TARGET:
            logger->error ("%s line %i: ***TRIGGER_ACTION_PREFERRED_TARGET****\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_LAUNCH_NUKES:
            logger->error ("%s line %i: ***TRIGGER_ACTION_LAUNCH_NUKES***\n", __FILE__, __LINE__);
            break;
        default:
            logger->error ("%s line %i: BIG ERROR unkown trigger\n", __FILE__, __LINE__);
            break;
    }
}

/**
 * Print a Trigger
 */
void PrintTrigger(RA_Tiggers Trigger)
{
	printf ("%s line %i: Read trigger:\n", __FILE__, __LINE__);
	printf ("name = \t\t\t%s\n", Trigger.name.c_str());	
	printf ("repeatable = \t\t%i\n", Trigger.repeatable);
    printf ("country = \t\t%i\n", Trigger.country);
    printf ("activate = \t\t%i\n", Trigger.activate);
    printf ("actions = \t\t%i\n", Trigger.actions);
   
    printf ("trigger1.event = \t%s\n", getTriggerEventNameByNumber(Trigger.trigger1.event).c_str());
    //printf ("trigger1.event = \t%i\n", Trigger.trigger1.event);
    printf ("trigger1.param1 = \t%i\n", Trigger.trigger1.param1);
    printf ("trigger1.param2 = \t%i\n", Trigger.trigger1.param2);
    
    printf ("trigger2.event = \t%s\n", getTriggerEventNameByNumber(Trigger.trigger2.event).c_str());
    printf ("trigger2.param1 = \t%i\n", Trigger.trigger2.param1);
    printf ("trigger2.param2 = \t%i\n", Trigger.trigger2.param2);
    
    printf ("action1->Action = \t%i\n", Trigger.action1->getAction());
    /*
    printf ("action1->param1 = \t%i\n", Trigger.action1->param1);
    printf ("action1->param2 = \t%i\n", Trigger.action1->param2);
    printf ("action1->param3 = \t%i\n", Trigger.action1->param3);
    */
    printf ("action2->Action = \t%i\n", Trigger.action2->getAction());
    /*
    printf ("action2->param1 = \t%i\n", Trigger.action2->param1);
    printf ("action2->param2 = \t%i\n", Trigger.action2->param2);
    printf ("action2->param3 = \t%i\n", Trigger.action2->param3);
    */
    printf ("\n\n\n");
}

/**
 * Initialize Triggers
 * 
 * - Reset global vars
 */
void InitializeTriggers()
{
	// Reset all the Global Vars
    for (int i = 0; i < 100; i++){
        GlobalVar[i] = false;
    }
}

/**
 * Return the name of a trigger with this number
 */
string getTriggerEventNameByNumber(Uint8 number)
{
	string str;
	switch(number){
	case 0:	str = "TRIGGER_EVENT_NO_EVENT";	break;
	case 1:	str = "TRIGGER_EVENT_ENTERED_BY"; break;
	case 2:	str = "TRIGGER_EVENT_SPIED_BY"; break;
	case 3: str = "TRIGGER_EVENT_THIEVED_BY"; break;
	case 4: str = "TRIGGER_EVENT_DISCOVERED_BY"; break;
	case 5: str = "TRIGGER_EVENT_HOUSE_DISCOVERED"; break;
	case 6: str = "TRIGGER_EVENT_ATTACKED"; break;
	case 7: str = "TRIGGER_EVENT_DESTROYED"; break;
	case 8: str = "TRIGGER_EVENT_ANY_EVENT"; break;
	case 9: str = "TRIGGER_EVENT_ALL_UNITS_DESTROYED"; break;
	case 10: str = "TRIGGER_EVENT_ALL_BUILDINGS_DESTROYED"; break;
	case 11: str = "TRIGGER_EVENT_ALL_DESTROYED"; break;
	case 12: str = "TRIGGER_EVENT_CREDITS_EXCEED"; break;
	case 13: str = "TRIGGER_EVENT_TIME_ELAPSED"; break;
	case 14: str = "TRIGGER_EVENT_MISSION_TIMER_EXPIRED"; break;
	case 15: str = "TRIGGER_EVENT_DESTROYED_NR_BUILDINGS"; break;
	case 16: str = "TRIGGER_EVENT_DESTROYED_NR_UNITS"; break;
	case 17: str = "TRIGGER_EVENT_NO_FACTORIES_LEFT"; break;
	case 18: str = "TRIGGER_EVENT_CIVILIANS_EVACUATED"; break;
	case 19: str = "TRIGGER_EVENT_BUILD_BUILDING_TYPE"; break;
	case 20: str = "TRIGGER_EVENT_BUILD_UNIT_TYPE"; break;
	case 21: str = "TRIGGER_EVENT_BUILD_INFANTRY_TYPE"; break;
	case 22: str = "TRIGGER_EVENT_BUILD_AIRCRAFT_TYPE"; break;
	case 23: str = "TRIGGER_EVENT_LEAVES_MAP"; break;
	case 24: str = "TRIGGER_EVENT_ZONE_ENTRY"; break;
	case 25: str = "TRIGGER_EVENT_CROSSED_HORIZONTAL_LINE"; break;
	case 26: str = "TRIGGER_EVENT_CROSSED_VERTICAL_LINE"; break;
	case 27: str = "TRIGGER_EVENT_GLOBAL_IS_SET"; break;
	case 28: str = "TRIGGER_EVENT_GLOBAL_IS_CLEAR"; break;
	case 29: str = "TRIGGER_EVENT_DESTROYED_FAKES_ALL"; break;
	case 30: str = "TRIGGER_EVENT_LOW_POWER"; break;
	case 31: str = "TRIGGER_EVENT_ALL_BRIDGES_DESTROYED"; break;
	case 32: str = "TRIGGER_EVENT_BUILDING_EXISTS"; break;
	default : str = "TRIGGER_EVENT_????"; break;
	}
	return str;
}
