// ImageProc.cpp
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

#include <cstdlib>
#include <cstring>
#include "video/Renderer.h"
#include "include/imageproc.h"
#include "include/Logger.h"

/** Constructor, empty
 *
 */
ImageProc::ImageProc()
{}



/** Destructor, empty */
ImageProc::~ImageProc()
{}

void ImageProc::scaleInterlace( SDL_Surface *src, SDL_Surface *dest)
{
    Uint16 cx, *xpos;
    Uint32 linestart, curpos, curposlinestart, pos, limit;
    float xmod = (float)(src->w-1)/(float)(dest->w-1);
    float ymod = 2.0f*(float)(src->h-1)/(float)(dest->h-1);
    float cxf, cyf;
    Uint8 *srcp, *destp;
    destp = (Uint8*)dest->pixels;
    srcp = (Uint8*)src->pixels;
    xpos = new Uint16[dest->w];
    cxf = 0;
    for( cx = 0; cx < dest->w; cx++ ) {
        xpos[cx] = (Uint16)(cxf+0.5f)*src->format->BytesPerPixel;
        cxf += xmod;
    }
    curpos = 0;
    curposlinestart = 0;
    cyf = 0;
    cx = 0;
    linestart = 0;
    limit = dest->w*(dest->h>>1);
    for( pos = 0; pos < limit; pos++, cx++ ) {
        if( cx == dest->w ) {
            cyf += ymod;
            linestart = src->pitch*((Uint16)(cyf+0.5f));
            curposlinestart += (dest->pitch<<1);
            curpos = curposlinestart;
            cx = 0;
        }
        memcpy(destp+curpos, srcp+linestart+
               xpos[cx], src->format->BytesPerPixel);
        curpos += src->format->BytesPerPixel;
    }
    delete[] xpos;
}

void ImageProc::scaleNearest( SDL_Surface *src, SDL_Surface *dest)
{
    Uint16 cx, *xpos;
    Uint32 linestart, curpos, curposlinestart, limit, pos;
    float cxf, cyf;
    Uint8 *srcp, *destp;

    float xmod = (float)(src->w-1)/(float)(dest->w-1);
    float ymod = (float)(src->h-1)/(float)(dest->h-1);

    SDL_LockSurface(src);
    SDL_LockSurface(dest);

    destp = (Uint8*)dest->pixels;
    srcp = (Uint8*)src->pixels;

    xpos = new Uint16[dest->w];

    cxf = 0;
    for( cx = 0; cx < dest->w; cx++ ) {
        xpos[cx] = (Uint16)(cxf+0.5f)*src->format->BytesPerPixel;
        cxf += xmod;
    }

    curpos = 0;
    curposlinestart = 0;
    cyf = 0;
    cx = 0;
    linestart = 0;
    limit = dest->w*dest->h;
    /// @todo The inner comparison succeeds about 0.15% of the time.
    for( pos = 0; pos < limit; cx++, pos++ ) {
        if( cx >= dest->w ) {
            cyf += ymod;
            linestart = src->pitch*((Uint16)(cyf+0.5f));
            curposlinestart += dest->pitch;
            curpos = curposlinestart;
            cx = 0;
        }

//	if (linestart+xpos[cx] < (unsigned) src->w*src->h*src->format->BytesPerPixel && curpos < (unsigned) dest->w*dest->h*dest->format->BytesPerPixel)
        	memcpy(destp+curpos, srcp+linestart+xpos[cx], src->format->BytesPerPixel);
//	else
//		printf ("HMMM, here too?\n");

        curpos += src->format->BytesPerPixel;
    }

    SDL_UnlockSurface(src);
    SDL_UnlockSurface(dest);
    delete[] xpos;
}



void ImageProc::scaleLinear( SDL_Surface *src, SDL_Surface *dest)
{
    /* FIXME: there is not much right in this function, use the 8bppscr
    version as a base and write a new one */
    Uint16 cx, cy, xbase, ybase;
    float xpos, ypos;
    float xa1, xa2, ya1, ya2;
    Uint8 byte, top, bot;
    Uint32 linestart, curpos, lineoffs, curposlinestart;
    float xscale = (float)(src->w-1)/(float)(dest->w-1);
    float yscale = (float)(src->h-1)/(float)(dest->h-1);
    Uint8 *srcp, *destp;
    destp = (Uint8*)dest->pixels;
    srcp = (Uint8*)src->pixels;
    curpos = 0;
    curposlinestart = 0;
    for( cy = 0; cy < dest->h; cy++ ) {
        ypos = (float)cy*yscale;
        ybase = (Uint16)ypos;
        ya2 = ypos-(float)ybase;
        ya1 = 1-ya2;
        linestart = ybase*src->pitch;
        curpos = curposlinestart;
        curposlinestart += dest->pitch;
        for( cx = 0; cx < dest->w; cx++ ) {
            xpos = (float)cx*xscale;
            xbase = (Uint16)xpos;
            xa2 = xpos-(float)xbase;
            xa1 = 1-xa2;
            lineoffs = xbase*src->format->BytesPerPixel;
            for( byte = 0; byte < src->format->BytesPerPixel; byte++ ) {
                top = (Uint8)(xa1*srcp[linestart+lineoffs] + xa2*srcp[linestart+lineoffs+1]);
                bot = (Uint8)(xa1*srcp[linestart+src->pitch+lineoffs] + xa2*srcp[linestart+src->pitch+lineoffs+1]);
                destp[curpos] = (Uint8)(ya1*top+ya2*bot);
                curpos++;
            }
        }
    }
}

