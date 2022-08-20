#include <rayc/app.h>
#include <rayc/log.h>
#include <rayc/data.h>
#include <rayc/config.h>
#include <rayc/map.h>
#include <rayc/object.h>
#include <rayc/player.h>
#include <rayc/intutils.h>
#include <rayc/strutils.h>
#include <rayc/video/draw.h>
#include <rayc/video/color.h>
#include <rayc/video/font.h>
#include <rayc/video/texture.h>

#include <memory>
#include <chrono>
#include <string>
#include <vector>
#include <list>
#include <map>

using namespace rayc;

struct Raycaster {
  struct Resources {
    std::map<std::string, Font*> fonts;
    std::vector<Texture> textures;
    std::vector<Texture> sprites;
    rayc::Texture texturePlaceholder;
    rayc::Texture textureOverlay;
    Map map;
  } res;

  enum GameState {
    GS_NOT_PLAYING,
    GS_PLAYING,
  } state = GS_NOT_PLAYING;

  Config config;

  bool mapLoaded = false;
  bool shouldRun = true;
  bool fpsCounter = true;
  bool profile = true;
  bool spriteOverlay = false;

  Player player;

  // float fov = M_PI / 6.0f; // 600x600 - 0.5
  // float fov = M_PI / 4.8f; // 800x600 - 0.7
  float fov = M_PI / 3.66f; // 1000x600 - 0.85
  // float fov = M_PI / 3.14f; // 1200x600 - 1
  float depth = 30.0f;

  float step = 0.01f;
  int textureColumnWidth = 1;

  float rotationSpeed = 3.0f;
  float movementSpeed = 7.0f;

  float* depthBuffer = nullptr;

  std::list<std::pair<int, std::unique_ptr<GameObject>>> objects;

  enum Side {
    NORTH, SOUTH, WEST, EAST, TOP, BOTTOM
  };

  struct TileHit {
    Vec2i tilePosition {0, 0};
    Vec2d hitPosition {0, 0};
    float rayLength = 0.0f;
    float sampleX = 0.0f;
    Side side = NORTH;
  };

  struct DDAResult {
    bool hitWall = false;
    bool hitDoor = false;
    
    TileHit tile;
    TileHit door; // std::vector<TileHit> doors;
  };

 public:
  void init();

  bool onFrameUpdate(float frameTime);
  void onConsoleCommand(std::string line);

  void loadMap(const std::string& path);
  void unloadMap();

 private:
  DDAResult castRay(Vec2d src, Vec2d direction);
  void render(float frameTime);
  void processInput(float frameTime);
} raycaster;


void Raycaster::init() {
  // float aspectRatio = (float)getWidth()/getHeight();
  // float divider = ((1-(aspectRatio-1)) + 2);
  // fov = M_PI / divider;

  // if (aspectRatio < 1 || aspectRatio > 2) {
  //   warning("Aspect ratio should range from 1 to 2");
  // }

  raycaster.res.texturePlaceholder = Texture(getResourcePath(RES_TEXTURE, config.getValueOrDie("texture", "default", "test.texture is required")));
  raycaster.res.textureOverlay = Texture(getResourcePath(RES_TEXTURE, config.getValueOrDie("texture", "overlay", "test.overlay is required")));

  depthBuffer = new float[getWidth()];
}

bool Raycaster::onFrameUpdate(float frameTime) {
  if (state == GS_PLAYING) {
    render(frameTime);

    if (!rayc::isTextInputEnabled()) {
      processInput(frameTime);
    }
  }

  if (fpsCounter) {
    char buffer[5] = {0};
    snprintf(buffer, 5, "%4d", int(1.0f/frameTime));
    res.fonts["main"]->draw(
      std::string(buffer),
      {getWidth() - res.fonts["main"]->getSize() * 4, 0},
      RGB_WHITE
    );
  }

  return shouldRun;
}

