#ifndef _RAYC_VIDEO_TEXTURE_H_
#define _RAYC_VIDEO_TEXTURE_H_ 1

#include <string>
#include <SDL2/SDL.h>

namespace rayc {

class Texture {
 private:
  std::string m_filename;
  SDL_Texture* m_texture = nullptr;
  int m_width = 0;
  int m_height = 0;

 public:
  Texture();
  Texture(const std::string& filename);
  Texture(int w, int h);
  Texture(const Texture& rhs) = delete;
  Texture(Texture&& rhs);
  ~Texture();

  Texture& operator=(Texture&& rhs);

  Texture&& copy() const;

  SDL_Texture* getSdlTexture() const;

  int getWidth() const;
  int getHeight() const;
};

} /* namespace rayc */

#endif /* _RAYC_VIDEO_TEXTURE_H_ */