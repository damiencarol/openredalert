#include "include/triggers.h"

#include <cstdlib>
#include <cstring>
#include <string>

#include "include/ccmap.h"
#include "include/UnitAndStructurePool.h"
#include "game/Unit.h"
#include "audio/SoundEngine.h"
#include "include/PlayerPool.h"
#include "include/Logger.h"
#include "game/RA_Tigger.h"

namespace pc {
    extern ConfigType Config;
}
extern Logger * logger;

bool GlobalVar[100];

/** 
 * Check that the event parameters are correct
 * @param Event the event that was triggerd
 * @param param1 the parameter1 needed for the event
 * @param param2 the parameter2 needed for the event
 * @param value the value that the event has now
 * @returns void
 */
bool CheckParameters (unsigned int Event, int param1, int param2, int value)
{
    switch (Event){
        // Events without parameters
        case TRIGGER_EVENT_NO_EVENT:
        case TRIGGER_EVENT_SPIED_BY:
        case TRIGGER_EVENT_DISCOVERED_BY:
        case TRIGGER_EVENT_ATTACKED:
        case TRIGGER_EVENT_DESTROYED:
        case TRIGGER_EVENT_ANY_EVENT:
        case TRIGGER_EVENT_MISSION_TIMER_EXPIRED:
        case TRIGGER_EVENT_NO_FACTORIES_LEFT:
            return true;
            break;

        // Events with parameters
        case TRIGGER_EVENT_TIME_ELAPSED:
            if (param2 <= value)
                return true;
            break;

        // Unhandled events
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
        case TRIGGER_EVENT_GLOBAL_IS_SET:
        case TRIGGER_EVENT_GLOBAL_IS_CLEAR:
        case TRIGGER_EVENT_DESTROYED_FAKES_ALL:
        case TRIGGER_EVENT_LOW_POWER:
        case TRIGGER_EVENT_ALL_BRIDGES_DESTROYED:
        case TRIGGER_EVENT_BUILDING_EXISTS:
            printf ("%s line %i: CheckParameters, Event %i not handled jet -> skip ckeck\n", __FILE__, __LINE__, Event);
            return true;
            break;

        default:
            printf ("%s line %i: CheckParameters, Event %i not supported\n", __FILE__, __LINE__, Event);
            break;
    }

    return false;
}

