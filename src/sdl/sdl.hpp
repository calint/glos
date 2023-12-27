#pragma once
// reviewed: 2023-12-22
// reviewed: 2023-12-27

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_opengles2.h>

class sdl final {
  Mix_Music *music = nullptr;

public:
  inline void init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
      printf("%s %d: %s", __FILE__, __LINE__, SDL_GetError());
      std::abort();
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
      printf("%s %d: %s", __FILE__, __LINE__, SDL_GetError());
      std::abort();
    }
  }

  inline void free() {
    Mix_FreeMusic(music);
    SDL_Quit();
  }

  inline void play_mp3_path(const char *path) {
    music = Mix_LoadMUS(path);
    if (music == nullptr) {
      printf("%s %d: %s", __FILE__, __LINE__, Mix_GetError());
      std::abort();
    }

    if (Mix_PlayMusic(music, -1) == -1) {
      printf("%s %d: %s", __FILE__, __LINE__, Mix_GetError());
      std::abort();
    }
  }
};

static sdl sdl{};
