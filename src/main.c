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

	int gameover = 0;

	while (!gameover) {

		fps_counter_before_frame();

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

		fps_counter_after_frame();
	}

	sprites_free();

	texture_free();

	screen_free();

	fps_counter_free();

	sdl_free();

	return 0;
}
