#include "sdl.h"
#include "lib.h"
#include "sprites.h"
#include "textures.h"
#include "window.h"
#include "fps_counter.h"
//--------------------------------------------------------------------- main
int main(int argc, char *argv[]) {

	sdl_init();

	screen_init();

	textures_init();

	sprites_init();

	fps_counter_init();

	SDL_Event event;

	int running = 1;

	while (running) {

		fps_counter_before_frame();

		if (SDL_PollEvent(&event)) {

			switch (event.type) {

			case SDL_QUIT:
				running = 0;
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
				case SDLK_q:
					running = 0;
					break;
				}
				break;
			}
		}

		SDL_RenderClear(renderer);

//		SDL_RenderCopy(renderer, sprite[0].img, NULL, NULL);

		sprites_render(renderer);

		SDL_RenderPresent(renderer);

		fps_counter_after_frame();
	}

	sprites_free();

	texture_free();

	screen_free();

	fps_counter_free();

	sdl_free();

	return 0;
}