void Raycaster::loadMap(const std::string& name) {
  res.map = Map::load(getResourcePath(RES_MAP, name));

  if (!res.map.isValid) {
    error("Error loading map '%s'", name.c_str());
    printConsole(RGB_RED, "Failed to load map " + name);
    return;
  }

  player.position = Vec2d(res.map.startX, res.map.startY);
  
  unloadMap();

  for (auto &textureName : res.map.textures) {
    res.textures.push_back(Texture(getResourcePath(RES_TEXTURE, textureName)));
  }

  for (auto &spriteName : res.map.sprites) {
    res.sprites.push_back(Texture(getResourcePath(RES_SPRITE, spriteName)));
  }

  for (MapObject& object : res.map.objects) {
    float x = (float)object.x+0.5f;
    float y = (float)object.y+0.5f;
    objects.push_back({ 0, std::unique_ptr<GameObject>(new GameObject({x, y}, &res.sprites[object.sprite])) });
  }

  // objects.push_back({ 0, std::unique_ptr<GameObject>(new GameObject({(float)res.map.width/2 + 0.5f, (float)res.map.height/2 + 0.5f}, &res.sprites[1])) });

  info("Map '%s' loaded successfully.", res.map.name.c_str());

  state = GS_PLAYING;
}

void Raycaster::unloadMap() {
  res.textures.clear();
  res.sprites.clear();
}

void Raycaster::onConsoleCommand(std::string line) {
  auto tokens = splitstr(line);

  if (!tokens.empty()) {
    if (tokens[0] == "quit") {
      shouldRun = false;
    } else if (tokens[0] == "map") {
      if (tokens.size() != 2) {
        printConsole(RGB_RED, "Usage: map FILE");
      } else {
        loadMap(tokens[1]);
      }
    } else if (tokens[0] == "fov") {
      if (tokens.size() == 1) {
        printConsole(RGB_WHITE, std::to_string(fov));
      } else if (tokens.size() == 2) {
        float parsedFov = 0;
        if (rayc::stof(tokens[1], parsedFov)) {
          fov = parsedFov;
        } else {
          printConsole(RGB_RED, "Invalid value");
        }
      } else {
        printConsole(RGB_RED, "Usage: fov [VALUE]");
      }
    } else if (tokens[0] == "pos") {
      if (tokens.size() == 1) {
        printConsole(RGB_WHITE, std::to_string(player.position.x) + " " + std::to_string(player.position.y));
      } else if (tokens.size() == 3) {
        float x = 0, y = 0;
        if (rayc::stof(tokens[1], x) && rayc::stof(tokens[2], y)) {
          player.position.x = x;
          player.position.y = y;
        } else {
          printConsole(RGB_RED, "Invalid value");
        }
      } else {
        printConsole(RGB_RED, "Usage: pos [x] [y]");
      }
    } else if (tokens[0] == "heading") {
      int side = -1;
      if (player.angle >= -M_PI * 0.25f && player.angle < M_PI * 0.25f) {
        side = 1;
      } else if (player.angle >= M_PI * 0.25f && player.angle < M_PI * 0.75f) {
        side = 3;
      } else if (player.angle < -M_PI * 0.25f && player.angle >= -M_PI * 0.75f) {
        side = 4;
      } else if (player.angle >= M_PI * 0.75f || player.angle < -M_PI * 0.75f) {
        side = 2;
      }
      printConsole(RGB_WHITE, std::to_string(side));
    } else if (tokens[0] == "fpscouter") {
      fpsCounter = !fpsCounter;
    } else if (tokens[0] == "profile") {
      profile = !profile;
    } else if (tokens[0] == "spriteoverlay") {
      spriteOverlay = !spriteOverlay;
    } else if (tokens[0] == "fpscap") {
      if (tokens.size() == 1) {
        printConsole(RGB_WHITE, std::to_string(getFpsCap()));
      } else if (tokens.size() == 2) {
        int fpsCap = 0;
        if (rayc::stoi(tokens[1], fpsCap)) {
          setFpsCap(fpsCap);
        } else {
          printConsole(RGB_RED, "Invalid value");
        }
      } else {
        printConsole(RGB_RED, "Usage: fpscap [VALUE]");
      }
    } else {
      printConsole(RGB_RED, "Unknown command");
    }
  }
}

