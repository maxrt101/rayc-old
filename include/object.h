#ifndef _RAYC_OBJECT_H_
#define _RAYC_OBJECT_H_ 1

#include <rayc/math/vec2.h>
#include <rayc/video/texture.h>

namespace rayc {

enum GameObjectType {
  OBJTYPE_STATIONARY,
  OBJTYPE_PROJECTILE,
  OBJTYPE_NPC,
};

struct GameObject {
  GameObjectType type = OBJTYPE_STATIONARY;

  Vec2d position = {0, 0};
  Vec2d velocity = {0, 0};

  bool visible = true;
  bool remove = false;

  Texture* texture;

 public:
  GameObject() = default;
  GameObject(Vec2d position, Texture* texture);
  GameObject(GameObjectType type, Vec2d position, Vec2d velocity, Texture* texture);

  virtual void onCollision(GameObject* collided);
  virtual void onFrameUpdate(float frameTile);
};

} /* namespace rayc */

#endif /* _RAYC_OBJECT_H_ */