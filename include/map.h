#ifndef _RAYC_MAP_H_
#define _RAYC_MAP_H_ 1

#include <cstdint>
#include <vector>
#include <string>

#include <rayc/math/vec2.h>

namespace rayc {

const uint16_t MAP_MAGIC = 0xffab;
const uint16_t MAP_FILE_VERSION = 0x0002;

enum TileFlags {
  TILE_VDOOR = 0x1,
  TILE_HDOOR = 0x2,
};

enum DoorState {
  DOOR_CLOSED,
  DOOR_OPENING,
  DOOR_OPENED,
};

struct MapTile {
  // serializable data
  uint8_t flags;
  uint8_t texture;
  uint8_t height;

  // state
  DoorState doorState = DOOR_CLOSED;
  int openedPercent = 0;
  int framesSinceOpened = 0;

  bool isSolid() const;
  bool isDoor() const;
  bool isVDoor() const;
  bool isHDoor() const;
};

struct MapObject {
  uint16_t x;
  uint16_t y;
  uint8_t sprite;
};

/* TODO:

checksum
floor/celiling color (when floorcasting is implemented - add floorTexture and ceilingTexture to MapTile)

fog/darkness toggle

*/

class Map {
 public:
  bool isValid = false;

  uint16_t width;
  uint16_t height;
  uint16_t startX;
  uint16_t startY;

  Vec2<uint16_t> startPosition;

  std::string name;

  std::vector<MapTile> tiles;
  std::vector<MapObject> objects;
  std::vector<std::string> textures;
  std::vector<std::string> sprites;

 public:
  Map();
  Map(int w, int h);

  void save(const std::string& filename);
  static Map load(const std::string& filename);

  void printInfo() const;
  bool valid() const;

  MapTile& getTile(int offset);
  MapTile& getTile(int x, int y);
  MapTile& getTile(Vec2i pos);
};

} /* namespace rayc */

#endif /* _RAYC_MAP_H_ */