void ImageProc::scaleLinear8bppsrc(SDL_Surface *src, SDL_Surface *dest)
{
    Uint16 cx, *xbase, ybase;
    float xa1, *xa2, ya1, ya2;
    Uint32 color, pos, limit;
    SDL_Color col1, col2;
    Uint32 linestart, curpos, lineoffs, curposlinestart;
    float xmod = (float)(src->w-1)/(float)(dest->w-1);
    float ymod = (float)(src->h-1)/(float)(dest->h-1);
    float cxf, cyf;
    Uint8 *srcp, *destp;
    SDL_Color *top, *bottom, *changetmp;
    int topline, bottomline;
    destp = (Uint8*)dest->pixels;
    srcp = (Uint8*)src->pixels;
    xbase = new Uint16[dest->w];
    top = new SDL_Color[dest->w];
    bottom = new SDL_Color[dest->w];
    xa2 = new float[dest->w];

    cxf = 0;
    for( cx = 0; cx < dest->w; cx++ ) {
        xbase[cx] = (Uint16)cxf;
        xa2[cx] = cxf-(float)xbase[cx];
        cxf += xmod;
    }
    topline = -1;
    bottomline = -1;
    ybase = 0;
    linestart = 0;
    curpos = 0;
    curposlinestart = 0;
    ya2 = 0;
    ya1 = 1;
    cyf = 0;
    cx = 0;
    limit = dest->w*dest->h;
    for( pos = 0; pos < limit; pos++, cx++ ) {
        if( cx == dest->w ) {
            topline = ybase;
            if( ya2 >= 0.01 )
                bottomline = ybase+1;

            cyf += ymod;
            ybase = (Uint16)cyf;
            ya2 = cyf-(float)ybase;
            ya1 = 1-ya2;
            linestart = ybase*src->pitch;
            curposlinestart += dest->pitch;
            curpos = curposlinestart;
            if( bottomline == ybase ) {
                changetmp = top;
                top = bottom;
                bottom = changetmp;
                topline = ybase;
                bottomline = -1;
            }
            cx = 0;
        }
        xa1 = 1-xa2[cx];
        lineoffs = xbase[cx];

        if( topline != ybase ) {
            if( xa2[cx] >= 0.01 ) {
                col1 = src->format->palette->colors[srcp[linestart+lineoffs]];
                col2 = src->format->palette->colors[srcp[linestart+lineoffs+1]];
                top[cx].r = (Uint8)(xa1*col1.r+xa2[cx]*col2.r);
                top[cx].g = (Uint8)(xa1*col1.g+xa2[cx]*col2.g);
                top[cx].b = (Uint8)(xa1*col1.b+xa2[cx]*col2.b);
            } else {
                top[cx] = src->format->palette->colors[srcp[linestart+lineoffs]];
            }
        }
        if( bottomline != ybase+1 && ya2 >= 0.01 ) {
            if( xa2[cx] >= 0.01 ) {
                col1 = src->format->palette->colors[srcp[linestart+src->pitch+lineoffs]];
                col2 = src->format->palette->colors[srcp[linestart+src->pitch+lineoffs+1]];
                bottom[cx].r = (Uint8)(xa1*col1.r+xa2[cx]*col2.r);
                bottom[cx].g = (Uint8)(xa1*col1.g+xa2[cx]*col2.g);
                bottom[cx].b = (Uint8)(xa1*col1.b+xa2[cx]*col2.b);
            } else {
                bottom[cx] = src->format->palette->colors[srcp[linestart+src->pitch+lineoffs]];

            }
        }
        if( xa2[cx] >= 0.01 ) {
            col1.r = (Uint8)(ya1*top[cx].r+ya2*bottom[cx].r);
            col1.g = (Uint8)(ya1*top[cx].g+ya2*bottom[cx].g);
            col1.b = (Uint8)(ya1*top[cx].b+ya2*bottom[cx].b);
        } else {
            col1 = top[cx];
        }
        color = SDL_MapRGB(dest->format, col1.r, col1.g, col1.b),
                //color <<= (8*(4-dest->format->BytesPerPixel));
                memcpy(destp+curpos, &color,
                       dest->format->BytesPerPixel);

        curpos+=dest->format->BytesPerPixel;

    }
    delete[] top;
    delete[] bottom;
    delete[] xbase;
    delete[] xa2;

}

