#include "MoneyCounter.h"
#include <cassert>
#include "ActionEventQueue.h"
#include "include/Logger.h"
#include "include/PlayerPool.h"
#include "audio/SoundEngine.h"
#include "include/config.h"
#include "Player.h"
namespace p
{
extern ActionEventQueue * aequeue;
}
namespace pc
{
extern SoundEngine * sfxeng;
extern ConfigType Config;
}

MoneyCounter::MoneyCounter(Sint32* money, Player* player, MoneyCounter** backref) :
	ActionEvent(1)
{
	money = money;
	player = player;
	queued = false;
	creditleft = 0;
	debtleft = 0;
	creditsound = -1;
	debitsound = -1;
	sound = true;
	backref = backref;
}

MoneyCounter::~MoneyCounter()
{
	*backref = 0;
}

void MoneyCounter::run()
{
	queued = false;
	Uint8 Dcred = step(creditleft);
	Uint8 Ddebt = step(debtleft);
	/*if (Dcred > 0) {
	 money += Dcred;
	 } else if (sound && creditsound != -1) {
	 pc::sfxeng->StopLoopedSound(creditsound);
	 creditsound = -1;
	 }
	 if (Ddebt > 0) {
	 money -= Ddebt;
	 } else if (sound && debitsound != -1) {
	 pc::sfxeng->StopLoopedSound(debitsound);
	 debitsound = -1;
	 }*/
	// TODO TRY THIS !!!!!!!!!!!!!    
	(*money) += Dcred;
	(*money) -= Ddebt;
	// increase
	if (creditleft > debtleft)
	{
		if (sound && debitsound != -1)
		{
			pc::sfxeng->StopLoopedSound(debitsound);
			debitsound = -1;
		}
		if ((sound && -1 == creditsound) && this->player->getPlayerNum() == p::ppool->getLPlayerNum())
		{
			creditsound = pc::sfxeng->PlayLoopedSound(pc::Config.MoneyCountUp,
					0);
		}
	}
	else if (creditleft < debtleft)
	{
		if (sound && creditsound != -1)
		{
			pc::sfxeng->StopLoopedSound(creditsound);
			creditsound = -1;
		}
		if ((sound && -1 == debitsound) && this->player->getPlayerNum() == p::ppool->getLPlayerNum())
		{
			debitsound = pc::sfxeng->PlayLoopedSound(pc::Config.MoneyCountDown,
					0);
		}
	}

	if (Dcred > 0 || Ddebt > 0)
	{
		p::aequeue->scheduleEvent(this);
		queued = true;
	}
}

void MoneyCounter::addCredit(Uint16 amount, Uint8 PlayerNumb)
{
	creditleft += amount;
	/*if ((sound && -1 == creditsound) && PlayerNumb == p::ppool->getLPlayerNum()) {
	 creditsound = pc::sfxeng->PlayLoopedSound(pc::Config.MoneyCountUp,0);
	 }*/
	reshedule();
}

void MoneyCounter::addDebt(Uint16 amount, Uint8 PlayerNumb)
{
	debtleft += amount;
	/*if ((sound && -1 == debitsound) && PlayerNumb == p::ppool->getLPlayerNum()) {
	 debitsound = pc::sfxeng->PlayLoopedSound(pc::Config.MoneyCountDown,0);
	 }*/
	reshedule();
}

Uint8 MoneyCounter::step(Uint16& value)
{
	if (value == 0)
	{
		return 0;
	}

	if (value < delta)
	{
		Uint8 oldvalue = static_cast<Uint8>(value);
		value = 0;
		return oldvalue;
	}
	else
	{
		value -= delta;
		return delta;
	}
}

void MoneyCounter::reshedule()
{
	if (!queued)
	{
		queued = true;
		p::aequeue->scheduleEvent(this);
	}
}
