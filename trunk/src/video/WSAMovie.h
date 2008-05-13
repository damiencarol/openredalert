#ifndef WSAMOVIE_H
#define WSAMOVIE_H

#include <string>

#include "SDL/SDL_types.h"
#include "SDL/SDL_video.h"

#include "WSAHeader.h"
#include "GraphicsEngine.h"


using std::string;

class WSAMovie{
public:

    WSAMovie(string fname);
    ~WSAMovie();
    void animate(GraphicsEngine* grafEngine);

private:
    SDL_Surface *decodeFrame(Uint16 framenum);
    Uint8 *wsadata;
    Uint8 *framedata;
    /** Palette of the video (for frame) */
    SDL_Color palette[256];
    /** Whether WSA loops or not */
    Uint8 loop;
    char *sndfile;
    /** Header of the video */
    WSAHeader header;
};

#endif //WSAMOVIE_H
