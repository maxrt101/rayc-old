#ifndef _RAYC_VIDEO_DRAW_H_
#define _RAYC_VIDEO_DRAW_H_ 1

#include <rayc/math/rect.h>
#include <rayc/video/texture.h>

namespace rayc {

void setBuffer(Texture* texture);
void clearBuffer();
void renderBuffer();
void setDrawColor(int r, int g, int b, int a = 255);
void fillRect(const Rect& rect);
void copyTexture(Texture* texture, const Rect& src, const Rect& dest);

} /* namespace rayc */

#endif /* _RAYC_VIDEO_DRAW_H_ */