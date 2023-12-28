#pragma once
// reviewed: 2023-12-22
// reviewed: 2023-12-27

class window final {
  SDL_Window *sdl_window = nullptr;
  SDL_Renderer *sdl_renderer = nullptr;
  SDL_GLContext sdl_gl_context = nullptr;

public:
  inline void init() {
    sdl_window =
        SDL_CreateWindow("glos", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         window_width, window_height, SDL_WINDOW_OPENGL);
    if (!sdl_window) {
      printf("%s %d: %s", __FILE__, __LINE__, SDL_GetError());
      std::abort();
    }

    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
    //                     SDL_GL_CONTEXT_PROFILE_CORE);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    sdl_gl_context = SDL_GL_CreateContext(sdl_window);
    if (!sdl_gl_context) {
      printf("%s %d: %s", __FILE__, __LINE__, SDL_GetError());
      std::abort();
    }

    sdl_renderer = SDL_CreateRenderer(
        sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!sdl_renderer) {
      printf("%s %d: %s", __FILE__, __LINE__, SDL_GetError());
      std::abort();
    }

    gl_print_context_profile_and_version();
  }

  inline void free() {
    SDL_DestroyRenderer(sdl_renderer);
    SDL_GL_DeleteContext(sdl_gl_context);
    SDL_DestroyWindow(sdl_window);
  }

  inline void swap_buffers() { SDL_GL_SwapWindow(sdl_window); }

  inline auto get_width_and_height() const -> std::pair<int, int> {
    int w = 0;
    int h = 0;
    SDL_GetWindowSize(sdl_window, &w, &h);
    return {w, h};
  }

private:
  inline static void gl_print_context_profile_and_version() {
    int value;
    if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &value)) {
      printf("%s %d: %s", __FILE__, __LINE__, SDL_GetError());
      std::abort();
    }
    printf("%-32s", "SDL_GL_CONTEXT_PROFILE_MASK");
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
      printf("%s %d: %d", __FILE__, __LINE__, value);
      std::abort();
    }
    printf(" (%d)\n", value);

    if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &value)) {
      printf("%s %d: %s", __FILE__, __LINE__, SDL_GetError());
      std::abort();
    }
    printf("%-32s  %d\n", "SDL_GL_CONTEXT_MAJOR_VERSION", value);

    if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &value)) {
      printf("%s %d: %s", __FILE__, __LINE__, SDL_GetError());
      std::abort();
    }
    printf("%-32s  %d\n", "SDL_GL_CONTEXT_MINOR_VERSION", value);
  }
};

static window window{};
