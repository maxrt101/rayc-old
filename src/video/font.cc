#include <rayc/video/font.h>
#include <rayc/math/rect.h>
#include <rayc/app.h>
#include <rayc/log.h>

rayc::Font::Font(const std::string& filename) : Font(filename, 14) {}

rayc::Font::Font(const std::string& filename, int size) {
  m_size = size;
  m_font = TTF_OpenFont(filename.c_str(), size);
  if (!m_font) {
    sdlError("Failed to open font");
    die();
  }
  debug("Font(%s) %p", filename.c_str(), m_font);
}

TTF_Font* rayc::Font::getTtfFont() const {
  return m_font;
}

int rayc::Font::getSize() const {
  return m_size;
}

void rayc::Font::draw(const std::string& text, Vec2i pos, SDL_Color color) {
  if (!m_font || text.empty()) {
    return;
  }

  SDL_Surface* surface = TTF_RenderText_Solid(m_font, text.c_str(), color);
  SDL_Texture* texture = SDL_CreateTextureFromSurface(getRenderer(), surface);

  Rect dest = {pos.x, pos.y, surface->w, surface->h};
  SDL_Rect destSdl = dest.toSdlRect();

  SDL_RenderCopy(getRenderer(), texture, NULL, &destSdl);

  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}
