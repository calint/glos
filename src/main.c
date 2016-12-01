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

//----------------------------------------------------------------------- init

inline static void init(){

	object_alloc(&_default_logo_1);
	object_alloc(&_default_logo_2);
	object_alloc(&_default_logo_3);
	object_alloc(&_default_logo_4);

}

//-------------------------------------------------------------background_color

struct {
	GLclampf red;
	GLclampf green;
	GLclampf blue;
} background_color = {0,0,0};

//------------------------------------------------------------------------ main

int main(int argc, char *argv[]) {

	//-------------------------------------------------------------------- init

	sdl_init();

	window_init();

	textures_init();

	sprites_init();

	fps_init();

	shader_init();

	objects_init();

	init();

	//-------------------------------------------------------------------- loop

	for(int running = 1; running; ) {

		fps_at_frame_start();

		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			//-------------------------------------------------------------keys
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
					background_color.red = 1;
					background_color.green = 0;
					background_color.blue = 0;
					break;
				case SDLK_a:
					background_color.red = 0;
					background_color.green = 1;
					background_color.blue = 0;
					break;
				case SDLK_s:
					background_color.red = 0;
					background_color.green = 0;
					background_color.blue = 1;
					break;
				case SDLK_d:
					background_color.red = 1;
					background_color.green = 1;
					break;
				}
			}
		}

		//---------------------------------------------------------------- draw

		glClearColor( background_color.red, background_color.green,
				background_color.blue, 1.0);

		glClear(GL_COLOR_BUFFER_BIT);

//		shader_render();

		sprites_update(fps.dt);

		sprites_render(renderer);

		objects_update(fps.dt);

		objects_render();

		SDL_GL_SwapWindow(window);

		fps_at_frame_done();

	}

	//---------------------------------------------------------------------free

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
