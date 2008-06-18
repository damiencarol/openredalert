#include "BarrelExplosionActionEvent.h"

#include "ExplosionAnim.h"
#include "video/ImageCache.h"
#include "ActionEventQueue.h"
#include "CncMap.h"
#include "UnitAndStructurePool.h"
#include "Structure.h"

namespace p
{
extern ActionEventQueue* aequeue;
extern CnCMap* ccmap;
extern UnitAndStructurePool* uspool;

}
namespace pc
{
extern ImageCache* imgcache;
}

/**
 * @param p  of the anim
 */
BarrelExplosionActionEvent::BarrelExplosionActionEvent(Uint32 p, Uint32 pos) :
	ActionEvent(p)
{
	// Set a delay (0.512 sec)
	setDelay(6);

	// Save the position
	position = pos;

	// Reschedule this anim
	p::aequeue->scheduleEvent(this);
}

/**
 */
BarrelExplosionActionEvent::~BarrelExplosionActionEvent()
{
	// Do nothing
}

/**
 * @todo Add sound code
 * @todo change 3 in delay of ExploAnim
 */
void BarrelExplosionActionEvent::run()
{
	// Play the flame sound #1
	//pc::sfxeng->PlaySound("firebl3.aud");

	// Play the large fire misc anim
	Uint32 numImageNapalmBarrel = pc::imgcache->loadImage("napalm3.shp");
	// 3 = delay 14 = num image in ^    -36 = height/2 in ^  -36 = width/2 in ^
	new ExplosionAnim(3, position, numImageNapalmBarrel, 14, -36, -36);

	
	
	
	//
	// APPLY DAMAGE FOR STRUCTURE
	//
	Structure* str = 0;
	// to the left
	str = p::uspool->getStructureAt(position - 1, false);
	if (str != 0)
	{
		str->applyDamage(77, 0, 0);
	}
	// to the right
	str = p::uspool->getStructureAt(position + 1, false);
	if (str != 0)
	{
		str->applyDamage(77, 0, 0);
	}
	// to the up
	str = p::uspool->getStructureAt(position - p::ccmap->getWidth(), false);
	if (str != 0)
	{
		str->applyDamage(77, 0, 0);
	}
	// to the down
	str = p::uspool->getStructureAt(position + p::ccmap->getWidth(), false);
	if (str != 0)
	{
		str->applyDamage(77, 0, 0);
	}

	
	
	
	//
	// APPLY DAMAGE FOR GROUND UNIT
	//
	Unit* unitTarget = 0;	
	// to the left
	unitTarget = p::uspool->getGroundUnitAt(position - 1, 0x80);
	if (unitTarget != 0)
	{
		unitTarget->applyDamage(77, 0, 0);
	}
	// to the right
	unitTarget = p::uspool->getGroundUnitAt(position + 1, 0x80);
	if (unitTarget != 0)
	{
		unitTarget->applyDamage(77, 0, 0);
	}
	// to the up
	unitTarget = p::uspool->getGroundUnitAt(position - p::ccmap->getWidth(), 0x80);
	if (unitTarget != 0)
	{
		unitTarget->applyDamage(77, 0, 0);
	}
	// to the down
	unitTarget = p::uspool->getGroundUnitAt(position + p::ccmap->getWidth(), 0x80);
	if (unitTarget != 0)
	{
		unitTarget->applyDamage(77, 0, 0);
	}

	
	
	
	

	// After launched anim destroye this anim
	delete this;
}
