// MapAnimationMenu.cpp
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

#include "MapAnimationMenu.hpp"

#include <SDL/SDL.h>

#include "video/WSAMovie.h"
#include "video/GraphicsEngine.h"

namespace UI
{
    
/**
 * @param theGraphicEngine Graphic engine to use
 * @param mapNumber Number of the mission to play the animation map
 * @param isAlly True if it's an ally mission else for Soviet mission it's false
 */
void MapAnimationMenu::Play(GraphicsEngine& theGraphicEngine, const unsigned int mapNumber, const bool isAlly) const
{
    // Build begin of the WSA movie to play (always start with "ms" in ressources)
    string nameWSAmovie = "ms";
    // Add "a" if it's ally mission or "s" for soviets missions
    if (isAlly == true)
    {
        nameWSAmovie += "a";
    }
    else
    {
        nameWSAmovie += "s";
    }
    
    // Convert map number into a letter
    // @todo convert mapNumber into a letter
    nameWSAmovie += "a.wsa";
    
    // Play the WSA movie
    // @todo change that way to drive frame by frame WSA movie
    WSAMovie mapAnimationMovie(nameWSAmovie);
    mapAnimationMovie.animate(theGraphicEngine);
    
    // Wait 200 ms
    // @todo change that way to implemente mouse mouvement
    SDL_Delay(200);
}

}
