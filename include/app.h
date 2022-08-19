#ifndef _RAYC_APP_H_
#define _RAYC_APP_H_ 1

#include <cstdlib>
#include <string>
#include <functional>

#include <SDL2/SDL.h>

#include <rayc/video/font.h>

namespace rayc {

using UpdaterCb = std::function<bool(float)>;
using ConsoleCommandCb = std::function<void(std::string)>;

struct KeyState {
  bool pressed = false;
  bool held = false;
  bool released = false;

  KeyState() = default;
  KeyState(bool p, bool h, bool r);
};

void init(int width, int height);
void shutdown();
void run(UpdaterCb cb, ConsoleCommandCb commandCb);
[[noreturn]] void die(int exitCode = EXIT_FAILURE);

int getWidth();
int getHeight();

KeyState getKeyState(SDL_Scancode sc);

void enableTextInput();
void disableTextInput();
bool isTextInputEnabled();

bool isTextInputReady();
std::string getTextInput();
SDL_Scancode getTextInputEvent();

void setConsoleFont(Font* font);
void printConsole(SDL_Color color, const std::string& line);
void clearConsole();
int getLineLimit();
void setLineLimit(int limit);

void setFpsCap(int cap);
int getFpsCap();

int getCycles();

SDL_Window* getWindow();
SDL_Renderer* getRenderer();

} /* namespace rayc */

#endif /* _RAYC_APP_H_ */