bool CheckOtherEvent (unsigned int Event, int param1, int param2, int value)
{
    switch (Event){
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
        case TRIGGER_EVENT_GLOBAL_IS_SET:
        case TRIGGER_EVENT_GLOBAL_IS_CLEAR:
        case TRIGGER_EVENT_DESTROYED_FAKES_ALL:
        case TRIGGER_EVENT_LOW_POWER:
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


bool CheckSecondTriggerEvent (int TriggerNumb, RA_Tiggers  *Trigger)
{
int EventToCheck,
	param2;
	if (TriggerNumb == 1){
		EventToCheck	= Trigger->trigger1.event;
		param2		= Trigger->trigger1.param2;
	}else{
		EventToCheck	= Trigger->trigger2.event;
		param2		= Trigger->trigger2.param2;
	}

	switch (EventToCheck){
		case TRIGGER_EVENT_GLOBAL_IS_SET:
			if (GlobalVar[param2]){
				//printf ("PARAMCHECK OKE|\n");
				return true;
			}
			break;
		case TRIGGER_EVENT_GLOBAL_IS_CLEAR:
			if (!GlobalVar[param2]){
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

/** 
 * Handle UnitOrStructure triggers
 * 
 * @param UnitOrStructure the unit or structure that has the event happening
 * @param Event the event that was caused for this unit or structure
 * @param param this is a event parameter ( doesn't have to be used )
 * @returns void
 */
void HandleTriggers (UnitOrStructure* UnitOrStructure, int Event, int param )
{
	std::string AssociatedTriggerName;
	RA_Tiggers  *AssociatedTrigger;
	//int			value = 0;

	// Check if there are someone behind this trigger :)
    if (UnitOrStructure == NULL){
    	logger->warning("%s line %i: No structure defined ! \n", __FILE__, __LINE__);
        return;
    }
    

    // Get the trigger name from the unit or structure
    AssociatedTriggerName = UnitOrStructure->getTriggerName();

    if (AssociatedTriggerName == "None")
        return;

    //
    // Find the associated trigger in the map
    //
    AssociatedTrigger = p::ccmap->getTriggerByName (AssociatedTriggerName);
    if (AssociatedTrigger == NULL){
        printf ("Trigger not found %s\n", AssociatedTrigger->name.c_str());
        return;
    }


	if (AssociatedTrigger->trigger1.event != Event && AssociatedTrigger->trigger2.event != Event)
		return;

#if 0
    int countrynr = p::ppool->getHouseNumByPlayerNum(UnitOrStructure->getOwner());

    // Check if the trigger was meant for us
    if (countrynr != AssociatedTrigger->country)
        return;
#endif

//    printf ("%s line %i: Trigger name = %s, unitorstructurename = %c%c%c\n", __FILE__, __LINE__, AssociatedTriggerName.c_str(), UnitOrStructure->getType()->getTName()[0], UnitOrStructure->getType()->getTName()[1], UnitOrStructure->getType()->getTName()[2]);

    switch (AssociatedTrigger->activate){
        // Only trigger event 1 must be true
        case 0:
            if (AssociatedTrigger->trigger1.event == Event){
                ExecuteTriggerAction (Event, 1, AssociatedTrigger );
                if (AssociatedTrigger->actions == 1){
                    ExecuteTriggerAction (Event, 2, AssociatedTrigger );
                }
            }
            break;
        // Triggger event one and two must be true
        case 1:
			//printf ("%s line %i: TODO: ********************trigger event1 and trigger event2 must be true check: T1 = %i, T2 = %i*******************\n", __FILE__, __LINE__, AssociatedTrigger->trigger1.event, AssociatedTrigger->trigger2.event);
			if (AssociatedTrigger->trigger1.event == Event){
				// Trigger 1 is already oke, check trigger 2
				if (CheckSecondTriggerEvent (2, AssociatedTrigger)){
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

            //printf ("%s line %i: TODO: ********************trigger event1 and trigger event2 must be true check: T1 = %i, T2 = %i, param = %i *******************\n", __FILE__, __LINE__, AssociatedTrigger->trigger1.event, AssociatedTrigger->trigger2.event, value);
            break;
        // Either the first or the second trigger event must be true (activate all associated action on trigger1 or trigger2)
        case 2:
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
            }
            break;
        // Either the first or the second trigger event must be true (activate action 1 for trigger1, activate action2 for trigger2)
        case 3:
            if (AssociatedTrigger->trigger1.event == Event){
                ExecuteTriggerAction (Event, 1, AssociatedTrigger );
            }
            if (AssociatedTrigger->trigger2.event == Event){
                ExecuteTriggerAction (Event, 2, AssociatedTrigger );
            }

            break;
        default:
            printf ("%s line %i: Invalid trigger activation\n", __FILE__, __LINE__);
            break;
    }
#if 0
    PrintTrigger ( *AssociatedTrigger );
#endif

    // Tirgger the any tirgger event
    if (Event != TRIGGER_EVENT_ANY_EVENT ){
        //printf ("%s line %i: Trigger any event trigger\n", __FILE__, __LINE__);
        HandleTriggers ( UnitOrStructure, TRIGGER_EVENT_ANY_EVENT );
    }
}

/** @Handle global triggers
    * @param Event the event that was caused for this unit or structure
    * @param param the parameter that goes with the event (example time eleapsed)
    * @returns void
**/
void HandleGlobalTrigger ( int Event, int value )
{
	RA_Tiggers  *Trigger;
	int         TriggNumb = 0;

//	if (Event == TRIGGER_EVENT_TIME_ELAPSED)
//		printf ("%s line %i: event = %i, param = %i\n", __FILE__, __LINE__, Event, value);

	if (Event == TRIGGER_EVENT_ZONE_ENTRY)
		printf ("%s line %i:  ***TRIGGER_ZONE_ENTRY***, cellpos = %i\n", __FILE__, __LINE__, value);

	while ((Trigger = p::ccmap->getTriggerByNumb ( TriggNumb )) != NULL){

		TriggNumb++;

		if (Trigger == NULL)
			return;

		if (Trigger->trigger1.event != Event && Trigger->trigger2.event != Event)
			continue;



		switch (Trigger->activate){
			// Only trigger event 1 must be true
			case 0:
				if (Trigger->trigger1.event == Event){
					if (!CheckParameters (Event, Trigger->trigger1.param1, Trigger->trigger1.param2, value))
						continue;
					ExecuteTriggerAction ( Event, 1, Trigger );
					if (Trigger->actions == 1){
						ExecuteTriggerAction ( Event, 2, Trigger );
					}
				}
				break;
			// Triggger event one and two must be true
			case 1:
				//printf ("%s line %i: T1 = %i, T2 = %i, T1->P2 = %i, T2->P2 = %i, param = %i\n", __FILE__, __LINE__, Trigger->trigger1.event, Trigger->trigger2.event, Trigger->trigger1.param2, Trigger->trigger2.param2, value);
				if (Trigger->trigger1.event == Event && !CheckParameters (Event, Trigger->trigger1.param1, Trigger->trigger1.param2, value)){
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
				//printf ("%s line %i: TODO: ********************trigger event1 and trigger event2 must be true check: T1 = %i, T2 = %i, param = %i *******************\n", __FILE__, __LINE__, Trigger->trigger1.event, Trigger->trigger2.event, value);
				break;
			// Either the first or the second trigger event must be true (activate all associated action on trigger1 or trigger2)
			case 2:
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
				}
				break;
			// Either the first or the second trigger event must be true (activate action 1 for trigger1, activate action2 for trigger2)
			case 3:
				if (Trigger->trigger1.event == Event){
					if (Trigger->trigger1.event == TRIGGER_EVENT_TIME_ELAPSED && !CheckParameters (Event, Trigger->trigger1.param1, Trigger->trigger1.param2, value))
						continue;
					ExecuteTriggerAction ( Event, 1, Trigger );
				}
				if (Trigger->trigger2.event == Event){
					if (Trigger->trigger2.event == TRIGGER_EVENT_TIME_ELAPSED && Trigger->trigger2.param2 < value)
						continue;
					ExecuteTriggerAction ( Event, 2, Trigger );
				}
				break;
			default:
				printf ("%s line %i: Invalid trigger activation\n", __FILE__, __LINE__);
				break;
		}
#if 0
        PrintTrigger ( *Trigger );
#endif

	}
	// Trigger the any trigger event
	if (Event != TRIGGER_EVENT_ANY_EVENT ){
		HandleGlobalTrigger ( TRIGGER_EVENT_ANY_EVENT, 0 );
	}

}


void CheckCellTriggers ( Uint32 pos )
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
    unit = p::uspool->getGroundUnitAt ( pos );

	if (unit == NULL)
    	unit = p::uspool->getFlyingAt ( pos );

	if (unit == NULL)
		return;



    if (unit->getTriggerName() == "None")
    {
        return;
    }

    Trigger = p::ccmap->getTriggerByName (unit->getTriggerName().c_str());

    if (Trigger == NULL)
        return;

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

#if 0
    PrintTrigger ( *Trigger );
#endif

}


void ExecuteTriggerAction (unsigned int Event, Uint8 ActionNr, RA_Tiggers *Trigger )
{
    unsigned int    Action;
    int             parameter;
    RA_Tiggers      *Trig;
    Structure       *str;
    std::string     TriggerName;
    int             TeamNr;
    RA_Teamtype     *Team;
    Uint32 Waypoint = 0; // For reveal around waypoint/zone

	// Check if the trigger should be executed (if it is not repeatable and 
	// has already exectued once it should not exectute again)
	if (!Trigger->repeatable && Trigger->hasexecuted){
		return;
	}

	// Set the has executed var now (to prevent inf loops)
	Trigger->hasexecuted = true;
	p::ccmap->setTriggerByName (Trigger->name.c_str(), Trigger);


	if (ActionNr == 1){
		Action = Trigger->action1.Action;
	}else if (ActionNr == 2){
		Action = Trigger->action2.Action;
	}else
		return;

    switch (Action){
        case TRIGGER_ACTION_NO_ACTION:
            break;
        case TRIGGER_ACTION_WINNER_IS:
            logger->error ("%s line %i: ***TRIGGER_ACTION_WINNER_IS***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_LOSER_IS:
            logger->error ("%s line %i: ***TRIGGER_ACTION_LOSER_IS***\n", __FILE__, __LINE__);
            break;
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
			if (ActionNr == 1)
				TeamNr = Trigger->action1.param1;
			else if (ActionNr == 2)
				TeamNr = Trigger->action2.param1;
			else
				return;
			Team = p::ccmap->getTeamtypeByNumb ( TeamNr );
			if (Team != NULL){
				logger->warning ("Reinforcement team = %s\n", Team->tname.c_str());
				p::uspool->createReinforcements(Team);
			}
			// Play the reinforcements have arrived tune
			pc::sfxeng->PlaySound(pc::Config.Reinforcements);
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
            logger->error ("%s line %i: ***TRIGGER_ACTION_TEXT***\n", __FILE__, __LINE__);
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
		if (ActionNr == 1){
			Waypoint = Trigger->action1.param3;
		}else if (ActionNr == 2){
			Waypoint = Trigger->action1.param3;
		}
		p::ppool->getLPlayer()->revealAroundWaypoint(Waypoint);
		logger->error ("%s line %i: ***TRIGGER_ACTION_REVEAL_AROUND_WAYPOINT***, waypoint1 == %u\n", __FILE__, __LINE__, Waypoint);
            break;
	case TRIGGER_ACTION_REVEAL_ZONE_OF_WAYPOINT:
		/// This is incorrect this funtion should reveal a complete zone, two possibilities
		/// 1. Everything around the waypoint where one would be able to walk / drive
		/// 2. If the waypoint is over a place where one can't walk/drive reveal all places on the map where one can't walk/drive
		if (ActionNr == 1){
			Waypoint = Trigger->action1.param3;
		}else if (ActionNr == 2){
			Waypoint = Trigger->action1.param3;
		}
		p::ppool->getLPlayer()->revealAroundWaypoint(Waypoint);
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
            if (ActionNr == 1)
                parameter = Trigger->action1.param2;
            else
                parameter = Trigger->action2.param2;

            Trig =  p::ccmap->getTriggerByNumb ( parameter );

            if (Trig->action1.Action != TRIGGER_ACTION_FORCE_TRIGGER)
                ExecuteTriggerAction ( Event, 1, Trig );
            if (Trig->action2.Action != TRIGGER_ACTION_FORCE_TRIGGER && Trig->actions == 1){
                ExecuteTriggerAction ( Event, 2, Trig );
            }
            logger->warning ("%s line %i: Force trigger %s\n", __FILE__, __LINE__, Trig->name.c_str());
            break;
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
            if (ActionNr == 1){
                GlobalVar[Trigger->action1.param3] = true;
                logger->error ("%s line %i: Set global = %i *************************************\n", __FILE__, __LINE__, Trigger->action1.param3);
            }else if (ActionNr == 2){
                GlobalVar[Trigger->action2.param3] = true;
                logger->error ("%s line %i: Set global = %i *************************************\n", __FILE__, __LINE__, Trigger->action2.param3);
            }
            break;
        case TRIGGER_ACTION_GLOBAL_CLEAR:
            if (ActionNr == 1){
                GlobalVar[Trigger->action1.param3] = false;
                logger->error ("%s line %i: Clear global = %i *************************************\n", __FILE__, __LINE__, Trigger->action1.param3);
            }else if (ActionNr == 2){
                GlobalVar[Trigger->action2.param3] = false;
                logger->error ("%s line %i: Clear global = %i *************************************\n", __FILE__, __LINE__, Trigger->action2.param3);
            }
            break;
        case TRIGGER_ACTION_AUTO_BASE_BUILDING:
            logger->error ("%s line %i: ***TRIGGER_ACTION_AUTO_BASE_BUILDING***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_GROW_SHROUD_ONE_STEP:
            logger->error ("%s line %i: ***TRIGGER_ACTION_GROW_SHROUD_ONE_STEP***\n", __FILE__, __LINE__);
            break;
        case TRIGGER_ACTION_DESTROY_BUILDING:
        	//printf ("Handle action destroy building, event = %i\n", Event);
        	//break;
        	//PrintTrigger ( *Trigger );
			for (unsigned int i = 0; i < p::uspool->getNumbStructures(); i++){
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
					/// TODO DANGEROUS HACK
					/// this is a dangerous hack
					/// possibility off recursive function calls,
					/// possibility off infinite loops
					str->applyDamage(255, NULL, NULL);
				}
			}
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

void PrintTrigger ( RA_Tiggers Trigger )
{
	printf ("%s line %i: Read trigger:\n", __FILE__, __LINE__);
	printf ("name = \t\t\t%s\n", Trigger.name.c_str());	
	printf ("repeatable = \t\t%i\n", Trigger.repeatable);
    printf ("country = \t\t%i\n", Trigger.country);
    printf ("activate = \t\t%i\n", Trigger.activate);
    printf ("actions = \t\t%i\n", Trigger.actions);
    printf ("trigger1.event = \t%i\n", Trigger.trigger1.event);
    printf ("trigger1.param1 = \t%i\n", Trigger.trigger1.param1);
    printf ("trigger1.param2 = \t%i\n", Trigger.trigger1.param2);
    printf ("trigger2.event = \t%i\n", Trigger.trigger2.event);
    printf ("trigger2.param1 = \t%i\n", Trigger.trigger2.param1);
    printf ("trigger2.param2 = \t%i\n", Trigger.trigger2.param2);
    printf ("action1.Action = \t%i\n", Trigger.action1.Action);
    printf ("action1.param1 = \t%i\n", Trigger.action1.param1);
    printf ("action1.param2 = \t%i\n", Trigger.action1.param2);
    printf ("action1.param3 = \t%i\n", Trigger.action1.param3);
    printf ("action2.Action = \t%i\n", Trigger.action2.Action);
    printf ("action2.param1 = \t%i\n", Trigger.action2.param1);
    printf ("action2.param2 = \t%i\n", Trigger.action2.param2);
    printf ("action2.param3 = \t%i\n", Trigger.action2.param3);
    printf ("\n\n\n");
}


void InitializeTriggers (void)
{
    for (int i = 0; i < 100; i++){
        GlobalVar[i] = false;
    }
}

