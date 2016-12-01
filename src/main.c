#include "sdl.h"
#include "fps.h"
#include "lib.h"
#include "sprites.h"
#include "textures.h"
#include "window.h"
#include "shader.h"
#include "objects.h"
//-------------------------------------------------------------- object logo
inline static void _logo_init(object*o) {
	o->pos.x = 100;
	o->pos.y = 100;
}

inline static void _logo_render(object*o) {
	SDL_Rect dest = { (int) o->pos.x, (int) o->pos.y,
			50, 50 };
	SDL_RenderCopy(renderer,texture[0], NULL, &dest);
}
//-------------------------------------------------------------- object logo2
inline static void _logo2_init(object*o) {
	o->pos.x = 200;
	o->pos.y = 200;
	o->dpos.x = 100;
}

inline static void _logo2_update(object*o,float dt) {
	if(o->pos.x>220 || o->pos.x<50)
		o->dpos.x=-o->dpos.x;
}

inline static void _logo2_render(object*o) {
	SDL_Rect dest = { (int) o->pos.x, (int) o->pos.y,
			100, 100 };
	SDL_RenderCopy(renderer,texture[0], NULL, &dest);
}
//-------------------------------------------------------------- object logo3
inline static void _logo3_init(object*o) {
	o->pos.x = 100;
	o->pos.y = 300;
	o->dpos.x = -200;
}

inline static void _logo3_update(object*o,float dt) {
	if(o->pos.x>400 || o->pos.x<50)
		o->dpos.x=-o->dpos.x;
}

inline static void _logo3_render(object*o) {
	SDL_Rect dest = { (int) o->pos.x, (int) o->pos.y,
			20, 20 };
	SDL_RenderCopy(renderer,texture[0], NULL, &dest);
}
//--------------------------------------------------------------------- main
int main(int argc, char *argv[]) {
	sdl_init();
	window_init();
	textures_init();
	sprites_init();
	fps_init();
	shader_init();
	objects_init();

	GLclampf red = 0;
	GLclampf green = 0;
	GLclampf blue = 0;

	object*o;

	o = object_alloc();
	o->init=_logo_init;
	o->render = _logo_render;
	o->init(o);

	o = object_alloc();
	o->init=_logo2_init;
	o->update =_logo2_update;
	o->render = _logo2_render;
	o->init(o);

	object_free(o);
//
//	o = object_alloc();
//	o->init=_logo3_init;
//	o->update =_logo3_update;
//	o->render = _logo3_render;
//	o->init(o);


	printf("object[%s %p}\n", o->type.path, (void*) o);

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

//		sprites_update(fps_dt);
//		sprites_render(renderer);

		float dt = (float) fps_dt;
		objects_update(dt);
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
