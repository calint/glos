#pragma once
//------------------------------------------------------------------ window
#define window_width 512
#define window_height 512
static SDL_Window *window;
static SDL_Renderer *window_free;
static inline void screen_init() {

	window = SDL_CreateWindow("Hello World!", SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED, window_width,
			window_height, SDL_WINDOW_SHOWN);

	if (!window) {
		SDL_Quit();
		exit(2);
	}

	window_free = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (!window_free) {
		SDL_DestroyWindow(window);
		SDL_Quit();
		exit(3);
	}

}

static inline void screen_free() {
	SDL_DestroyRenderer(window_free);
	SDL_DestroyWindow(window);
}
//------------------------------------------------------------------ window
