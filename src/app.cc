#include <rayc/app.h>
#include <rayc/log.h>
#include <rayc/version.h>
#include <rayc/math/rect.h>
#include <rayc/video/font.h>
#include <rayc/video/draw.h>
#include <rayc/video/color.h>

#include <cstdio>
#include <vector>
#include <chrono>
#include <cstring>
#include <iostream>
#include <algorithm>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

using namespace rayc;

struct FrameKeyState {
  bool pressed = false;
  bool released = false;
};

struct ApplicationState {
  bool isRunning = false;
  bool isInitialized = false;

  int screenWidth = 0;
  int screenHeight = 0;

  int cycles = 0;
  int fpsCap = 60;

  FrameKeyState keyState[322];
  bool heldKeys[322];
  bool isTextInputEnabled = false;
  bool isTextInputReady = false;
  std::string textInputLine;
  SDL_Scancode textInputEvent;

  struct ConsoleEntry {
    SDL_Color color = RGB_WHITE;
    std::string text;
  };

  struct ConsoleState {
    bool active = false;
    std::string prompt = "]";
    Font* font = nullptr;
    int lineLimit = 25;
    std::vector<ConsoleEntry> buffer;
  } console;

  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
} state;


rayc::KeyState::KeyState(bool p, bool h, bool r)
  : pressed(p), held(h), released(r) {}


static void resetKeys() {
  for (int i = 0; i < 322; i++) {
    state.keyState[i].pressed = false;
    state.keyState[i].released = false;
    state.heldKeys[i] = false;
  }
}

static void updateKeydown(SDL_Scancode sc) {
  if (!state.heldKeys[sc]) {
    state.keyState[sc].pressed = true;
    state.heldKeys[sc] = true;
  }
}

static void renderConsole() {
  Rect consoleBg = {0, 0, getWidth(), state.console.font->getSize() * (state.console.lineLimit + 1)};

  setDrawColor(0, 0, 0, 128);
  fillRect(consoleBg);

  int textYoffset = 0;

  if (state.console.font) {
    if (state.console.buffer.size() < state.console.lineLimit) {
      for (auto& entry : state.console.buffer) {
        state.console.font->draw(entry.text, {0, textYoffset}, entry.color);
        textYoffset += state.console.font->getSize();
      }
    } else {
      for (int i = state.console.buffer.size()-state.console.lineLimit; i<state.console.buffer.size(); i++) {
        state.console.font->draw(state.console.buffer[i].text, {0, textYoffset}, state.console.buffer[i].color);
        textYoffset += state.console.font->getSize();
      }
    }
    std::string command = state.console.prompt + state.textInputLine;
    state.console.font->draw(command, {0, textYoffset}, RGB_WHITE);
  }
}

void rayc::init(int width, int height) {
  printf("rayc v%s\n", RAYC_VERSION_STRING);

  state.screenWidth = width;
  state.screenHeight = height;

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    sdlFatal("SDL_Init failed");
    die();
  }

  if (IMG_Init(IMG_INIT_JPG) < 0) {
    sdlFatal("IMG_Init failed");
    die();
  }

  if (TTF_Init() < 0) {
    sdlFatal("TTF_Init failed");
    die();
  }

  state.window = SDL_CreateWindow(
    "rayc",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    width,
    height,
    SDL_WINDOW_SHOWN
  );

  state.renderer = SDL_CreateRenderer(state.window, -1, 0);
  if (state.renderer == NULL) {
    sdlFatal("Renderer creation failed");
    die();
  }

  SDL_SetRenderDrawBlendMode(getRenderer(), SDL_BLENDMODE_BLEND);

  memset(&state.keyState, 0, 322*sizeof(FrameKeyState));
  memset(&state.heldKeys, 0, 322);

  info("rayc initialized succesfully");
  state.isInitialized = true;
}

void rayc::shutdown() {
  if (!state.isInitialized) {
    return;
  }

  TTF_Quit();
  IMG_Quit();
  SDL_DestroyRenderer(state.renderer);
  SDL_DestroyWindow(state.window);
  SDL_Quit();

  state.isInitialized = false;

  info("rayc stopped");
}

