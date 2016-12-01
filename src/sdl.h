#pragma once
#include <SDL.h>
#include <SDL_opengles2.h>
#include <SDL_image.h>
#include <unistd.h>
#define cpu_level1_cache_line_size 64
//#define GLOS_EMBEDDED

//----------------------------------------------------------------------- init

static inline void sdl_init() {
	if (SDL_Init(SDL_INIT_VIDEO)) {
		perror("could not initialize SDL");
		SDL_Quit();
		exit(1);
	}

//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
//
//	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
//	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	int imgFlags=IMG_INIT_PNG;
	if(!(IMG_Init(imgFlags)&IMG_INIT_PNG)){
		printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
	}
}

//------------------------------------------------------------------------free

static inline void sdl_free() {
	SDL_Quit();
}

//----------------------------------------------------------------------------
