// LoadingScreen.h
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

#ifndef LOADINGSCREEN_H
#define LOADINGSCREEN_H

#include <string>

#include "SDL/SDL_thread.h"

class GraphicsEngine;
class CPSImage;

using std::string;

class LoadingScreen
{
public:
    LoadingScreen();
    ~LoadingScreen();
    void setCurrentTask(const string& task);
    const std::string& getCurrentTask() const ;
private:
    // Non-copyable
    LoadingScreen(const LoadingScreen&) ;
    LoadingScreen& operator=(const LoadingScreen&) ;

    static int runRenderThread(void *inst);
    SDL_Thread *renderThread;
    SDL_mutex *lsmutex;
    CPSImage* logo;
    bool done;
    std::string task_;
    Uint32 oldwidth;
};

#endif //LOADINGSCREEN_H
