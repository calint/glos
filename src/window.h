#pragma once
#define GL3_PROTOTYPES 1
#include <GL/glew.h>

//------------------------------------------------------------------ window
#define window_width 512
#define window_height 512
static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_GLContext *window_glcontext;

static inline void window_init() {

	window = SDL_CreateWindow("gles", SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED, window_width,
			window_height, SDL_WINDOW_OPENGL);

	if (!window) {
		SDL_Quit();
		exit(2);
	}

	window_glcontext = SDL_GL_CreateContext(window);
	// Set our OpenGL version.
	// SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
			SDL_GL_CONTEXT_PROFILE_CORE);

	// 3.2 is part of the modern versions of OpenGL, but most video cards whould be able to run it
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	// Turn on double buffering with a 24bit Z buffer.
	// You may need to change this to 16 or 32 for your system
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GL_SetSwapInterval(1);

//		// Init GLEW
//		// Apparently, this is needed for Apple. Thanks to Ross Vander for letting me know
//		#ifndef __APPLE__
//		glewExperimental = GL_TRUE;
//		glewInit();
//		#endif

	renderer = SDL_CreateRenderer(window, -1, SDL_WINDOW_OPENGL);

	if (!renderer) {
		SDL_DestroyWindow(window);
		SDL_Quit();
		exit(3);
	}

}

static inline void window_free() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(window_glcontext);
}
//------------------------------------------------------------------ window