/** 
 * Scale the incoming image
 * 
 * @param the image to process
 * @param quality. Selects the interpolation method.  Currently unused.
 */

SDL_Surface* ImageProc::scale(SDL_Surface* input, Sint8 quality)
{
    SDL_Surface *output;
    Uint8 bytesPerPixel;
    
    // get the bytePerPixel value 
    bytesPerPixel = input->format->BytesPerPixel;

    output = SDL_CreateRGBSurface (SDL_SWSURFACE,(input->w<<1),(input->h<<1),
                                   bytesPerPixel << 3, 0xff, 0xff, 0xff, 0);
    output->format->Rmask = input->format->Rmask;
    output->format->Gmask = input->format->Gmask;
    output->format->Bmask = input->format->Bmask;
    output->format->Amask = input->format->Amask;
    if( input->format->palette != NULL ) {
        SDL_SetColors(output, input->format->palette->colors, 0, input->format->palette->ncolors);
    }

    //check the quality and call different functions
    scaleNearest(input, output);

    return output;
}

SDL_Surface* ImageProc::scale(SDL_Surface* input, Sint32 Width, Sint32 Heigth)
{
    SDL_Surface *output;
    Uint8 bytesPerPixel = input->format->BytesPerPixel;

    output = SDL_CreateRGBSurface (SDL_SWSURFACE,Width,Heigth, bytesPerPixel << 3, 0, 0, 0, 0);
    output->format->Rmask = input->format->Rmask;
    output->format->Gmask = input->format->Gmask;
    output->format->Bmask = input->format->Bmask;
    output->format->Amask = input->format->Amask;
    if( input->format->palette != NULL ) {
        SDL_SetColors(output, input->format->palette->colors, 0, input->format->palette->ncolors);
    }

    //check the quality and call different functions
    scaleNearest(input, output);

    return output;
}



/** Scales down the map to a much smaller image which will be placed in the radar screen as the "minimap". Needs only to be called once per map load
 * @param the image to process
 */
SDL_Surface* ImageProc::minimapScale(SDL_Surface *input, Uint8 pixsize)
{
    SDL_Surface *output;
    int bytesPerPixel = input->format->BytesPerPixel;
    output = SDL_CreateRGBSurface (SDL_SWSURFACE, pixsize, pixsize, bytesPerPixel *8, 0xff, 0xff, 0xff, 0);
    output->format->Rmask = input->format->Rmask;
    output->format->Gmask = input->format->Gmask;
    output->format->Bmask = input->format->Bmask;
    output->format->Amask = input->format->Amask;
    if( input->format->palette != NULL ) {
        SDL_SetColors(output, input->format->palette->colors, 0, input->format->palette->ncolors);
    }
    scaleNearest(input, output);

    return output;
}

void ImageProc::closeVideoScale()
{
    SDL_FreeSurface(videoOutputBuffer);
    videoOutputBuffer = NULL;
}

void ImageProc::initVideoScale(SDL_Surface* input, int videoq)
{
    //Make a new video buffer with the apropriate size and bpp (bpp==8)
    //Delete the old one if it exsists
    SDL_FreeSurface(videoOutputBuffer);
    this->videoq = videoq;
    if( videoq == 1 ) {
        videoOutputBuffer = SDL_CreateRGBSurface (SDL_SWSURFACE,
                            SDL_GetVideoSurface()->w,SDL_GetVideoSurface()->w*input->h/input->w,
                            16, 0xf800, 0x7c0, 0x3e, 1);
        //32, 0xff000000, 0xff0000, 0xff00, 0xff);

    } else {
        videoOutputBuffer = SDL_CreateRGBSurface (SDL_SWSURFACE,
                            SDL_GetVideoSurface()->w,
                            SDL_GetVideoSurface()->w*input->h/input->w,
                            8, 0xff, 0xff, 0xff, 0);
        videoOutputBuffer->format->Rmask = input->format->Rmask;
        videoOutputBuffer->format->Gmask = input->format->Gmask;
        videoOutputBuffer->format->Bmask = input->format->Bmask;
        videoOutputBuffer->format->Amask = input->format->Amask;
        if( input->format->palette != NULL ) {
            SDL_SetColors(videoOutputBuffer, input->format->palette->colors,
                          0, input->format->palette->ncolors);
        }
    }
}

SDL_Surface* ImageProc::scaleVideo(SDL_Surface *input)
{
    if( videoOutputBuffer->format->BytesPerPixel == 1 ) {
        SDL_SetColors(videoOutputBuffer, input->format->palette->colors,
                      0, input->format->palette->ncolors);
    }
    switch(videoq) {
    case -1:
        scaleInterlace(input, videoOutputBuffer);
        break;
    case 1:
        scaleLinear8bppsrc(input, videoOutputBuffer);
        break;
    case 0:
    default:
        scaleNearest(input, videoOutputBuffer);
        break;
    }

    return videoOutputBuffer;
}

