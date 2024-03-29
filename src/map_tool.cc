#include <rayc/map.h>
#include <rayc/strutils.h>
#include <rayc/log.h>

int main(int argc, char ** argv) {
  if (argc < 2) {
    rayc::error("Usage: %s MAP_FILE [UPD_MAP_FILE]", argv[0]);
    return 1;
  }

  // auto map = rayc::Map::load(argv[1]);

  // map.textures[0] = "notexture.png";

  // for (auto& sprite : map.sprites) {
  //   sprite = rayc::pathGetFile(sprite);
  // }

  // map.printInfo();

  auto map = rayc::Map(10, 10);
  map.tiles = {
    {0,1,1}, {0,1,1}, {0,1,1}, {0,1,1}, {0,1,1}, {0,1,1}, {0,1,1}, {0,1,1}, {0,1,1}, {0,1,1},
    {0,1,1}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,1,1}, {0,0,0}, {0,0,0}, {0,1,1},
    {0,1,1}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,1,1}, {0,0,0}, {0,0,0}, {0,1,1},
    {0,1,1}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,1,1}, {0,0,0}, {0,0,0}, {0,1,1},
    {0,1,1}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,1,1}, {0,0,0}, {0,0,0}, {0,1,1},
    {0,1,1}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {2,2,1}, {0,0,0}, {0,0,0}, {0,1,1},
    {0,1,1}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,1,1}, {0,0,0}, {0,0,0}, {0,1,1},
    {0,1,1}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,1,1}, {0,0,0}, {0,0,0}, {0,1,1},
    {0,1,1}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,1,1}, {0,0,0}, {0,0,0}, {0,1,1},
    {0,1,1}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,1,1}, {0,0,0}, {0,0,0}, {0,1,1},
    {0,1,1}, {0,1,1}, {0,1,1}, {0,1,1}, {0,1,1}, {0,1,1}, {0,1,1}, {0,1,1}, {0,1,1}, {0,1,1},
  };

  map.name = "door_test";
  map.startX = 2;
  map.startY = 5;

  map.textures.push_back("notexture.png");
  map.textures.push_back("wolf3d/WALL0.bmp");
  map.textures.push_back("wolf3d/WALL98.bmp");

  if (argc == 3) {
    map.save(std::string(argv[2]));
  }

  return 0;
}
