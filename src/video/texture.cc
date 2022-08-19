#include <rayc/video/texture.h>
#include <rayc/app.h>
#include <rayc/log.h>

#include <SDL2/SDL_image.h>

rayc::Texture::Texture() {}

rayc::Texture::Texture(const std::string& filename) : m_filename(filename) {
  m_texture = IMG_LoadTexture(getRenderer(), filename.c_str());
  if (!m_texture) {
    error("Error loading texture '%s'", filename.c_str());
    die();
  } else {
    SDL_QueryTexture(m_texture, NULL, NULL, &m_width, &m_height);
  }
  debug("Texture(%s) %p", filename.c_str(), m_texture);
}

rayc::Texture::Texture(int w, int h) {
  m_width = w;
  m_height = h;
  m_texture = SDL_CreateTexture(getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
  if (!m_texture) {
    error("Error creating texture %dx%d", w, h);
    die();
  }
}

rayc::Texture::Texture(Texture&& rhs) {
  m_texture = rhs.m_texture;
  m_width = rhs.m_width;
  m_height = rhs.m_height;

  rhs.m_texture = nullptr;
}

rayc::Texture::~Texture() {
  if (m_texture) {
    SDL_DestroyTexture(m_texture);
  }
}

rayc::Texture& rayc::Texture::operator=(rayc::Texture&& rhs) {
  m_texture = rhs.m_texture;
  m_width = rhs.m_width;
  m_height = rhs.m_height;

  rhs.m_texture = nullptr;
  return *this;
}

rayc::Texture&& rayc::Texture::copy() const {
  return std::move(Texture(m_filename));
}

SDL_Texture* rayc::Texture::getSdlTexture() const {
  return m_texture;
}

// SDL_Surface* rayc::Texture::getPixels() const {}

int rayc::Texture::getWidth() const {
  return m_width;
}

int rayc::Texture::getHeight() const {
  return m_height;
}