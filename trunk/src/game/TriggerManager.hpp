// TriggerManager.hpp
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

#ifndef TRIGGERMANAGER_H
#define	TRIGGERMANAGER_H

class CnCMap;

namespace OpenRedAlert
{
namespace Game
{

/**
 * Object that manage triggers activation
 * <p>
 * This object analyse trigger, test if they must acts and make action in
 * the map. This object is responsible of destruction of the triggers.
 *
 * @author Damien Carol (OpenRedAlert)
 * @version 1.0
 * @since r???
 */
class TriggerManager
{
public:
    TriggerManager(CnCMap* theMap);
    virtual ~TriggerManager();

    void handle();

private:
    TriggerManager();
    TriggerManager(const TriggerManager& orig);
    CnCMap* cncMap;
};

}

}

#endif	//TRIGGERMANAGER_HPP

