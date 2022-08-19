#ifndef _RAYC_VIDEO_COLOR_H_
#define _RAYC_VIDEO_COLOR_H_ 1

#include <SDL2/SDL.h>

namespace rayc {

const SDL_Color RGB_WHITE = {255, 255, 255};
const SDL_Color RGB_BLACK = {  0,   0,   0};
const SDL_Color RGB_RED   = {255,   0,   0};
const SDL_Color RGB_GREEN = {0,   255,   0};
const SDL_Color RGB_BLUE  = {0,     0, 255};

} /* namespace rayc */

#endif /* _RAYC_VIDEO_COLOR_H_ */