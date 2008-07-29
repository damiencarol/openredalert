// UHarvestEvent.cpp
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

#include "UHarvestEvent.h"

#include <cmath>

#include "CnCMap.h"
#include "UnitOrStructure.h"
#include "Structure.h"
#include "unittypes.h"
#include "UnitAndStructurePool.h"
#include "TurnAnimEvent.h"
#include "MoveAnimEvent.h"
#include "ActionEventQueue.h"
#include "Player.h"
#include "PlayerPool.h"
#include "include/Logger.h"
#include "Unit.h"

namespace p {
	extern ActionEventQueue * aequeue;
	extern CnCMap* ccmap;
	extern PlayerPool* ppool;
	extern UnitAndStructurePool* uspool;
}
extern Logger * logger;

UHarvestEvent::UHarvestEvent(Uint32 p, Unit *un) : UnitAnimEvent(p,un)
{
	ForceEmpty			= false;
	this->un			= un;
	stopping			= false;
	index				= 0;
	delay				= 0;
	facing				= 0;
	MoveTargePos		= un->FindTiberium ();
	manual_pauze		= false;
	NumbResources		= un->GetNumResources ();
	new_orgimage		= false;
	OrgImage			= un->getImageNum(0)&0xff;
	RetryMoveCounter	= 0;
	ReturnStep			= 1;

	// Get the resources whe harvested previously from the unit
	for (int i = 0; i < NumbResources; i++)
	{
		un->GetResourceType (i, &ResourceTypes[i]);
	}
}

/**
 */
UHarvestEvent::~UHarvestEvent()
{
	logger->debug("UHarvest destructor\n");

	if (this->un->harvestanim == this)
		this->un->harvestanim = NULL;
}

/**
 * 
 */
void UHarvestEvent::stop()
{
    if (un == NULL) {
#ifdef DEBUG_HARVEST_ANIM
        printf("UHarvestEvent::stop: un is NULL!?\n");
#endif
        abort();
    }
    stopping = true;
}

/**
 * 
 */
void UHarvestEvent::setHarvestingPos(Uint32 pos)
{
	MoveTargePos = pos;

	if ( p::uspool->getStructureAt(pos) != NULL){
 		if (p::uspool->getStructureAt(pos)->isRefinery ()){
			manual_pauze = false;
			if (NumbResources > 0){
				ReturnStep	= 1;
				ForceEmpty	= true;
			}
			return;
		}
	}

	// Pauze harvesting if the player sends us to a place where there is no resource
	if (p::ccmap->getResourceFrame(pos) == 0){
		manual_pauze = true;
		return;
	}

	manual_pauze = false;
}

/**
 */
void UHarvestEvent::update()
{
    stopping = false;
}

/**
 */
