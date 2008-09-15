// sdllayer.cpp
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

#include "include/sdllayer.h"

#include "include/Logger.h"

//#define _15BIT(r,g,b) (((r&248)<<7) + ((g&248)<<2) + (b>>3))
//#define _16BIT(r,g,b) (((r&248)<<8) + ((g&252)<<3) + (b>>3))

extern Logger * logger;

/**
 * Get a pixel from a SDL surface
 *
 * @param Surface the sdl surface to get the pixel from.
 * @param color the color gotten from the sdl surface
 * @param x x-pos in the picture to get the pixel color form
 * @param y y-pos in the picture to get the pixel color form
 */
void SDLLayer::get_pixel ( SDL_Surface* Surface, Uint32 &color, Uint32 x, Uint32 y )
{
	//Uint32	color = 0;
	Uint8	*ubuff8;
	Uint16	*ubuff16;
	Uint32	*ubuff32;

	color = 0;

	// How we draw the pixel depends on the bitdepth
	switch(Surface->format->BytesPerPixel)
	{
		case 1:
			ubuff8 = (Uint8*) Surface->pixels;
			ubuff8 += (y * Surface->pitch) + x;
			//*ubuff8 = (Uint8) color;
			color = *ubuff8;
			break;

		case 2:
			ubuff8 = (Uint8*) Surface->pixels;
			ubuff8 += (y * Surface->pitch) + (x*2);
			ubuff16 = (Uint16*) ubuff8;
			//*ubuff16 = (Uint16) color;
			color = *ubuff16;
			break;

		case 3:
			ubuff8 = (Uint8*) Surface->pixels;
			ubuff8 += (y * Surface->pitch) + (x*3);
			color = 0;
			#if SDL_BYTEORDER == SDL_LIL_ENDIAN
				color |= ubuff8[2] << 16;
				color |= ubuff8[1] << 8;
				color |= ubuff8[0];
			#else
				color |= ubuff8[0] << 16;
				color |= ubuff8[1] << 8;
				color |= ubuff8[2];
			#endif
			break;

		case 4:
			ubuff8 = (Uint8*) Surface->pixels;
			ubuff8 += (y*Surface->pitch) + (x*4);
			ubuff32 = (Uint32*)ubuff8;
			color = *ubuff32;
			break;

		default:
			logger->error("Error: Unknown color depth in surface\n");
	}
//	return color;
}

/**
 * Get a pixel from a SDL surface
 *
 * @param Surface the sdl surface to get the pixel from.
 * @param RGBcolor the color gotten from the sdl surface
 * @param x x-pos in the picture to get the pixel color form
 * @param y y-pos in the picture to get the pixel color form
 */
void SDLLayer::get_pixel ( SDL_Surface* Surface, SDL_Color &RGBcolor, Uint32 x, Uint32 y )
{
	Uint32	color = 0;

	get_pixel ( Surface, color, x, y );
	SDL_GetRGB( color, Surface->format, &RGBcolor.r, &RGBcolor.g, &RGBcolor.b );
}

/**
 * Set a pixel to a color in a sdl surface
 *
 * @param Surface the sdl surface to get the pixel from.
 * @param color the color gotten from the sdl surface (Uint32 color)
 * @param x x-pos in the picture to get the pixel color form
 * @param y y-pos in the picture to get the pixel color form
 * @returns void
 */
void SDLLayer::set_pixel ( SDL_Surface* Surface, Uint32 color, Uint32 x, Uint32 y )
{
	SDL_Rect DestRect;

	// Draw the top line
	DestRect.x = x;
	DestRect.y = y;
	DestRect.w = 1;
	DestRect.h = 1;
	SDL_FillRect (Surface, &DestRect, color);
}

/**
 * Set a pixel to a color in a sdl surface
 *
 * @param Surface the sdl surface to get the pixel from.
 * @param RGBcolor the color gotten from the sdl surface (SDL_Color color)
 * @param x x-pos in the picture to get the pixel color form
 * @param y y-pos in the picture to get the pixel color form
 */
void SDLLayer::set_pixel ( SDL_Surface* Surface, SDL_Color RGBcolor, Uint32 x, Uint32 y )
{
	Uint32 color = SDL_MapRGB( Surface->format, RGBcolor.r, RGBcolor.g, RGBcolor.b );
	set_pixel ( Surface, color, x, y );
}


/**
 * Draw a rectangle outline on a sdl surface
 *
 * @param Surface the sdl surface to draw the rectangle on.
 * @param color the color of the rectangle to draw
 * @param x x-pos of the rectangle to draw
 * @param y y-pos of the rectangle to draw
 * @param wdith the width of the rectangle to draw
 * @param height the heigth of the rectangle to draw
 * @param lnpx the line weigth of the square to draw
 */
void SDLLayer::draw_rectangle(SDL_Surface* Surface, Uint32 color, Uint16 x, Uint16 y, Uint16 width, Uint16 height, Uint8 lnpx )
{
	SDL_Rect DestRect;

	if (Surface == 0)
	{
		return;
	}

	// Draw the top line
	DestRect.x = x;
	DestRect.y = y;
	DestRect.w = width;
	DestRect.h = 1;
	SDL_FillRect (Surface, &DestRect, color);


	// Draw the bottum line
	DestRect.y = y+height-1;
	SDL_FillRect (Surface, &DestRect, color);

	// Draw the left line
	DestRect.y = y;
	DestRect.w = 1;
	DestRect.h = height;
	SDL_FillRect (Surface, &DestRect, color);

	// Draw the left line
	DestRect.x = x+width-1;
	SDL_FillRect (Surface, &DestRect, color);
}