void rayc::run(UpdaterCb cb, ConsoleCommandCb commandCb) {
  auto time1 = std::chrono::system_clock::now();
  auto time2 = std::chrono::system_clock::now();
  auto time3 = std::chrono::system_clock::now();

  float frameTime = 0.1;        // Time spent rendering
  float actualFrameTime = 0.1;  // Time spent in this function during 1 frame

  setDrawColor(0, 0, 0, 255);
  clearBuffer();

  state.isRunning = true;

  while (state.isRunning) {
    time1 = std::chrono::system_clock::now();

    state.cycles++;
    float targetFrameTime = 1.0f/state.fpsCap;

    memset(&state.keyState, 0, 322*sizeof(FrameKeyState));

    state.isTextInputReady = false;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT: {
          state.isRunning = false;
          shutdown();
          return;
        }
        case SDL_KEYDOWN: {
          SDL_Scancode sc = event.key.keysym.scancode;
          if (event.key.keysym.scancode == SDL_SCANCODE_GRAVE) {
            if (state.console.active) {
              resetKeys();
              state.console.active = false;
              disableTextInput();
            } else {
              state.console.active = true;
              enableTextInput();
            }
          } else {
            updateKeydown(sc);

            if (state.isTextInputEnabled) {
              state.textInputEvent = sc;
              switch (sc) {
                case SDL_SCANCODE_BACKSPACE: {
                  if (!state.textInputLine.empty()) {
                    state.textInputLine.pop_back();
                  }
                  break;
                }
                case SDL_SCANCODE_RETURN: {
                  state.isTextInputReady = true;
                  break;
                }
                default:
                  break;
              }
            }
            if (state.console.active && state.isTextInputReady) {
              printConsole({255, 255, 0}, state.console.prompt + state.textInputLine);
              commandCb(state.textInputLine);
            }
          }
          break;
        }
        case SDL_KEYUP: {
          state.keyState[event.key.keysym.scancode].released = true;
          state.heldKeys[event.key.keysym.scancode] = false;
          break;
        }
        case SDL_TEXTINPUT: {
          state.textInputLine += event.text.text;
          break;
        }
      }
    }

    clearBuffer();

    state.isRunning = cb(actualFrameTime);

    if (state.console.active) {
      renderConsole();
    }

    renderBuffer();

    time2 = std::chrono::system_clock::now();
    std::chrono::duration<float> frameDuration = time2 - time1;
    frameTime = frameDuration.count();

    // Fps cap
    if (frameTime < targetFrameTime) {
      SDL_Delay((targetFrameTime - frameTime) * 1000);
    }

    if (state.isTextInputReady) {
      state.isTextInputReady = false;
      state.textInputLine = "";
    }

    time3 = std::chrono::system_clock::now();
    std::chrono::duration<float> actualFrameDuration = time3 - time1;
    actualFrameTime = actualFrameDuration.count();
    // frameTime = actualFrameDuration.count();
  }
}

[[noreturn]] void rayc::die(int exitCode) {
  shutdown();
  exit(exitCode);
}

int rayc::getWidth() {
  return state.screenWidth;
}

int rayc::getHeight() {
  return state.screenHeight;
}

rayc::KeyState rayc::getKeyState(SDL_Scancode sc) {
  return KeyState(state.keyState[sc].pressed, state.heldKeys[sc], state.keyState[sc].released);
}

void rayc::enableTextInput() {
  state.isTextInputEnabled = true;
  SDL_StartTextInput();
  state.isTextInputReady = false;
  state.textInputLine = "";
}

void rayc::disableTextInput() {
  state.isTextInputEnabled = false;
  SDL_StopTextInput();
}

bool rayc::isTextInputEnabled() {
  return state.isTextInputEnabled;
}

bool rayc::isTextInputReady() {
  return state.isTextInputReady;
}

std::string rayc::getTextInput() {
  return state.textInputLine;
}

SDL_Scancode rayc::getTextInputEvent() {
  return state.textInputEvent;
}

void rayc::printConsole(SDL_Color color, const std::string& line) {
  state.console.buffer.push_back({color, line});
}

void rayc::setConsoleFont(Font* font) {
  state.console.font = font;
}

void rayc::clearConsole() {
  state.console.buffer.clear();
}

int rayc::getLineLimit() {
  return state.console.lineLimit;
}

void rayc::setLineLimit(int limit) {
  state.console.lineLimit = limit;
}

void rayc::setFpsCap(int cap) {
  state.fpsCap = cap;
}

int rayc::getFpsCap() {
  return state.fpsCap;
}

int rayc::getCycles() {
  return state.cycles;
}

SDL_Window* rayc::getWindow() {
  return state.window;
}

SDL_Renderer* rayc::getRenderer() {
  return state.renderer;
}
