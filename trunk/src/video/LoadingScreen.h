#ifndef LOADINGSCREEN_H
#define LOADINGSCREEN_H

#include <string>

#include "SDL/SDL_thread.h"

class GraphicsEngine;
class CPSImage;

class LoadingScreen {
public:
    LoadingScreen();
    ~LoadingScreen();
    void setCurrentTask(const std::string& task);
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

#endif /* LOADINGSCREEN_H */
