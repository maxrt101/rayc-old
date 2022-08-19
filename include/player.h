#ifndef _RAYC_PLAYER_H_
#define _RAYC_PLAYER_H_ 1

#include <cmath>
#include <rayc/math/vec2.h>

namespace rayc {

struct Player {
  Vec2d position;
  float angle = 0.0f;
};

} /* namespace rayc */

#endif /* _RAYC_PLAYER_H_ */