/**
 * Draw a rectangle outline on a sdl surface
 *
 * @param Surface the sdl surface to draw the rectangle on.
 * @param RGBcolor the color of the rectangle to draw
 * @param x x-pos of the rectangle to draw
 * @param y y-pos of the rectangle to draw
 * @param wdith the width of the rectangle to draw
 * @param height the heigth of the rectangle to draw
 * @param lnpx the line weigth of the square to draw
 * @return void
 */
void SDLLayer::draw_rectangle ( SDL_Surface* Surface, SDL_Color RGBcolor, Uint16 x, Uint16 y, Uint16 width, Uint16 height, Uint8 lnpx )
{
	Uint32 color = SDL_MapRGB( Surface->format, RGBcolor.r, RGBcolor.g, RGBcolor.b );
	draw_rectangle ( Surface, color, x, y, width, height, lnpx );
}

/**
 * Draw a solid rectangle on a sdl surface
 *
 * @param Surface the sdl surface to draw the rectangle on.
 * @param color the color of the rectangle to draw
 * @param x x-pos of the rectangle to draw
 * @param y y-pos of the rectangle to draw
 * @param wdith the width of the rectangle to draw
 * @param height the heigth of the rectangle to draw
 * @return void
 */
void SDLLayer::draw_solidrectangle ( SDL_Surface* Surface, Uint32 color, Uint16 x, Uint16 y, Uint16 width, Uint16 height )
{
	SDL_Rect DestRect;

	if (Surface == 0)
		return;

	DestRect.x = x;
	DestRect.y = y;
	DestRect.w = width;
	DestRect.h = height;

	SDL_FillRect (Surface, &DestRect, color);
}

/**
 * Draw a solid rectangle on a sdl surface
 *
 * @param Surface the sdl surface to draw the rectangle on.
 * @param RGBcolor the color of the rectangle to draw
 * @param x x-pos of the rectangle to draw
 * @param y y-pos of the rectangle to draw
 * @param wdith the width of the rectangle to draw
 * @param height the heigth of the rectangle to draw
 * @return void
 */
void SDLLayer::draw_solidrectangle ( SDL_Surface* Surface, SDL_Color RGBcolor, Uint16 x, Uint16 y, Uint16 width, Uint16 height )
{
	Uint32 color = SDL_MapRGB( Surface->format, RGBcolor.r, RGBcolor.g, RGBcolor.b );
	draw_solidrectangle ( Surface, color, x, y, width, height );
}

/**
 * Draw a horizontal line on a sdl surface
 *
 * @param Surface the sdl surface to draw the rectangle on.
 * @param RGBcolor the color of the rectangle to draw
 * @param x x-pos of the rectangle to draw
 * @param y y-pos of the rectangle to draw
 * @param length the heigth of the rectangle to draw
 * @param wdith the width of the rectangle to draw
 * @return void
 */
void SDLLayer::draw_h_line ( SDL_Surface* Surface, SDL_Color RGBcolor, Uint16 x, Uint16 y, Uint16 length, Uint16 pt )
{
	draw_solidrectangle ( Surface, RGBcolor, x, y, length, pt );
}

/**
 * Draw a vertical line on a sdl surface
 *
 * @param Surface the sdl surface to draw the rectangle on.
 * @param RGBcolor the color of the rectangle to draw
 * @param x x-pos of the rectangle to draw
 * @param y y-pos of the rectangle to draw
 * @param wdith the width of the rectangle to draw
 * @param height the heigth of the rectangle to draw
 * @return void
 */
void SDLLayer::draw_v_line ( SDL_Surface* Surface, SDL_Color RGBcolor, Uint16 x, Uint16 y, Uint16 length, Uint16 pt )
{
	draw_solidrectangle ( Surface, RGBcolor, x, y, length, pt );
}

#if 0
/**
 * Draw a random straight line (doesn't have to be horizontal or vertical)
 *
 * @param startx starting x postition for the line
 * @param starty starting y postition for the line
 * @param stopx ending x postition for the line
 * @param stopy ending y postition for the line
 * @param wdith the width of the line
 * @param colour the line color
 * @return void
 */
void draw_line(Sint16 startx, Sint16 starty, Sint16 stopx, Sint16 stopy, Uint16 width, Uint32 colour)
{
    float xmod, ymod, length, xpos, ypos;
    int i, len;
    SDL_Rect dest;
    xmod = static_cast<float>(stopx-startx);
    ymod = static_cast<float>(stopy-starty);

    length = sqrt(xmod*xmod+ymod*ymod);
    xmod /= length;
    ymod /= length;

    len = static_cast<int>(length+0.5f);
    xpos = static_cast<float>(startx-(width>>1));
    ypos = static_cast<float>(starty-(width>>1));
    for(i = 0; i < len; i++) {
        dest.x = (Sint16)xpos;
        dest.y = (Sint16)ypos;
        dest.w = width;
        dest.h = width;
        SDL_FillRect(screen, &dest, colour);
        xpos += xmod;
        ypos += ymod;
    }
}

/**
 * Draw a random straight line (doesn't have to be horizontal or vertical)
 *
 * @param startx starting x postition for the line
 * @param starty starting y postition for the line
 * @param stopx ending x postition for the line
 * @param stopy ending y postition for the line
 * @param wdith the width of the line
 * @param colour the line color
 * @return void
 */
void draw_line(Sint16 startx, Sint16 starty, Sint16 stopx, Sint16 stopy, Uint16 width, SDL_Color RGBcolor)
{
	Uint32 color = SDL_MapRGB( Surface->format, RGBcolor.r, RGBcolor.g, RGBcolor.b );
	draw_line( startx, starty, stopx, stopy, width, color );
}

#endif
