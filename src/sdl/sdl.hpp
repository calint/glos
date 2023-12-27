#pragma once
// reviewed: 2023-12-22

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_opengles2.h>
#include <iostream>
#include <unistd.h>

class sdl final {
public:
  Mix_Music *music = nullptr;

  inline void init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
      std::cout << "SDL_Init failed: " << SDL_GetError() << std::endl;
      abort();
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
      std::cout << "Mix_OpenAudio failed: " << Mix_GetError() << std::endl;
      abort();
    }
  }

  inline void free() {
    Mix_FreeMusic(music);
    IMG_Quit();
    SDL_Quit();
  }

  inline void play_path(const char *path) {
    music = Mix_LoadMUS(path);
    if (music == nullptr) {
      std::cout << "Mix_LoadMUS failed: " << Mix_GetError() << std::endl;
      abort();
    }

    if (Mix_PlayMusic(music, -1) == -1) {
      std::cout << "Mix_PlayMusic failed: " << Mix_GetError() << std::endl;
      abort();
    }
  }
};

static sdl sdl{};
