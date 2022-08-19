#ifndef _RAYC_VIDEO_RECT_H_
#define _RAYC_VIDEO_RECT_H_ 1

#include <SDL2/SDL.h>

namespace rayc {

struct Rect {
  int x, y, w, h;

  Rect();
  Rect(int x, int y, int w, int h);
  Rect(int w, int h);

  bool isUnit() const;
  SDL_Rect toSdlRect() const;
};

} /* namespace rayc */

#endif /* _RAYC_VIDEO_RECT_H_ */