void UHarvestEvent::run()
{
#ifdef DEBUG_HARVEST_ANIM
	if ( un->getOwner() == p::ppool->getLPlayerNum() ){
		printf ("%s line %i: Run harvest animation\n", __FILE__, __LINE__);
	}
#endif

	if( !un->isAlive() || stopping ) {
#ifdef DEBUG_HARVEST_ANIM
		if ( un->getOwner() == p::ppool->getLPlayerNum() ){
			printf ("%s line %i: Stopping harvest animation\n", __FILE__, __LINE__);
		}
#endif
		delete this;
		return;
	}

	if (RetryMoveCounter > 5){
		stop();
		setDelay(14);
		p::aequeue->scheduleEvent(this);
		return;
	}


	if (un->GetBaseRefinery() != NULL){
		if (!un->GetBaseRefinery()->isAlive())
			un->SetBaseRefinery(NULL);
	}

	if (manual_pauze){
#ifdef DEBUG_HARVEST_ANIM
		if ( un->getOwner() == p::ppool->getLPlayerNum() )
			printf ("%s line %i: Manual pauze harvest animation\n", __FILE__, __LINE__);
#endif
		un->setImageNum(OrgImage, 0);
		new_orgimage = true;
       	if (un->getPos() == MoveTargePos){
			stop();
			setDelay(14);
			p::aequeue->scheduleEvent(this);
			return;
		}
		un->move(MoveTargePos, false);
		if (RetryMoveCounter == 0){
			un->moveanim->setSchedule(this);
		}else{
			setDelay(14);
			p::aequeue->scheduleEvent(this);
		}
		RetryMoveCounter++;
		return;
	}

	if (new_orgimage){
		OrgImage = un->getImageNum(0)&0xff;
		OrgImage -= OrgImage%4;
		if (OrgImage > 32)
			OrgImage = 8;
		//facing = (Uint32) OrgImage;
		new_orgimage = false;
		//printf ("New org image\n");
	}

	if (NumbResources < 5 && !ForceEmpty/*&& !un->EmptyHarvester*/){
#ifdef DEBUG_HARVEST_ANIM
		if ( un->getOwner() == p::ppool->getLPlayerNum() )
			printf ("%s line %i: Harvest\n", __FILE__, __LINE__);
#endif
		if (un->getPos() != MoveTargePos){
			// If there is something occupying our position move to a diffirent pos...
//			if (p::uspool->getUnitOrStructureAt(MoveTargePos) != NULL && un->getPos() != MoveTargePos || RetryMoveCounter > 2){
			if (p::uspool->cellOccupied(MoveTargePos) && un->getPos() != MoveTargePos || RetryMoveCounter > 2){
				MoveTargePos = un->FindTiberium ();
			}
			un->setImageNum(OrgImage, 0);
			un->move(MoveTargePos, false);
			new_orgimage = true;
			if (RetryMoveCounter == 0){
				un->moveanim->setSchedule(this);
			}else{
				setDelay(14);
				p::aequeue->scheduleEvent(this);
			}
			RetryMoveCounter++;
			return;
		}
		RetryMoveCounter = 0;

        if (p::ccmap->getResourceFrame(un->getPos()) == 0){
            MoveTargePos = un->FindTiberium ();
        }

        int NumbLayers = un->type->getNumLayers();

        facing = (Uint32)(((double)OrgImage / (double)32) * (double)8);

        if (index < 8){
            un->setImageNum(32+(facing*8) + index, NumbLayers - 1);
            index++;
        }else{
            //un->setImageNum(OrgImage, NumbLayers - 1);

            // Start new annimation
            index = 0;

            Uint8 type, amount;

            // Remove the mined tiberium from the map
            if (p::ccmap->getResource(un->getPos(), &type, &amount)) {
                p::ccmap->decreaseResource(un->getPos(), 1);
                ResourceTypes[NumbResources] = type;
                un->AddResource (ResourceTypes[NumbResources]);
                //if (un->getOwner() == p::ppool->getLPlayerNum())
                    //printf ("LPlayer, add resource type %i to harvester\n", type);
                NumbResources++;
                ReturnStep = 1;
            }
        }
    }else if (un->GetBaseRefinery() != NULL){
#ifdef DEBUG_HARVEST_ANIM
		if ( un->getOwner() == p::ppool->getLPlayerNum() )
			printf ("%s line %i: Empty, step = %i \n", __FILE__, __LINE__, ReturnStep);
#endif
		// We are full --> move back to our base and dump our content there
		//int MoveToPos = this->GetBaseRefineryPos ();
		if (ReturnStep == 1){
			if (this->GetBaseRefineryPos () != un->getPos()){
				un->setImageNum(OrgImage, 0);
		        un->move(this->GetBaseRefineryPos (), false);
				new_orgimage = true;
				if (RetryMoveCounter == 0){
					un->moveanim->setSchedule(this);
				}else{
					setDelay(14);
					p::aequeue->scheduleEvent(this);
				}
				RetryMoveCounter++;
				return;
			}else{
				RetryMoveCounter = 0;
				ReturnStep = 2;
			}

		}else if (ReturnStep == 2){ //un->getImageNum(0)&0xff != 8){
                	un->turn(8,0);
                	un->turnanim1->setSchedule(this);
			new_orgimage = true;
			ReturnStep = 3;
			return;
		}else if (ReturnStep == 3){
			// We are back at the refinery, start dump animation
			int NumbLayers = un->type->getNumLayers();

			if (index < 8){
				un->setImageNum(32+(8*8) + index, NumbLayers - 1);
				index++;
			}else{

				un->setImageNum(OrgImage, NumbLayers - 1);

//			if (index > 8){
				// Start new annimation
				index = 0;

				int Value = 0;
				for (int i = 0; i < NumbResources; i++)
					Value += (10 - ResourceTypes[i])*50;

				NumbResources = 0;

				un->EmptyResources ();

				ReturnStep = 1;

				ForceEmpty = false;

				p::ppool->getPlayer(un->getOwner())->changeMoney(Value);

				if (un->getOwner() != p::ppool->getLPlayerNum())
					MoveTargePos = un->FindTiberium ();
			}
		}else{
			// Hmm, we should never get here --> make returnstep 1
			ReturnStep = 1;
		}
	}
	// Reschedule this..
	setDelay(10);
	p::aequeue->scheduleEvent(this);

}

/**
 */
int UHarvestEvent::GetBaseRefineryPos (void)
{
	Uint16 x, y, xdiv;
	int temppos;

	if (un->GetBaseRefinery() == NULL)
		return 0;

	xdiv = 1;

	p::ccmap->translateFromPos(un->GetBaseRefinery()->getPos(), &x, &y);
	y += 2;
	temppos = p::ccmap->translateToPos(x+xdiv, y);

//	while ( !p::ccmap->isBuildableAt(un->getOwner(), temppos) && y < p::ccmap->getHeight() && un->getPos() != temppos ){
	while ( p::ccmap->getCost(temppos, un) > 4 && y < p::ccmap->getHeight() && un->getPos() != temppos ){
		if ( xdiv < 10 && x + xdiv < p::ccmap->getWidth())
			xdiv++;
		else{
			y += 1;
			xdiv = 0;
		}
		//printf ("xdiv is %i\n", xdiv);
		temppos = p::ccmap->translateToPos(x+xdiv, y);
	}

//	printf ("UNITANIMATIONS x = %i, y = %i, temppos = %i\n", x, y, temppos);

	return temppos;
}
