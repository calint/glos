#pragma once
// reviewed: 2023-12-22
// reviewed: 2023-12-27
// reviewed: 2024-01-04

#include <SDL2/SDL.h>

namespace glos {
class sdl final {
public:
  inline void init() {
    if (SDL_Init(SDL_INIT_VIDEO)) {
      printf("%s:%d: %s\n", __FILE__, __LINE__, SDL_GetError());
      std::abort();
    }
  }

  inline void free() { SDL_Quit(); }
};

inline sdl sdl{};
} // namespace glos