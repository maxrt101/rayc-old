#include <rayc/map.h>
#include <rayc/app.h>
#include <rayc/log.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <vector>

template <typename T> void writeBinary(std::ostream& file, T& data) {
  file.write((char*)&data, sizeof(T));
}

template <typename T> void readBinary(std::istream& file, T* data) {
  file.read((char*)data, sizeof(T));
}

bool rayc::MapTile::isSolid() const {
  return texture != 0;
}

bool rayc::MapTile::isDoor() const {
  return flags & TILE_VDOOR || flags & TILE_HDOOR;
}

bool rayc::MapTile::isVDoor() const {
  return flags & TILE_VDOOR;
}

bool rayc::MapTile::isHDoor() const {
  return flags & TILE_HDOOR;
}

rayc::Map::Map() : Map(0, 0) {}

rayc::Map::Map(int w, int h) : width(w), height(h), startPosition(0, 0) {
  for (int i = 0; i < w*h; i++) {
    tiles.push_back({0, 0, 1});
  }
}

void rayc::Map::save(const std::string& filename) {
  std::ofstream file(filename, std::ios::out | std::ios::binary);
  
  writeBinary(file, MAP_MAGIC);
  writeBinary(file, width);
  writeBinary(file, height);
  writeBinary(file, startX);
  writeBinary(file, startY);

  for (char c : name) {
    writeBinary(file, c);
  }
  char c = '\0';
  writeBinary(file, c);

  uint8_t size = textures.size();
  writeBinary(file, size);
  for (auto &t : textures) {
    for (char c : t) {
      writeBinary(file, c);
    }
    char c = '\0';
    writeBinary(file, c);
  }

  size = sprites.size();
  writeBinary(file, size);
  for (auto &texture : sprites) {
    for (char c : texture) {
      writeBinary(file, c);
    }
    char c = '\0';
    writeBinary(file, c);
  }

  size = objects.size();
  writeBinary(file, size);
  for (auto &object : objects) {
    writeBinary(file, object.x);
    writeBinary(file, object.y);
    writeBinary(file, object.sprite);
  }

  for (auto &tile : tiles) {
    writeBinary(file, tile.flags);
    writeBinary(file, tile.texture);
    writeBinary(file, tile.height);
  }

  file.close();
}

rayc::Map rayc::Map::load(const std::string& filename) {
  std::ifstream file(filename, std::ios::in | std::ios::binary);
  
  Map map;
  if (!file) {
    error("Invalid file '%s'", filename.c_str());
    map.isValid = false;
    return map;
  }

  uint16_t magic = 0;
  readBinary(file, &magic);
  if (magic != MAP_MAGIC) {
    error("Invalid file '%s'", filename.c_str());
    map.isValid = false;
    return map;
  }

  readBinary(file, &map.width);
  readBinary(file, &map.height);
  readBinary(file, &map.startX);
  readBinary(file, &map.startY);
  
  char c;
  readBinary(file, &c);
  while (c != '\0') {
    map.name.push_back(c);
    readBinary(file, &c);
  }
  
  uint8_t count = 0;
  readBinary(file, &count);
  for (int i = 0; i < count; i++) {
    std::string textureName;
    readBinary(file, &c);
    while (c != '\0') {
      textureName.push_back(c);
      readBinary(file, &c);
    }
    map.textures.push_back(textureName);
  }
  
  readBinary(file, &count);
  for (int i = 0; i < count; i++) {
    std::string spriteName;
    readBinary(file, &c);;
    while (c != '\0') {
      spriteName.push_back(c);
      readBinary(file, &c);;
    }
    map.sprites.push_back(spriteName);
  }

  readBinary(file, &count);
  for (int i = 0; i < count; i++) {
    MapObject obj;
    readBinary(file, &obj.x);
    readBinary(file, &obj.y);
    readBinary(file, &obj.sprite);
    map.objects.push_back(obj);
  }

  for (int i = 0; i < map.width*map.height; i++) {
    MapTile tile;
    readBinary(file, &tile.flags);
    readBinary(file, &tile.texture);
    readBinary(file, &tile.height);
    map.tiles.push_back(tile);
  }

  map.isValid = true;
  if (file.bad()) {
    error("Error reading file '%s'", filename.c_str());
    map.isValid = false;
  }
  file.close();
  return map;
}

void rayc::Map::printInfo() const {
  printf("map:     %s\n", name.c_str());
  printf("width:   %d\n", width);
  printf("height:  %d\n", height);
  printf("startx:  %d\n", startX);
  printf("starty:  %d\n", startY);
  printf("objects:\n");
  for (auto& object : objects) {
    printf("  (%d, %d): %d\n", object.x, object.y, object.sprite);
  }
  printf("textures:\n");
  for (int i = 0; i < textures.size(); i++) {
    printf("  %d: %s\n", i, textures[i].c_str());
  }
  printf("sprites:\n");
  for (int i = 0; i < sprites.size(); i++) {
    printf("  %d: %s\n", i, sprites[i].c_str());
  }
}

bool rayc::Map::valid() const {
  return isValid;
}

rayc::MapTile& rayc::Map::getTile(int offset) {
  return tiles[offset];
}

rayc::MapTile& rayc::Map::getTile(int x, int y) {
  return tiles[width * y + x];
}

rayc::MapTile& rayc::Map::getTile(Vec2i pos) {
  return tiles[width * pos.y + pos.x];
}
