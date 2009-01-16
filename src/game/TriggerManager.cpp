// TriggerManager.cpp
//
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

#include "TriggerManager.hpp"

#include <vector>

#include "CnCMap.h"
#include "Trigger.hpp"

using std::vector;

namespace OpenRedAlert
{
namespace Game
{

/**
 * The constructor is private to avoid it
 */
TriggerManager::TriggerManager()
{
    // Nothing
}

/**
 *
 */
TriggerManager::TriggerManager(CnCMap* pCncMap)
{
    // Update the reference
    this->cncMap = pCncMap;
}

/**
 *The constructor is private to avoid it
 */
TriggerManager::~TriggerManager()
{
    // Nothing
}

void TriggerManager::handle()
{
    int numberOfTrigger = this->cncMap->getTriggerPool()->size();

    for (int i = 0; i < numberOfTrigger; i++)
    {
        // Get the Trigger
        vector<OpenRedAlert::Game::Trigger*>* theTriggerPool = this->cncMap->getTriggerPool();
        OpenRedAlert::Game::Trigger* theTrigger = (*theTriggerPool)[i];

        // Some stuff ...
        printf("Trigger in stack : %s\n", theTrigger->getName().c_str());
    }
}

} //Game

} //OpenRedAlert
