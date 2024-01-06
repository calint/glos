#pragma once
// reviewed: 2023-12-22
// reviewed: 2023-12-27
// reviewed: 2024-01-04
// reviewed: 2024-01-06

namespace glos {
class window final {
public:
  inline void init() {
    sdl_window =
        SDL_CreateWindow("glos", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         window_width, window_height, SDL_WINDOW_OPENGL);
    if (not sdl_window) {
      fprintf(stderr, "\n%s:%d: cannot create window: %s\n", __FILE__, __LINE__,
              SDL_GetError());
      fflush(stderr);
      std::abort();
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    sdl_gl_context = SDL_GL_CreateContext(sdl_window);
    if (not sdl_gl_context) {
      fprintf(stderr, "\n%s:%d: cannot create gl context: %s\n", __FILE__,
              __LINE__, SDL_GetError());
      fflush(stderr);
      std::abort();
    }

    sdl_renderer =
        SDL_CreateRenderer(sdl_window, -1,
                           SDL_RENDERER_ACCELERATED |
                               (window_vsync ? SDL_RENDERER_PRESENTVSYNC : 0));
    if (not sdl_renderer) {
      fprintf(stderr, "\n%s:%d: cannot create renderer: %s\n", __FILE__,
              __LINE__, SDL_GetError());
      fflush(stderr);
      std::abort();
    }

    gl_print_context_profile_and_version();
  }

  inline void free() {
    SDL_DestroyRenderer(sdl_renderer);
    SDL_GL_DeleteContext(sdl_gl_context);
    SDL_DestroyWindow(sdl_window);
  }

  inline void swap_buffers() const { SDL_GL_SwapWindow(sdl_window); }

  inline auto get_width_and_height() const -> std::pair<int, int> {
    int w = 0;
    int h = 0;
    SDL_GetWindowSize(sdl_window, &w, &h);
    return {w, h};
  }

private:
  SDL_Window *sdl_window = nullptr;
  SDL_Renderer *sdl_renderer = nullptr;
  SDL_GLContext sdl_gl_context = nullptr;

  static inline void gl_print_context_profile_and_version() {
    int value = 0;
    if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &value)) {
      fprintf(stderr, "\n%s:%d: cannot get opengl attribute: %s\n", __FILE__,
              __LINE__, SDL_GetError());
      fflush(stderr);
      std::abort();
    }
    printf("SDL_GL_CONTEXT_PROFILE_MASK = ");
    switch (value) {
    case SDL_GL_CONTEXT_PROFILE_CORE:
      printf("SDL_GL_CONTEXT_PROFILE_CORE");
      break;
    case SDL_GL_CONTEXT_PROFILE_COMPATIBILITY:
      printf("SDL_GL_CONTEXT_PROFILE_COMPATIBILITY");
      break;
    case SDL_GL_CONTEXT_PROFILE_ES:
      printf("SDL_GL_CONTEXT_PROFILE_ES");
      break;
    case 0:
      printf("any");
      break;
    default:
      perror("unknown option");
      printf("%s:%d: %d\n", __FILE__, __LINE__, value);
    }
    printf(" (%d)\n", value);

    if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &value)) {
      fprintf(stderr, "\n%s:%d: cannot get opengl attribute: %s\n", __FILE__,
              __LINE__, SDL_GetError());
      fflush(stderr);
      std::abort();
    }
    printf("SDL_GL_CONTEXT_MAJOR_VERSION = %d\n", value);

    if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &value)) {
      fprintf(stderr, "\n%s:%d: cannot get opengl attribute: %s\n", __FILE__,
              __LINE__, SDL_GetError());
      fflush(stderr);
      std::abort();
    }
    printf("SDL_GL_CONTEXT_MINOR_VERSION = %d\n", value);
  }
};

inline window window{};
}; // namespace glos