#include <rayc/object.h>

rayc::GameObject::GameObject(Vec2d position, Texture* texture)
  : position(position), texture(texture) {}

rayc::GameObject::GameObject(GameObjectType type, Vec2d position, Vec2d velocity, Texture* texture)
  : type(type), position(position), velocity(velocity), texture(texture) {}

void rayc::GameObject::onCollision(GameObject* collided) {
  remove = true;
}

void rayc::GameObject::onFrameUpdate(float frameTile) {
  if (type == OBJTYPE_PROJECTILE) {
    position += velocity * frameTile;
  }
}
