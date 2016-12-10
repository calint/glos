#pragma once
#include "sdl.h"
//------------------------------------------------------------------------ lib
inline static void gl_request_profile_and_version(
		SDL_GLprofile prof,
		int major,
		int minor
){
	if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,prof)){
		printf("%s %d: %s",__FILE__,__LINE__,SDL_GetError());
		exit(23);
	}
	if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,major)){
		printf("%s %d: %s",__FILE__,__LINE__,SDL_GetError());
		exit(24);
	}
	if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,minor)){
		printf("%s %d: %s",__FILE__,__LINE__,SDL_GetError());
		exit(25);
	}
}

inline static void gl_print_context_profile_and_version(){
	int value;
	if(SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,&value)){
		printf("%s %d: %s",__FILE__,__LINE__,SDL_GetError());
		exit(26);
	}
	printf("%-32s","SDL_GL_CONTEXT_PROFILE_MASK");
	switch(value){
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
		printf("%s %d: %d",__FILE__,__LINE__,value);
		exit(22);
	}
	printf(" (%d)\n",value);

	if(SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,&value)){
		printf("%s %d: %s",__FILE__,__LINE__,SDL_GetError());
		exit(27);
	}
	printf("%-32s  %d\n","SDL_GL_CONTEXT_MAJOR_VERSION",value);

	if(SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,&value)){
		printf("%s %d: %s",__FILE__,__LINE__,SDL_GetError());
		exit(28);
	}
	printf("%-32s  %d\n","SDL_GL_CONTEXT_MAJOR_VERSION",value);
}
//--------------------------------------------------------------------- window
#define window_width 1024
#define window_height 1024

struct{
	SDL_Window*ref;
	SDL_Renderer*renderer;
	SDL_GLContext*glcontext;
	SDL_Surface*surface;
}window;
static inline void window_init() {
//	puts(" *** requested:");
//	gl_request_profile_and_version(SDL_GL_CONTEXT_PROFILE_ES,2,0);
	gl_request_profile_and_version(SDL_GL_CONTEXT_PROFILE_CORE,3,0);
//	puts(" ***  reading:");
//	gl_print_context_profile();

	window.ref=SDL_CreateWindow(
			"glos",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			window_width,
			window_height,
			SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
	if (!window.ref){
		printf("%s %d: %s",__FILE__,__LINE__,SDL_GetError());
		exit(2);
	}

	window.glcontext = SDL_GL_CreateContext(window.ref);
	if (!window.glcontext){
		printf("%s %d: %s",__FILE__,__LINE__,SDL_GetError());
		exit(14);
	}

	if(SDL_GL_SetSwapInterval(1)){
		printf("%s %d: %s",__FILE__,__LINE__,SDL_GetError());
		exit(29);
	}

	if(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1)){
		printf("%s %d: %s",__FILE__,__LINE__,SDL_GetError());
		exit(30);
	}
	if(SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24)){
		printf("%s %d: %s",__FILE__,__LINE__,SDL_GetError());
		exit(31);
	}
	window.surface=SDL_GetWindowSurface(window.ref);
	if (!window.surface){
		printf("%s %d: %s",__FILE__,__LINE__,SDL_GetError());
		exit(15);
	}
	window.renderer=SDL_CreateRenderer(window.ref,-1,SDL_WINDOW_OPENGL);
	if (!window.renderer){
		printf("%s %d: %s",__FILE__,__LINE__,SDL_GetError());
		exit(3);
	}
	gl_print_context_profile_and_version();
}

static inline void window_free() {
	SDL_DestroyRenderer(window.renderer);
	SDL_DestroyWindow(window.ref);
	SDL_GL_DeleteContext(window.glcontext);
}
//------------------------------------------------------------------ window
