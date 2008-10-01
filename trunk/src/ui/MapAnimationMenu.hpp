// MapAnimationMenu.hpp
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

#ifndef UI_MAPANIMATIONMENU_HPP
#define	UI_MAPANIMATIONMENU_HPP

class GraphicsEngine;

namespace UI
{

/**
 * Class wich provide easy way to show select mission animation
 * 
 * @author Damien Carol (OpenRedAlert)
 */
class MapAnimationMenu
{
public:
    /** Play the animation for a mission */
    void Play(GraphicsEngine& theGraphicEngine, const unsigned int mapNumber, const bool isAlly) const;
};

}

#endif	/* UI_MAPANIMATIONMENU_HPP */
