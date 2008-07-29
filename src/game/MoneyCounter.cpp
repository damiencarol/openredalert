// MoneyCounter.cpp
// 1.4

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

// MoneyCounter.cpp
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

#include "MoneyCounter.h"

#include <cassert>

#include "SDL/SDL_types.h"

#include "audio/SoundEngine.h"
#include "include/config.h"
#include "include/common.h"
#include "include/Logger.h"
#include "Player.h"
#include "PlayerPool.h"
#include "ActionEventQueue.h"

namespace pc {
	extern SoundEngine * sfxeng;
	extern ConfigType Config;
}
namespace p {
	extern ActionEventQueue* aequeue;
}

MoneyCounter::MoneyCounter(Sint32* money, Player* player, MoneyCounter** backref)
    : ActionEvent(1), money(*money), player(player), queued(false), creditleft(0), debtleft(0), creditsound(-1), debitsound(-1), sound(true), backref(backref)
{
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
    if (Dcred > 0) {
        money += Dcred;
    } else if (sound && creditsound != -1) {
        pc::sfxeng->StopLoopedSound(creditsound);
        creditsound = -1;
    }
    if (Ddebt > 0) {
        if (!player->hasInfMoney()) {
            assert(money - Ddebt >= 0);
        }
        money -= Ddebt;
    } else if (sound && debitsound != -1) {
        pc::sfxeng->StopLoopedSound(debitsound);
        debitsound = -1;
    }

    if (Dcred > 0 || Ddebt > 0) {
        p::aequeue->scheduleEvent(this);
        queued = true;
    }
}

void MoneyCounter::addCredit(Uint16 amount, Uint8 PlayerNumb)
{
    creditleft += amount;
    if ((sound && -1 == creditsound) && PlayerNumb == p::ppool->getLPlayerNum()) {
        creditsound = pc::sfxeng->PlayLoopedSound(pc::Config.MoneyCountUp,0);
    }
    reshedule();
}

void MoneyCounter::addDebt(Uint16 amount, Uint8 PlayerNumb)
{
    debtleft += amount;
    if ((sound && -1 == debitsound) && PlayerNumb == p::ppool->getLPlayerNum()) {
        debitsound = pc::sfxeng->PlayLoopedSound(pc::Config.MoneyCountDown,0);
    }
    reshedule();
}

Uint8 MoneyCounter::step(Uint16& value)
{
    if (value == 0){
        return 0;
    }

    if (value < delta) {
        Uint8 oldvalue = static_cast<Uint8>(value);
        value = 0;
        return oldvalue;
    } else {
        value -= delta;
        return delta;
    }
}

void MoneyCounter::reshedule() {
    if (!queued) {
        queued = true;
        p::aequeue->scheduleEvent(this);
    }
}
