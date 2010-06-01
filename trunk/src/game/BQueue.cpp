// BQueue.cpp
//
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

#include "BQueue.h"

#include <sstream>
#include <stdexcept>
#include <string>
#include <algorithm> // for find()

#include "Logger.hpp"
#include "audio/SoundEngine.h"
#include "Dispatcher.h"
#include "PlayerPool.h"
#include "UnitAndStructurePool.h"
#include "ActionEvent.h"
#include "UnitOrStructureType.h"
#include "ActionEventQueue.h"
#include "ConStatus.h"
#include "Player.h"
#include "BQTimer.h"
#include "RQstate.h"
#include "misc/config.h"
#include "CnCMap.h"

namespace pc {
	extern Sound::SoundEngine* sfxeng;
	extern ConfigType Config;
}
namespace p {
	extern Dispatcher* dispatcher;
	extern ActionEventQueue* aequeue;
	extern CnCMap* ccmap;
}

using std::min;
using std::stringstream;

/**
 * 
 */
BQueue::BQueue(Player* pPlayer) : 
    player(pPlayer), 
    status(BQ_EMPTY) 
{
    timer = new BQTimer(this, &timer);
}

BQueue::~BQueue() 
{
    if (timer != 0) {
        timer->destroy();
    }
    timer = 0;
}

bool BQueue::Add(const UnitOrStructureType * type) 
{
    switch (status) 
    {
        case BQ_INVALID:
            {
                stringstream message;
                message << "Queue '" << this << "' is  in invalid state";
                Logger::getInstance()->Error(__FILE__ , __LINE__, message.str());
            }
            break;
        case BQ_EMPTY:        
            if (0 == (left = type->getCost()))
            {
                stringstream message;
                message << "Type '" << type->getName() << "' has no cost";
                Logger::getInstance()->Error(__FILE__ , __LINE__, message.str());
            }
            last = p::aequeue->getCurtick();
            production.insert(Production::value_type(type, 1));
            queue.push_back(type);
            status = BQ_RUNNING;
            timer->Reshedule();
            return true;
            break;
        case BQ_PAUSED:
        case BQ_ALL_PAUSED:
            if (getCurrentType() == type) {
                status = BQ_RUNNING;
                last = p::aequeue->getCurtick();
                timer->Reshedule();
            }
            return true;
            break;
        case BQ_READY:
            // Can't enqueue, waiting for placement
            if (!type->isStructure()) {
                if (p::dispatcher->unitSpawn((UnitType *) type, player->getPlayerNum())) {
                    Placed();
                } 
                else 
                {
                    MACRO_LOG_DEBUG("Didn't spawn " + string(type->getName()))
                }
            }
            return false;
            break;
        case BQ_RUNNING:
            // First try to requeue another of the type
            switch (requeue(type)) {
                case RQ_DONE:
                    return true;
                case RQ_MAXED:
                    return false;
                case RQ_NEW:
                    // This type is new to the queue
                    if (0 == type->getCost()) {
                        // We divide by cost, so must not be zero.
                        stringstream message;
                        message << "Type '" << type->getName() << "' has no cost";
                        Logger::getInstance()->Error(__FILE__ , __LINE__, message.str());
                        return false;
                    }
                    production.insert(Production::value_type(type, 1));
                    queue.push_back(type);
                    timer->Reshedule();
                    return true;
            }
            return false;
            break;
        default:
            {
                stringstream message;
                message << "Queue '" << this << "' in /really/ invalid state " << status;
                Logger::getInstance()->Error(__FILE__ , __LINE__, message.str());
            }
            break;
    }
    return false;
}

ConStatus BQueue::PauseCancel(const UnitOrStructureType * type) 
{
    if (BQ_EMPTY == status) {
        return BQ_EMPTY;
    }
    // We search the map first because it should be faster.
    Production::iterator mit = production.find(type);
    if (production.end() == mit) {
        // Not queued this type at all
        // UI decision: do we behave as if we had clicked on the type currently
        // being built?
        return BQ_EMPTY;
    }
    Queue::iterator lit = find(queue.begin(), queue.end(), type);

    if (queue.begin() == lit) {
        switch (status) {
            case BQ_RUNNING:
                status = BQ_PAUSED;
                p::ccmap->getPlayerPool()->updateSidebar();
                return status;
                break;
            case BQ_READY:
                // Fallthrough
            case BQ_PAUSED:
            case BQ_ALL_PAUSED:
                if (mit->second > 1) {
                    --mit->second;
                } else {
                    // Refund what we've spent so far
                    player->changeMoney(mit->first->getCost() - left);
                    // Remain in paused state
                    next();
                }
                break;
            default:
                std::string msg("Unhandled state in pausecancel: ");
                msg += status;
                throw std::runtime_error(msg);
                break;
        }
    } else {
        if (mit->second > 1) {
            --mit->second;
        } else {
            queue.erase(lit);
            production.erase(mit);
        }
    }

    p::ccmap->getPlayerPool()->updateSidebar();
    return BQ_CANCELLED;
}

