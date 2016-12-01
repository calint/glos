#include "SDL.h"
#include <stdlib.h>

#define screen_width 512
#define screen_height 512

#define sprites_count 1024
static SDL_Rect sprites_pos[sprites_count];
static SDL_Surface*sprites_image[sprites_count];

static inline float random() {
	return rand() / (float) RAND_MAX;
}

static inline int randomRange(int low, int high) {
	int range = high - low;
	return (int) (random() * range) + low;
}

static inline void sprites_init() {
	SDL_Surface* temp = SDL_LoadBMP("logo.bmp");
	SDL_Surface* bg = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	for (int i = 0; i < sprites_count; i++) {
		sprites_image[i] = bg;
		sprites_pos[i].x = randomRange(0, screen_width);
		sprites_pos[i].y = randomRange(0, screen_height);
		sprites_pos[i].w = 74;
		sprites_pos[i].h = 55;
	}
}

static inline void sprites_free() {
	for (int i = 0; i < sprites_count; i++) {
		SDL_FreeSurface(sprites_image[i]);
	}
}

static inline void sprites_render(SDL_Surface*s) {
	for (int i = 0; i < sprites_count; i++) {
		SDL_BlitSurface(sprites_image[i], NULL, s, &sprites_pos[i]);
	}
}

void print_SDL_version(char* preamble, SDL_version* v) {
	printf("%s %u.%u.%u\n", preamble, v->major, v->minor, v->patch);
}

void print_SDL_versions() {
	SDL_version ver;

// Prints the compile time version
	SDL_VERSION(&ver);
	print_SDL_version("SDL compile-time version", &ver);

// Prints the run-time version
	ver = *SDL_Linked_Version();
	print_SDL_version("SDL runtime version", &ver);
}

int main(int argc, char *argv[]) {
	print_SDL_versions();

	SDL_Init(SDL_INIT_VIDEO);

	SDL_WM_SetCaption("SDL Test", "SDL Test");

	SDL_Surface* screen = SDL_SetVideoMode(screen_width, screen_height, 0, 0);

	SDL_Event event;

	int gameover = 0;

	sprites_init();

	Uint32 currentFPS = 0;

	int frame_count = 0;

	Uint32 calculate_fps_every_x_ms = 1000;

	Uint32 t0 = SDL_GetTicks();

	while (!gameover) {

		frame_count++;

		if (SDL_PollEvent(&event)) {

			switch (event.type) {

			case SDL_QUIT:
				gameover = 1;
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
				case SDLK_q:
					gameover = 1;
					break;
				}
				break;
			}
		}

		sprites_render(screen);

		SDL_UpdateRect(screen, 0, 0, 0, 0);

		Uint32 dt = SDL_GetTicks() - t0;

		if (dt < calculate_fps_every_x_ms)
			continue;

		if (dt != 0) {
			currentFPS = frame_count * 1000 / dt;
		} else {
			currentFPS = 0;
		}

		printf(" fps: %d\n", currentFPS);

		t0 = SDL_GetTicks();
	}

	sprites_free();

	/* cleanup SDL */
	SDL_Quit();

	return 0;
}