Raycaster::DDAResult Raycaster::castRay(Vec2d src, Vec2d direction) {
  Raycaster::DDAResult result;

  Vec2d rayDelta = {
    sqrt(1 + (direction.y / direction.x) * (direction.y / direction.x)),
    sqrt(1 + (direction.x / direction.y) * (direction.x / direction.y))
  };

  Vec2i mapCheck = {(int)src.x, (int)src.y};
  Vec2d sideDistance, stepDistance;

  if (direction.x < 0) {
    stepDistance.x = -1;
    sideDistance.x = (src.x - (float)mapCheck.x) * rayDelta.x;
  } else {
    stepDistance.x = 1;
    sideDistance.x = ((float)mapCheck.x + 1 - src.x) * rayDelta.x;
  }

  if (direction.y < 0) {
    stepDistance.y = -1;
    sideDistance.y = (src.y - (float)mapCheck.y) * rayDelta.y;
  } else {
    stepDistance.y = 1;
    sideDistance.y = ((float)mapCheck.y + 1 - src.y) * rayDelta.y;
  }

  Vec2d intersection;
  Vec2i hitTile;
  float maxDistance = 100.0f;
  float distance = 0.0f;
  // bool tileFound = false;
  TileHit hit;

  while (!result.hitWall && distance < maxDistance) {
    if (sideDistance.x < sideDistance.y) {
      sideDistance.x += rayDelta.x;
      mapCheck.x += stepDistance.x;
    } else {
      sideDistance.y += rayDelta.y;
      mapCheck.y += stepDistance.y;
    }

    Vec2d rayDistance = {(float)mapCheck.x - src.x, (float)mapCheck.y - src.y};
    distance = sqrt(rayDistance.x * rayDistance.x + rayDistance.y * rayDistance.y);

    if (res.map.getTile(mapCheck).isSolid()) {
      hitTile = mapCheck;

      if (res.map.getTile(mapCheck).isDoor() && res.map.getTile(mapCheck).doorState != DOOR_CLOSED) {
        // tileFound = false;
        result.hitDoor = true;
      } else {
        // tileFound = true;
        result.hitWall = true;
      }

      hit.tilePosition = mapCheck;

      float m = direction.y / direction.x;

      if (src.y <= mapCheck.y) {
        if (src.x <= mapCheck.x) {
          hit.side = WEST;
          intersection.y = m * (mapCheck.x - src.x) + src.y;
          intersection.x = float(mapCheck.x);
          hit.sampleX = intersection.y - std::floor(intersection.y);
        } else if (src.x >= (mapCheck.x + 1)) {
          hit.side = EAST;
          intersection.y = m * ((mapCheck.x + 1) - src.x) + src.y;
          intersection.x = float(mapCheck.x + 1);
          hit.sampleX = intersection.y - std::floor(intersection.y);
        } else {
          hit.side = NORTH;
          intersection.y = float(mapCheck.y);
          intersection.x = (mapCheck.y - src.y) / m + src.x;
          hit.sampleX = intersection.x - std::floor(intersection.x);
        }

        if (intersection.y < mapCheck.y) {
          hit.side = NORTH;
          intersection.y = float(mapCheck.y);
          intersection.x = (mapCheck.y - src.y) / m + src.x;
          hit.sampleX = intersection.x - std::floor(intersection.x);
        }
      } else if (src.y >= mapCheck.y + 1) {
        if (src.x <= mapCheck.x) {
          hit.side = WEST;
          intersection.y = m * (mapCheck.x - src.x) + src.y;
          intersection.x = float(mapCheck.x);
          hit.sampleX = intersection.y - std::floor(intersection.y);
        } else if (src.x >= (mapCheck.x + 1)) {
          hit.side = EAST;
          intersection.y = m * ((mapCheck.x + 1) - src.x) + src.y;
          intersection.x = float(mapCheck.x + 1);
          hit.sampleX = intersection.y - std::floor(intersection.y);
        } else {
          hit.side = SOUTH;
          intersection.y = float(mapCheck.y + 1);
          intersection.x = ((mapCheck.y + 1) - src.y) / m + src.x;
          hit.sampleX = intersection.x - std::floor(intersection.x);
        }

        if (intersection.y > (mapCheck.y + 1)) {
          hit.side = SOUTH;
          intersection.y = float(mapCheck.y + 1);
          intersection.x = ((mapCheck.y + 1) - src.y) / m + src.x;
          hit.sampleX = intersection.x - std::floor(intersection.x);
        }
      } else {
        if (src.x <= mapCheck.x) {
          hit.side = WEST;
          intersection.y = m * (mapCheck.x - src.x) + src.y;
          intersection.x = float(mapCheck.x);
          hit.sampleX = intersection.y - std::floor(intersection.y);
        } else if (src.x >= (mapCheck.x + 1)) {
          hit.side = EAST;
          intersection.y = m * ((mapCheck.x + 1) - src.x) + src.y;
          intersection.x = float(mapCheck.x + 1);
          hit.sampleX = intersection.y - std::floor(intersection.y);
        }
      }

      hit.hitPosition = intersection;

      if (res.map.getTile(mapCheck).isDoor() && res.map.getTile(mapCheck).doorState != DOOR_CLOSED) {
        result.door = hit;
      } else {
        result.tile = hit;
      }
    }
  }

  return result;
}

