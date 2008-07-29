// MoneyCounter.h
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

#ifndef MONEYCOUNTER_H
#define MONEYCOUNTER_H

#include "SDL/SDL_types.h"

#include "ActionEvent.h"

class Player;

/**
 * 
 */
class MoneyCounter : public ActionEvent {
public:
    MoneyCounter(Sint32* money, Player* player, MoneyCounter * * backref);
    ~MoneyCounter();
    void run();

    Uint16 getDebt() const { return debtleft; }

    void addCredit(Uint16 amount, Uint8 PlayerNumb);
    void addDebt(Uint16 amount, Uint8 PlayerNumb);

    bool isScheduled() const { return queued; }

private:
    static const Uint8 delta = 5;

    Sint32& money;
    Player * player;
    bool queued;
    // Seperate because we want both credit and debit sounds being played
    Uint16 creditleft;
    Uint16 debtleft;
    Sint32 creditsound;
    Sint32 debitsound;

    Uint8 step(Uint16 & value);
    bool sound;

    MoneyCounter * * backref;

    void reshedule();
};

#endif //MONEYCOUNTER_H
