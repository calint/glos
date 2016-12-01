#include <SDL.h>
#include "lib.h"
#include "sprites.h"
#include "textures.h"
#include "window.h"

//--------------------------------------------------------------------- main

void print_SDL_version(char* preamble, SDL_version* v) {
	printf("%s %u.%u.%u\n", preamble, v->major, v->minor, v->patch);
}

void print_SDL_versions() {

	SDL_version ver;

	SDL_VERSION(&ver);

	print_SDL_version("SDL compile-time version", &ver);

}

int main(int argc, char *argv[]) {
	print_SDL_versions();

	if (SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Quit();
		return 1;
	}



	screen_init();

	textures_init();

	sprites_init();



	Uint32 currentFPS = 0;

	int frame_count = 0;

	Uint32 calculate_fps_every_x_ms = 1000;

	Uint32 t0 = SDL_GetTicks();

	SDL_Event event;

	int gameover = 0;

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

		SDL_RenderClear(window_free);

		sprites_render(window_free);

		SDL_RenderPresent(window_free);

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

	texture_free();

	screen_free();

	SDL_Quit();

	return 0;
}
