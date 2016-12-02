#pragma once
#include <SDL.h>
#include <SDL_opengles2.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
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

	if(!(IMG_Init(IMG_INIT_PNG)&IMG_INIT_PNG)){
		puts("could not initialize IMG_Init");
		printf("%s %d: %s",__FILE__,__LINE__,IMG_GetError());
	}

	if(!(TTF_Init())){
		puts("could not initialize TTF_Init");
		printf("%s %d: %s",__FILE__,__LINE__,IMG_GetError());
	}
}

//------------------------------------------------------------------------free

static inline void sdl_free(){
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

//----------------------------------------------------------------------------
