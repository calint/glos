#pragma once
#include <SDL.h>
//--------------------------------------------------------------------- sdl
static inline void sdl_init() {
	if (SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Quit();
		exit(1);
	}
}

static inline void sdl_free() {
	SDL_Quit();
}
//--------------------------------------------------------------------- sdl