void Raycaster::render(float frameTime) {
  auto start = std::chrono::system_clock::now();

  clearBuffer();

  int screenHeight = getHeight();
  int screenWidth = getWidth();
  int mapHeight = res.map.height;
  int mapWidth = res.map.width;

  setDrawColor(128, 128, 128, 255);
  fillRect({0, screenHeight/2, screenWidth, screenHeight/2});

  // Vec2d forward = {
  //   cos(player.angle),
  //   sin(player.angle)
  // };

  // Vec2d right = {
  //   forward.y,
  //   -forward.x
  // };

  // float halfWidth = tan(fov / 2.0f); // forbidden FOV - 180 deg == PI rad

  auto wallRenderStart = std::chrono::system_clock::now();

  // float* loopTime = new float[screenWidth];
  // int* whileCount = new int[screenWidth];

  // for (int x = 0; x < screenWidth; x+=textureColumnWidth) {
  //   auto wallRenderLoopStart = std::chrono::system_clock::now();
    
  //   float distanceToWall = 0;
  //   bool hitWall = false;

  //   float offset = ((x * 2.0f / (screenWidth - 1.0f)) - 1.0f) * halfWidth;
  //   Vec2d rayDirection = forward + right * offset;
  //   Vec2i test = {0, 0};

  //   int side = 0;
  //   float sampleX = 0;

  //   int count = 0;
  //   while (!hitWall && distanceToWall < depth) {
  //     distanceToWall += step;

  //     test = player.position + rayDirection * distanceToWall;

  //     if (test.x < 0 || test.x > mapWidth || test.y < 0 || test.y > mapHeight) {
  //       hitWall = true;
  //       distanceToWall = depth;
  //     } else {
  //       if (res.map.getTile(test).isSolid()) {
  //         hitWall = true;

  //         Vec2d blockMid = {test.x + 0.5f, test.y + 0.5f};
  //         Vec2d testPoint = player.position + rayDirection * distanceToWall;
  //         float testAngle = atan2f(testPoint.y - blockMid.y, testPoint.x - blockMid.x);

  //         if (testAngle >= -M_PI * 0.25f && testAngle < M_PI * 0.25f) {
  //           sampleX = testPoint.y - test.y;
  //           side = 1;
  //         } else if (testAngle >= M_PI * 0.25f && testAngle < M_PI * 0.75f) {
  //           sampleX = testPoint.x - test.x;
  //           side = 3;
  //         } else if (testAngle < -M_PI * 0.25f && testAngle >= -M_PI * 0.75f) {
  //           sampleX = testPoint.x - test.x;
  //           side = 4;
  //         } else if (testAngle >= M_PI * 0.75f || testAngle < -M_PI * 0.75f) {
  //           sampleX = testPoint.y - test.y;
  //           side = 2;
  //         }
  //       }
  //       count++;
  //     }

  //     auto wallRenderLoopEnd = std::chrono::system_clock::now();
  //     std::chrono::duration<float> loopDuration = wallRenderLoopEnd - wallRenderLoopStart;
  //     loopTime[x] = loopDuration.count();
  //   }

  //   int yStart = (screenHeight/2.0f) - screenHeight / distanceToWall/2.0f;
  //   int yEnd = yStart + (float)screenHeight/distanceToWall;

  //   int textureIdx = res.map.getTile(test).texture;
  //   Texture* texture = &res.texturePlaceholder;
  //   if (textureIdx >= 0 && textureIdx < res.textures.size()) {
  //     texture = &res.textures[textureIdx];
  //   }

  //   float whole;
  //   int textureX = std::modf(sampleX, &whole) * texture->getWidth();
  //   if (side == 2 || side == 3) {
  //     textureX = texture->getWidth() - textureX - 1;
  //   }

  //   depthBuffer[x] = distanceToWall;

  //   copyTexture(
  //     texture,
  //     Rect(
  //       textureX,
  //       0,
  //       textureColumnWidth,
  //       texture->getHeight()
  //     ),
  //     Rect(
  //       x,
  //       yStart,
  //       textureColumnWidth,
  //       (float)screenHeight/distanceToWall
  //     )
  //   );
  // }

  for (int x = 0; x < screenWidth; x++) {
    float rayAngle = (player.angle - fov/2.0f) + (x / (float)screenWidth) * fov;
    Vec2d rayDirection = {sinf(rayAngle), cosf(rayAngle)};

    DDAResult result = castRay(player.position, rayDirection);

    if (result.hitWall) {
      Vec2d ray = result.tile.hitPosition - player.position;
      float rayLength = sqrt(ray.x * ray.x + ray.y * ray.y) * cos(rayAngle - player.angle);

      depthBuffer[x] = rayLength;

      float ceiling = (screenHeight/2.0f) - screenHeight / rayLength;
      float floor = screenHeight - ceiling;
      float wallHeight = floor - ceiling;

      int textureIdx = res.map.getTile(result.tile.tilePosition).texture;
      Texture* texture = &res.texturePlaceholder;
      if (textureIdx >= 0 && textureIdx < res.textures.size()) {
        texture = &res.textures[textureIdx];
      }

      float whole;
      int textureX = std::modf(result.tile.sampleX, &whole) * texture->getWidth();

      if (result.tile.side == SOUTH || result.tile.side == WEST) {
        textureX = texture->getWidth() - textureX - 1;
      }

      copyTexture(texture, {textureX, 0, 1, texture->getHeight()}, {x, (int)ceiling, 1, (int)wallHeight});
    }

  }

  auto wallRenderEnd = std::chrono::system_clock::now();
  auto objectRenderStart = std::chrono::system_clock::now();

  for (auto &pair : objects) {
    pair.second->onFrameUpdate(frameTime);
    
    if (res.map.getTile(pair.second->position.x, pair.second->position.y).isSolid()) {
      pair.second->onCollision(nullptr);
    }

    Vec2d vec = pair.second->position - player.position;
    Vec2d direction = {sinf(player.angle), cosf(player.angle)};

    float objectAngle = atan2f(direction.y, direction.x) - atan2f(vec.y, vec.x);
    float distanceFromPlayer = sqrtf(vec.x*vec.x + vec.y*vec.y) * cosf(objectAngle);
    pair.first = distanceFromPlayer;

    if (objectAngle < -M_PI) {
      objectAngle += 2.0f * M_PI;
    }

    if (objectAngle > M_PI) {
      objectAngle -= 2.0f * M_PI;
    }

    bool isInFov = fabs(objectAngle) < (fov + (1.0f / distanceFromPlayer)) / 2.0f;

    if (isInFov && distanceFromPlayer >= 0.5f && distanceFromPlayer < depth) {
      Vec2d floorPoint = {
        (0.5f * ((objectAngle / (fov * 0.5f))) + 0.5f) * screenWidth,
        (screenHeight / 2.0f) + (screenHeight / distanceFromPlayer) / std::cos(objectAngle / 2.0f)
      };

      Vec2d objectSize = {(double)pair.second->texture->getWidth(), (double)pair.second->texture->getHeight()};
      objectSize *= 2.0f * screenHeight/objectSize.y;
      objectSize /= distanceFromPlayer;

      Vec2i start = {(int)(floorPoint.x - objectSize.x / 2.0f), (int)(floorPoint.y - objectSize.y + 100.0f/distanceFromPlayer)};

      float whole;

      // printf("obj: sz=(%f %f) a=%f d=%f st=(%d %d)\n", objectSize.x, objectSize.y, objectAngle, distanceFromPlayer, start.x, start.y);

      for (int sx = 0; sx < objectSize.x; sx++) {
        int textureX = (sx / objectSize.x) * pair.second->texture->getWidth();

        if (depthBuffer[start.x + sx] >= distanceFromPlayer) {
          copyTexture(pair.second->texture,
            {textureX, 0, 1, pair.second->texture->getHeight()},
            {start.x+sx, start.y, 1, (int)objectSize.y}
          );

          if (spriteOverlay) {
            copyTexture(&res.textureOverlay,
              {textureX, 0, 1, pair.second->texture->getHeight()},
              {start.x+sx, start.y, 1, (int)objectSize.y}
            );
          }
        }
      }
    }

    objects.remove_if([](auto& p) { return p.second->remove; });
    objects.sort([](const auto& a, const auto& b) { return a.first > b.first; });
  }

  if (profile) {
    auto objectRenderEnd = std::chrono::system_clock::now();

    std::chrono::duration<float> renderDuration = objectRenderEnd - start;
    float renderTime = renderDuration.count();

    std::chrono::duration<float> wallRenderDuration = wallRenderEnd - wallRenderStart;
    float wallRenderTime = wallRenderDuration.count();

    std::chrono::duration<float> objectRenderDuration = objectRenderEnd - objectRenderStart;
    float objectRenderTime = objectRenderDuration.count();


    char buffer[32] = {0};
    int height = 0;

    auto printBuffer = [this, &buffer, &height]() {
      res.fonts["main"]->draw(
        std::string(buffer),
        {0, height},
        RGB_WHITE
      );
      height += res.fonts["main"]->getSize();
    };

    // float sum = 0;
    // for (int x = 0; x < screenWidth; x+=textureColumnWidth) {
    //   sum += loopTime[x];
    // }

    // int whileSum = 0;
    // for (int x = 0; x < screenWidth; x+=textureColumnWidth) {
    //   whileSum += whileCount[x];
    // }

    snprintf(buffer, 32, "targetFrameTime: %8f", 1.0f/getFpsCap());
    printBuffer();
    snprintf(buffer, 32, "frameTime: %8f", frameTime);
    printBuffer();
    snprintf(buffer, 32, "renderTime: %8f", renderTime);
    printBuffer();
    snprintf(buffer, 32, "wallRenderTime: %8f", wallRenderTime);
    printBuffer();
    // snprintf(buffer, 32, "avgWallLoopTime: %8f", sum/screenWidth);
    // printBuffer();
    // snprintf(buffer, 32, "avgWhileCount: %8d", whileSum/screenWidth);
    // printBuffer();
    snprintf(buffer, 32, "objectRenderTime: %8f", objectRenderTime);
    printBuffer();
  }

  // delete [] loopTime;
  // delete [] whileCount;
}