ConStatus BQueue::getStatus() const 
{
    return status;
}

ConStatus BQueue::getStatus(const UnitOrStructureType * type, Uint8 * quantity, Uint8 * progress) const
{
    * quantity = 0;
    * progress = 100; // Default to not grey'd out.
    if (BQ_EMPTY == status || BQ_INVALID == status) {
        // Fast exit for states with nothing to report
        return status;
    }
    Production::const_iterator it = production.find(type);
    * progress = 0;
    if (it == production.end()) {
        return status;
    }
    * quantity = it->second;
    if (getCurrentType() == type) {
        * progress = 100 * (it->first->getCost() - left) / getCurrentType()->getCost();
    }
    return status;
}

/**
 * Used for other bits of code to notify the buildqueue that they can continue
 * building having placed what was waiting for a position
 */
void BQueue::Placed() 
{
    p::ccmap->getPlayerPool()->updateSidebar();
    status = BQ_RUNNING;
    next();
}

void BQueue::Pause() 
{
    if (status == BQ_RUNNING) {
        status = BQ_ALL_PAUSED;
        p::ccmap->getPlayerPool()->updateSidebar();
    }
}

void BQueue::Resume() 
{
    if (status == BQ_ALL_PAUSED) {
        status = BQ_RUNNING;
        last = p::aequeue->getCurtick();
        timer->Reshedule();

    }
}

const UnitOrStructureType * BQueue::getCurrentType() const 
{
    return * queue.begin();
}

void BQueue::next() 
{
    // Check that queue is not empty
    if (queue.empty())
    {
        Logger::getInstance()->Error(__FILE__ , __LINE__, "[BQueue::next()] queue is empty !");
        return;
    }
    
    Production::iterator it = production.find(getCurrentType());
    
    // Check that it is not at the end
    if (it == production.end())
    {
        Logger::getInstance()->Error(__FILE__ , __LINE__, "[BQueue::next()] iterator is at the end !");
        return;
    }
    
    if (it->second <= 1) {
        production.erase(it);
        queue.pop_front();
        if (queue.empty()) {
            status = BQ_EMPTY;
            return;
        } else {
            // None left of the current type of thing being built, so move onto
            // the next item in the queue and start building
            status = BQ_RUNNING;
            it = production.find(getCurrentType());
            
            // Check that it is not at the end
            if (it == production.end())
            {
                Logger::getInstance()->Error("[BQueue::next()] iterator is at the end !");
                return;
            }
        }
    } else {
        --it->second;
    }
    left = it->first->getCost();
    last = p::aequeue->getCurtick();
    timer->Reshedule();
    p::ccmap->getPlayerPool()->updateSidebar();
}

RQstate BQueue::requeue(const UnitOrStructureType * type) 
{
    Production::iterator it = production.find(type);
    if (it == production.end()) {
        return RQ_NEW;
    }
    if (it->second > maxbuild) {
        return RQ_MAXED;
    }
    it->second++;
    return RQ_DONE;
}

/**
 * Each tick of the timer 
 * 
 * - calculate a delta
 * - decrease the money
 * ????- reschedule ????
 * ++ ???
 */
bool BQueue::tick() 
{
    if (status != BQ_RUNNING) {
        return false;
    }
    Uint8 delta = min((p::aequeue->getCurtick() - last) / buildspeed, left);

    if (delta == 0) {
        return false;
    }
    last += delta * buildspeed;
    /// @todo Play "tink" sound
    if (!player->changeMoney(-delta)) {
        /// @todo Play "insufficient funds" sound
        // Note: C&C didn't put build on hold when you initially run out, so you
        // could leave something partially built whilst waiting for the
        // harvester to return

        // Reschedule to keep checking for money
        return true;
    }
    left -= delta;

    if (0 != left) {
        p::ccmap->getPlayerPool()->updateSidebar();
        return true;
    }
    const UnitOrStructureType * type = getCurrentType();
    // For structures and blocked unit production, we wait for user
    // interaction.
    status = BQ_READY;
    if (!type->isStructure()) {
        UnitType * utype = (UnitType *) type;
        if (p::dispatcher->unitSpawn(utype, player->getPlayerNum())) {
            /// @todo Play "unit ready" sound
            // If we were able to spawn the unit, move onto the next
            // item
            pc::sfxeng->PlaySound(pc::Config.UnitReady);
            status = BQ_RUNNING;
            next();
        }
    } else {
        // Play "construction complete" sound
        pc::sfxeng->PlaySound(pc::Config.StructureReady);
    }
    p::ccmap->getPlayerPool()->updateSidebar();
    return false;
}

const Uint8 BQueue::buildspeed = 1;
const Uint8 BQueue::maxbuild = 99;
