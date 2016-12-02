#pragma once
#include "sdl.h"
//------------------------------------------------------------------ window
#define window_width 1024
#define window_height 1024

struct{

	SDL_Window*ptr;

	SDL_Renderer*renderer;

	SDL_GLContext*glcontext;

	SDL_Surface*surface;

}window;
static inline void init_window() {

	window.ptr=SDL_CreateWindow(
			"glos",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			window_width,
			window_height,
			SDL_WINDOW_OPENGL);

	if (!window.ptr){
		printf("%s %d: %s",__FILE__,__LINE__,SDL_GetError());
		exit(2);
	}

	window.glcontext = SDL_GL_CreateContext(window.ptr);
	if (!window.glcontext){
		printf("%s %d: %s",__FILE__,__LINE__,SDL_GetError());
		exit(14);
	}

	window.surface=SDL_GetWindowSurface(window.ptr);
	if (!window.surface){
		printf("%s %d: %s",__FILE__,__LINE__,SDL_GetError());
		exit(15);
	}

	window.renderer=SDL_CreateRenderer(window.ptr,-1,SDL_WINDOW_OPENGL);
	if (!window.renderer){
		printf("%s %d: %s",__FILE__,__LINE__,SDL_GetError());
		exit(3);
	}

	SDL_GL_SetSwapInterval(1);
}

static inline void window_free() {
	SDL_DestroyRenderer(window.renderer);
	SDL_DestroyWindow(window.ptr);
	SDL_GL_DeleteContext(window.glcontext);
}
//------------------------------------------------------------------ window
