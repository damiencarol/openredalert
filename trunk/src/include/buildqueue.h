#ifndef BUILDQUEUE_H
#define BUILDQUEUE_H

#include <list>
#include <map>

#include "SDL/SDL_types.h"

#include "game/ConStatus.h"

class UnitOrStructure;
class UnitOrStructureType;
class Player;

namespace BuildQueue {

class BQTimer;

typedef std::map<const UnitOrStructureType*, Uint8> Production;
typedef std::list<const UnitOrStructureType*> Queue;

class BQueue
{
public:
    BQueue(Player* p);
    ~BQueue();

    bool Add(const UnitOrStructureType* type);
    ConStatus PauseCancel(const UnitOrStructureType* type);

	ConStatus getStatus(void) const;
    ConStatus getStatus(const UnitOrStructureType* type, Uint8* quantity, Uint8* progress) const;

    void Placed();

	void Pause (void);
	void Resume (void);


    const UnitOrStructureType* getCurrentType() const ;
private:
    void next();
    enum RQstate {RQ_DONE, RQ_NEW, RQ_MAXED};
    RQstate requeue(const UnitOrStructureType* type);

    Player *player;
    Uint32 last, left;
    ConStatus status;

    BQTimer* timer;
    bool tick();
    void rescheduled();

    Production production;
    Queue queue;
};

}

#endif //BUILDQUEUE_H
