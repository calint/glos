#include "sdl.h"
#include "fps.h"
#include "lib.h"
#include "sprites.h"
#include "textures.h"
#include "window.h"
#include "shader.h"
#include "objects.h"

//-------------------------------------------------------------------- include

#include "classes/logo_1.h"
#include "classes/logo_2.h"
#include "classes/logo_3.h"
#include "classes/logo_4.h"

//--------------------------------------------------------------------- create
inline static void init(){
	object_alloc(&_default_logo_1);
	object_alloc(&_default_logo_2);
	object_alloc(&_default_logo_3);
	object_alloc(&_default_logo_4);
}

//----------------------------------------------------------------------- main
int main(int argc, char *argv[]) {

	sdl_init();

	window_init();

	textures_init();

	sprites_init();

	fps_init();

	shader_init();

	objects_init();

	init();

	GLclampf red = 0;
	GLclampf green = 0;
	GLclampf blue = 0;

	int running = 1;

	SDL_Event event;
	while (running) {

		fps_counter_at_frame_start();

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

//		shader_render();

//		sprites_update(fps.dt);
//		sprites_render(renderer);

		objects_update(fps.dt);
		objects_render();

		SDL_GL_SwapWindow(window);

		fps_counter_at_frame_done();
	}

	objects_free();
	shader_free();
	fps_free();
	sprites_free();
	textures_free();
	window_free();
	sdl_free();
	return 0;
}

//----------------------------------------------------------------------------
