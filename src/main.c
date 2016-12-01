#include "sdl.h"
#include "fps.h"
#include "lib.h"
#include "sprites.h"
#include "textures.h"
#include "window.h"
#include "shader.h"
#include "objects.h"
#include <SDL2/SDL2_gfxPrimitives.h>
//-------------------------------------------------------------- lib
inline static void _set_bounding_radius_from_xy_scale(object*o) {
	o->bouning_radius = (bounding_radius) sqrtf(
			o->scale.x * o->scale.x + o->scale.y * o->scale.y);
}

inline static void _draw_texture(object*o) {
	SDL_Rect dest = { (int) (o->position.x - o->scale.x), (int) (o->position.y
			- o->scale.y), (int) o->scale.x * 2, (int) o->scale.y * 2 };
	SDL_RenderCopy(renderer, texture[o->texture_id], NULL, &dest);
}

inline static void _draw_bounding_sphere(object*o) {
	circleColor(renderer, (short) o->position.x, (short) o->position.y,
			(short) o->bouning_radius, o->color);
}

inline static void _draw_texture_and_bounding_sphere(object*o) {
	_draw_texture(o);
	_draw_bounding_sphere(o);
}

//-------------------------------------------------------------- object logo 1
inline static void _init_logo_1(object*o) {
	o->position.x = 100;
	o->position.y = 100;
	_set_bounding_radius_from_xy_scale(o);
}

static object _default_logo_1 = { .scale = { 50, 50, 50, 0 }, .texture_id = 0,
		.init = _init_logo_1, .render = _draw_texture };
//-------------------------------------------------------------- object logo 2
inline static void _init_logo_2(object*o) {
	o->position.x = 200;
	o->position.y = 200;
	o->velocity.x = 100;
	_set_bounding_radius_from_xy_scale(o);
}

inline static void _constrain_logo_2(object*o, float dt) {
	if (o->position.x > 220) {
		o->position.x = 220;
		o->position.y += o->scale.y;
		o->velocity.x = -o->velocity.x;
	}
	if (o->position.x < 50) {
		o->position.x = 50;
		o->velocity.x = -o->velocity.x;
	}
	if (o->position.y > 400) {
		o->position.y = 400;
		o->velocity.y = o->velocity.x;
		o->velocity.x = 0;
	}
	if (o->position.y < 50) {
		o->position.y = 50;
		o->velocity.x = o->velocity.y;
	}
}

static object _default_logo_2 = { .scale = { 20, 20, 20, 0 }, .texture_id = 1,
		.init = _init_logo_2, .update = _constrain_logo_2, .render =
				_draw_texture };

//-------------------------------------------------------------- object logo 3
inline static void _init_logo_3(object*o) {
	o->position.x = 100;
	o->position.y = 300;
	o->velocity.x = -200;
	_set_bounding_radius_from_xy_scale(o);
}

inline static void _constrain_logo_3(object*o, float dt) {
	if (o->position.x > 400 || o->position.x < 50)
		o->velocity.x = -o->velocity.x;
}

static object _default_logo_3 = { .scale = { 20, 20, 20, 0 }, .texture_id = 1,
		.init = _init_logo_3, .update = _constrain_logo_3, .render =
				_draw_texture };
//-------------------------------------------------------------- object logo 4
inline static void _init_logo_4(object*o) {
	o->position.x = 100;
	o->position.y = 400;
	o->scale = (scale ) { 40, 40, 40, 0 };
	o->color = 0xff008000;
	o->texture_id = 1;
	_set_bounding_radius_from_xy_scale(o);
}

static object _default_logo_4 = { .scale = { 40, 40, 40, 0 }, .texture_id = 1,
		.init = _init_logo_4, .render = _draw_texture_and_bounding_sphere };
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
	*o = _default_logo_1;
	o->init(o);

	o = object_alloc();
	*o = _default_logo_2;
	o->init(o);

//	object_free(o);
//
	o = object_alloc();
	*o = _default_logo_3;
	o->init(o);

	o = object_alloc();
	*o = _default_logo_4;
	o->init(o);

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
