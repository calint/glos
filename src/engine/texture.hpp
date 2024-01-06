#pragma once
// reviewed: 2024-01-04

#include "metrics.hpp"
#include <GLES3/gl3.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <unordered_map>

namespace glos {
class texture final {
public:
  GLuint id = 0;
  size_t size_B = 0;

  inline void load(std::string const &path) {
    // load texture
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    printf(" * loading texture %u from '%s'\n", id, path.c_str());
    SDL_Surface *surface = IMG_Load(path.c_str());
    if (not surface) {
      fprintf(stderr, "\n%s:%d: cannot load image from '%s'\n", __FILE__,
              __LINE__, path.c_str());
      fflush(stderr);
      std::abort();
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, surface->pixels);
    size_B = size_t(surface->w * surface->h) * sizeof(uint32_t);
    SDL_FreeSurface(surface);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
};

class textures final {
public:
  inline void init() {}

  inline void free() {
    for (auto const &pair : store) {
      glDeleteTextures(1, &pair.second.id);
      metrics.buffered_texture_data -= pair.second.size_B;
    }
    store.clear();
  };

  inline auto get_id_or_load(std::string const &path) -> GLuint {
    auto it = store.find(path);
    if (it != store.end()) {
      return it->second.id;
    }

    texture tex{};
    tex.load(path);
    metrics.buffered_texture_data += tex.size_B;
    // get id before moving tex
    GLuint const id = tex.id;
    store.insert({path, std::move(tex)});
    return id;
  }

private:
  std::unordered_map<std::string, texture> store{};
};

inline textures textures{};
} // namespace glos