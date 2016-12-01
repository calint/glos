#include "fps.h"
#include "sdl.h"
#include "lib.h"
#include "sprites.h"
#include "textures.h"
#include "window.h"
#include "shader.h"
//--------------------------------------------------------------------- main
int main(int argc, char *argv[]) {

	sdl_init();

	window_init();

	textures_init();

	sprites_init();

	fps_init();

	shader_init();

	SDL_Event event;

	GLclampf red = 0;

	GLclampf green = 0;

	GLclampf blue = 0;

	int running = 1;

	while (running) {

		fps_counter_before_frame();

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				running = 0;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					running = 0;
					break;
				case SDLK_w:
					red = 1;
					green = 0;
					blue = 0;
					break;
				case SDLK_a:
					red = 0;
					green = 1;
					blue = 0;
					break;
				case SDLK_s:
					red = 0;
					green = 0;
					blue = 1;
					break;
				case SDLK_d:
					red = 1;
					green = 1;
					break;
				}
			}
		}

		glClearColor(red, green, blue, 1.0);

		glClear(GL_COLOR_BUFFER_BIT);

		shader_render();
//
//		double dt = fps_dt;
//
//		sprites_update(dt);
//
//		sprites_render(renderer);

		SDL_GL_SwapWindow(window);

		fps_counter_after_frame();

	}

	fps_free();

	sprites_free();

	textures_free();

	window_free();

	sdl_free();

	return 0;

}
