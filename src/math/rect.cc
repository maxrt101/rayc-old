#include <rayc/math/rect.h>

rayc::Rect::Rect()
  : x(0), y(0), w(0), h(0) {}

rayc::Rect::Rect(int x, int y, int w, int h)
  : x(x), y(y), w(w), h(h) {}

rayc::Rect::Rect(int w, int h)
  : x(0), y(0), w(w), h(h) {}

bool rayc::Rect::isUnit() const {
  return w == 0 && h == 0;
}

SDL_Rect rayc::Rect::toSdlRect() const {
  SDL_Rect rect;
  rect.x = x;
  rect.y = y;
  rect.w = w;
  rect.h = h;
  return rect;
}
