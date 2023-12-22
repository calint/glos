#pragma once
// reviewed: 2023-12-22

class window final {
public:
  SDL_Window *sdl_window = nullptr;
  SDL_Renderer *renderer = nullptr;
  SDL_GLContext glcontext = nullptr;

  inline void init() {
    gl_request_profile_and_version(SDL_GL_CONTEXT_PROFILE_ES, 3, 2);

    sdl_window =
        SDL_CreateWindow("glos", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         window_width, window_height, SDL_WINDOW_OPENGL);
    if (!sdl_window) {
      printf("%s %d: %s", __FILE__, __LINE__, SDL_GetError());
      exit(1);
    }
    glcontext = SDL_GL_CreateContext(sdl_window);
    if (!glcontext) {
      printf("%s %d: %s", __FILE__, __LINE__, SDL_GetError());
      exit(2);
    }
    if (SDL_GL_SetSwapInterval(1)) {
      printf("%s %d: %s", __FILE__, __LINE__, SDL_GetError());
      exit(3);
    }
    if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1)) {
      printf("%s %d: %s", __FILE__, __LINE__, SDL_GetError());
      exit(4);
    }
    if (SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24)) {
      printf("%s %d: %s", __FILE__, __LINE__, SDL_GetError());
      exit(5);
    }
    renderer = SDL_CreateRenderer(sdl_window, -1, SDL_WINDOW_OPENGL);
    if (!renderer) {
      printf("%s %d: %s", __FILE__, __LINE__, SDL_GetError());
      exit(6);
    }

    gl_print_context_profile_and_version();
  }

  inline void free() {
    SDL_DestroyRenderer(renderer);
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(sdl_window);
  }

  inline void swap_buffers() { SDL_GL_SwapWindow(sdl_window); }

  inline static void gl_request_profile_and_version(SDL_GLprofile prof,
                                                    int major, int minor) {
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, prof)) {
      printf("%s %d: %s", __FILE__, __LINE__, SDL_GetError());
      exit(23);
    }
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major)) {
      printf("%s %d: %s", __FILE__, __LINE__, SDL_GetError());
      exit(24);
    }
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor)) {
      printf("%s %d: %s", __FILE__, __LINE__, SDL_GetError());
      exit(25);
    }
  }

  inline static void gl_print_context_profile_and_version() {
    int value;
    if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &value)) {
      printf("%s %d: %s", __FILE__, __LINE__, SDL_GetError());
      exit(26);
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
      exit(22);
    }
    printf(" (%d)\n", value);

    if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &value)) {
      printf("%s %d: %s", __FILE__, __LINE__, SDL_GetError());
      exit(27);
    }
    printf("%-32s  %d\n", "SDL_GL_CONTEXT_MAJOR_VERSION", value);

    if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &value)) {
      printf("%s %d: %s", __FILE__, __LINE__, SDL_GetError());
      exit(28);
    }
    printf("%-32s  %d\n", "SDL_GL_CONTEXT_MINOR_VERSION", value);
  }
};

static window window{};
