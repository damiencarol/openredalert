// LoadingScreen.cpp
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

#include "LoadingScreen.h"

#include "SDL/SDL_timer.h"
#include "SDL/SDL_events.h"

#include "include/config.h"
#include "video/CPSImage.h"
#include "video/GraphicsEngine.h"
#include "include/Logger.h"
#include "CPSImage.h"
#include "video/ImageNotFound.h"
#include "video/MessagePool.h"

using std::string;

namespace pc {
	extern GraphicsEngine * gfxeng;
	extern MessagePool* msg;
}
extern Logger * logger;


/** @todo Abstract away the dependencies on video stuff so we can use
 * LoadingScreen as part of the server's console output.
 */
LoadingScreen::LoadingScreen()
{
    done = false;
    lsmutex = SDL_CreateMutex();
    oldwidth = pc::msg->getWidth();
    try {
        logo = new CPSImage("title.cps",1);
        pc::msg->setWidth(logo->getImage()->w / 4);
    } catch (ImageNotFound&) {
        logger->error("Couldn't load startup graphic\n");
        logo = 0;
    }
//    renderThread = SDL_CreateThread(LoadingScreen::runRenderThread, this);
}

LoadingScreen::~LoadingScreen()
{
//    int stat;
    while(SDL_mutexP(lsmutex)==-1) {
        logger->warning("Couldn't lock mutex\n");
    }

    done = true;

    while(SDL_mutexV(lsmutex)==-1) {
        logger->warning("Couldn't unlock mutex\n");
    }
//    SDL_WaitThread(renderThread, &stat);
    SDL_DestroyMutex(lsmutex);
    delete logo;
    pc::msg->setWidth(oldwidth);
}

void LoadingScreen::setCurrentTask(const string& task)
{
    while(SDL_mutexP(lsmutex)==-1) {
        logger->warning("Couldn't lock mutex\n");
    }
    task_ = task;
    while(SDL_mutexV(lsmutex)==-1) {
        logger->warning("Couldn't unlock mutex\n");
    }
}
int LoadingScreen::runRenderThread(void* inst)
{
    bool isDone = false;
    SDL_Event event;
    LoadingScreen *instance = (LoadingScreen*)inst;

    while( !isDone ) {
        while(SDL_mutexP(instance->lsmutex)==-1) {
            logger->warning("Couldn't lock mutex\n");
        }

        //render the frame here
        if (instance->logo == 0) {
            pc::gfxeng->renderLoading(instance->task_, 0);
        } else {
            pc::gfxeng->renderLoading(instance->task_,
                    instance->logo->getImage());
        }
        isDone = instance->done;

        while(SDL_mutexV(instance->lsmutex)==-1) {
            logger->warning("Couldn't unlock mutex\n");
        }
        // Limit fps to ~10
        while ( SDL_PollEvent(&event) ) {}
        SDL_Delay(500);
    }
    return 0;
}

const string& LoadingScreen::getCurrentTask() const
{
	return task_;
}

LoadingScreen::LoadingScreen(const LoadingScreen&)
{
}

LoadingScreen& LoadingScreen::operator=(const LoadingScreen&)
{
	return *this;
}
