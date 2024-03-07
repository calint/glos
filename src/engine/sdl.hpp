#pragma once
// reviewed: 2023-12-22
// reviewed: 2023-12-27
// reviewed: 2024-01-04
// reviewed: 2024-01-06
// reviewed: 2024-01-10

namespace glos {

class sdl final {
public:
  inline void init() {
    if (SDL_Init(SDL_INIT_VIDEO)) {
      throw glos_exception{
          std::format("cannot initiate sdl video: {}", SDL_GetError())};
    }
  }

  inline void free() { SDL_Quit(); }
};

static sdl sdl{};
} // namespace glos