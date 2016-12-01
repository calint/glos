#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>
#include <unistd.h>
//#define GLOS_EMBEDDED
#define cpu_level1_cache_line_size 64
//--------------------------------------------------------------------- sdl
static inline void sdl_init() {
	if (SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Quit();
		exit(1);
	}

//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
//
//	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
//	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

}

static inline void sdl_free() {
	SDL_Quit();
}
//--------------------------------------------------------------------- sdl
