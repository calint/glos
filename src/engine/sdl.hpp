#pragma once
// reviewed: 2023-12-22
// reviewed: 2023-12-27
// reviewed: 2024-01-04
// reviewed: 2024-01-06
// reviewed: 2024-01-10

namespace glos {

class sdl final {
public:
  inline auto init() -> void {
    if (SDL_Init(SDL_INIT_VIDEO)) {
      throw exception{
          std::format("cannot initiate sdl video: {}", SDL_GetError())};
    }
  }

  inline auto free() -> void { SDL_Quit(); }
};

static sdl sdl{};
} // namespace glos