void Raycaster::processInput(float frameTime) {
  // if (getKeyState(SDL_SCANCODE_LEFT).held) {
  //   player.angle += rotationSpeed * frameTime;
  //   if (player.angle > 2.0f * M_PI) { // kinda works?
  //     player.angle -= 2.0f * M_PI;
  //   }
  // }

  // if (getKeyState(SDL_SCANCODE_RIGHT).held) {
  //   player.angle -= rotationSpeed * frameTime;
  //   if (player.angle < -2.0f * M_PI) { // kinda works?
  //     player.angle += 2.0f * M_PI;
  //   }
  // }

  // if (getKeyState(SDL_SCANCODE_W).held) {
  //   player.position.x += cosf(player.angle) * movementSpeed * frameTime;
  //   player.position.y += sinf(player.angle) * movementSpeed * frameTime;

  //   if (res.map.getTile((int)player.position.x, (int)player.position.y).isSolid()) {
  //     player.position.x -= cosf(player.angle) * movementSpeed * frameTime;
  //     player.position.y -= sinf(player.angle) * movementSpeed * frameTime;
  //   }
  // }

  // if (getKeyState(SDL_SCANCODE_S).held) {
  //   player.position.x -= cosf(player.angle) * movementSpeed * frameTime;
  //   player.position.y -= sinf(player.angle) * movementSpeed * frameTime;

  //   if (res.map.getTile((int)player.position.x, (int)player.position.y).isSolid()) {
  //     player.position.x += cosf(player.angle) * movementSpeed * frameTime;
  //     player.position.y += sinf(player.angle) * movementSpeed * frameTime;
  //   }
  // }

  // if (getKeyState(SDL_SCANCODE_A).held) {
  //   player.position.x -= sinf(player.angle) * movementSpeed * frameTime;
  //   player.position.y += cosf(player.angle) * movementSpeed * frameTime;

  //   if (res.map.getTile((int)player.position.x, (int)player.position.y).isSolid()) {
  //     player.position.x += sinf(player.angle) * movementSpeed * frameTime;
  //     player.position.y -= cosf(player.angle) * movementSpeed * frameTime;
  //   }
  // }
  
  // if (getKeyState(SDL_SCANCODE_D).held) {
  //   player.position.x += sinf(player.angle) * movementSpeed * frameTime;
  //   player.position.y -= cosf(player.angle) * movementSpeed * frameTime;

  //   if (res.map.getTile((int)player.position.x, (int)player.position.y).isSolid()) {
  //     player.position.x -= sinf(player.angle) * movementSpeed * frameTime;
  //     player.position.y += cosf(player.angle) * movementSpeed * frameTime;
  //   }
  // }

  if (getKeyState(SDL_SCANCODE_LEFT).held) {
    player.angle -= rotationSpeed * frameTime;
    if (player.angle < -2.0f * M_PI) { // kinda works?
      player.angle += 2.0f * M_PI;
    }
  }

  if (getKeyState(SDL_SCANCODE_RIGHT).held) {
    player.angle += rotationSpeed * frameTime;
    if (player.angle > 2.0f * M_PI) { // kinda works?
      player.angle -= 2.0f * M_PI;
    }
  }

  if (getKeyState(SDL_SCANCODE_W).held) {
    player.position.x += sinf(player.angle) * movementSpeed * frameTime;
    player.position.y += cosf(player.angle) * movementSpeed * frameTime;

    if (res.map.getTile((int)player.position.x, (int)player.position.y).isSolid()) {
      player.position.x -= sinf(player.angle) * movementSpeed * frameTime;
      player.position.y -= cosf(player.angle) * movementSpeed * frameTime;
    }
  }

  if (getKeyState(SDL_SCANCODE_S).held) {
    player.position.x -= sinf(player.angle) * movementSpeed * frameTime;
    player.position.y -= cosf(player.angle) * movementSpeed * frameTime;

    if (res.map.getTile((int)player.position.x, (int)player.position.y).isSolid()) {
      player.position.x += sinf(player.angle) * movementSpeed * frameTime;
      player.position.y += cosf(player.angle) * movementSpeed * frameTime;
    }
  }

  if (getKeyState(SDL_SCANCODE_A).held) {
    player.position.x -= cosf(player.angle) * movementSpeed * frameTime;
    player.position.y += sinf(player.angle) * movementSpeed * frameTime;

    if (res.map.getTile((int)player.position.x, (int)player.position.y).isSolid()) {
      player.position.x += cosf(player.angle) * movementSpeed * frameTime;
      player.position.y -= sinf(player.angle) * movementSpeed * frameTime;
    }
  }
  
  if (getKeyState(SDL_SCANCODE_D).held) {
    player.position.x += cosf(player.angle) * movementSpeed * frameTime;
    player.position.y -= sinf(player.angle) * movementSpeed * frameTime;

    if (res.map.getTile((int)player.position.x, (int)player.position.y).isSolid()) {
      player.position.x -= cosf(player.angle) * movementSpeed * frameTime;
      player.position.y += sinf(player.angle) * movementSpeed * frameTime;
    }
  }
}

