#ifndef MONEYCOUNTER_H
#define MONEYCOUNTER_H

#include "SDL/SDL_types.h"

#include "ActionEvent.h"
#include "Player.h"

class MoneyCounter : public ActionEvent {
public:
    MoneyCounter(Sint32 *, Player *, MoneyCounter * *);
    ~MoneyCounter();
    void run();

    Uint16 getDebt() const { return debtleft; }

    void addCredit(Uint16 amount, Uint8 PlayerNumb);
    void addDebt(Uint16 amount, Uint8 PlayerNumb);

    bool isScheduled() const { return queued; }

private:
    static const Uint8 delta = 5;

    Sint32 & money;
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
