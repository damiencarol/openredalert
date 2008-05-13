#ifndef BQUEUE_H
#define BQUEUE_H

#include <list>
#include <map>

#include "SDL/SDL_types.h"

#include "ConStatus.h"
#include "Production.h"
#include "Queue.h"
#include "RQstate.h"

class Player;
class UnitOrStructureType;
class BQTimer;

using std::list;
using std::map;


class BQueue
{
public:
    BQueue(Player* pPlayer);
    ~BQueue();

    bool Add(const UnitOrStructureType* type);
    ConStatus PauseCancel(const UnitOrStructureType* type);

	ConStatus getStatus() const;
    ConStatus getStatus(const UnitOrStructureType* type, Uint8* quantity, Uint8* progress) const;

    void Placed();

	void Pause();
	void Resume();
	bool tick();
    
    const UnitOrStructureType* getCurrentType() const ;
private:
    void next();
    void rescheduled();

    RQstate requeue(const UnitOrStructureType* type);

    Player* player;
    Uint32 last; Uint32 left;
    ConStatus status;

    BQTimer* timer;
    
    Production production;
    Queue queue;

    /**
     * TODO These values should be moved to a configuration file. I don't think buildspeed needs to be changed once this code is stable. 
     */
    static const Uint8 buildspeed;
    static const Uint8 maxbuild;
};

#endif //BQUEUE_H