bool onFrameUpdateCb(float frameTime) {
  return raycaster.onFrameUpdate(frameTime);
}

void onConsoleCommandCb(std::string line) {
  raycaster.onConsoleCommand(line);
}

int main(int argc, char ** argv) {
  try {
    if (argc != 2) {
      error("Usage: %s DATA_FOLDER", argv[0]);
      return 1;
    }

    setDataFolder(argv[1]);

    if (!checkDataFolderStructure(getDataFolder())) {
      error("Bad data folder");
      return 1;
    }

    setLogLevel(LogLevel::DEBUG);

    raycaster.config = Config::fromFile(getResourcePath(RES_RAYC_CONFIG));

    init(
      std::stoi(raycaster.config.getValueOr("window", "width", "800")),
      std::stoi(raycaster.config.getValueOr("window", "height", "600"))
    );

    raycaster.res.fonts["main"] = new Font(getResourcePath(RES_FONT, raycaster.config.getValueOrDie("fonts", "main", "fonts.main is required")));
    setConsoleFont(raycaster.res.fonts["main"]);

    raycaster.init();
    run(onFrameUpdateCb, onConsoleCommandCb);
    shutdown();

    return 0;

  } catch (std::exception& e) {
    fatal("Uncaught exception: %s", e.what());
    return 1;
  }
}
