#pragma once
#include "sdl.h"
//------------------------------------------------------------------ window
#define window_width 512
#define window_height 512
struct{
	SDL_Window*ptr;
	SDL_Renderer*renderer;
	SDL_GLContext*glcontext;
	SDL_Surface*surface;
}window;

static inline void window_init() {

	window.ptr=SDL_CreateWindow(
			"gles",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			window_width,
			window_height,
			SDL_WINDOW_OPENGL
		);

	if (!window.ptr){
		perror("could not create window");
		printf("%s %d: %s",__FILE__,__LINE__,"");
		SDL_Quit();
		exit(2);
	}

//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);

	window.glcontext = SDL_GL_CreateContext(window.ptr);
	if (!window.glcontext){
		perror("could not create opengl context");
		printf("%s %d: %s",__FILE__,__LINE__,"");
		SDL_DestroyWindow(window.ptr);
		SDL_Quit();
		exit(14);
	}

	window.surface=SDL_GetWindowSurface(window.ptr);
	if (!window.surface){
		perror("could not create surface");
		printf("%s %d: %s",__FILE__,__LINE__,"");
		SDL_DestroyWindow(window.ptr);
		SDL_Quit();
		exit(15);
	}

//	// Set our OpenGL version.
//	// SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
//			SDL_GL_CONTEXT_PROFILE_CORE);
//
//	// 3.2 is part of the modern versions of OpenGL, but most video cards whould be able to run it
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

// Turn on double buffering with a 24bit Z buffer.
// You may need to change this to 16 or 32 for your system
//	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GL_SetSwapInterval(1);

//		// Init GLEW
//		// Apparently, this is needed for Apple. Thanks to Ross Vander for letting me know
//		#ifndef __APPLE__
//		glewExperimental = GL_TRUE;
//		glewInit();
//		#endif

	window.renderer=SDL_CreateRenderer(window.ptr,-1,SDL_WINDOW_OPENGL);
	if (!window.renderer){
		perror("could not create renderer");
		SDL_DestroyWindow(window.ptr);
		SDL_Quit();
		exit(3);
	}

}

static inline void window_free() {
	SDL_DestroyRenderer(window.renderer);
	SDL_DestroyWindow(window.ptr);
	SDL_GL_DeleteContext(window.glcontext);
}
//------------------------------------------------------------------ window
