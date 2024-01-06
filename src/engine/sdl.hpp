#pragma once
// reviewed: 2023-12-22
// reviewed: 2023-12-27
// reviewed: 2024-01-04
// reviewed: 2024-01-06

namespace glos {
class sdl final {
public:
  inline void init() {
    if (SDL_Init(SDL_INIT_VIDEO)) {
      fprintf(stderr, "\n%s:%d: cannot initiate sdl video: %s\n", __FILE__,
              __LINE__, SDL_GetError());
      fflush(stderr);
      std::abort();
    }
  }

  inline void free() { SDL_Quit(); }
};

inline sdl sdl{};
} // namespace glos