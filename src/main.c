#include "sdl.h"
#include "fps.h"
#include "lib.h"
#include "sprites.h"
#include "textures.h"
#include "window.h"
#include "shader.h"
#include "objects.h"
//-------------------------------------------------------------- lib
inline static bounding_radius bounding_radius_for_scale(scale*s) {
	return (bounding_radius) sqrtf(s->x * s->x + s->y * s->y + s->z * s->z);
}

inline static void _render_texture(object*o) {
	SDL_Rect dest = { (int) o->position.x, (int) o->position.y,
			(int) o->scale.x, (int) o->scale.y };
	SDL_RenderCopy(renderer, texture[o->texture_id], NULL, &dest);
}
//-------------------------------------------------------------- object logo
inline static void _logo_init(object*o) {
	o->position.x = 100;
	o->position.y = 100;
	o->scale = (scale ) { 50, 50, 50, 0 };
	o->bouning_radius = bounding_radius_for_scale(&o->scale);
	o->texture_id = 0;
}
//-------------------------------------------------------------- object logo2
inline static void _logo2_init(object*o) {
	o->position.x = 200;
	o->position.y = 200;
	o->velocity.x = 100;
	o->scale = (scale ) { 20, 20, 20, 0 };
	o->texture_id = 1;
}

inline static void _constrain(object*o, float dt) {
	if (o->position.x > 220){
		o->position.x=220;
		o->velocity.x = -o->velocity.x;
	}
	if(o->position.x < 50){
		o->position.x=50;
		o->velocity.x = -o->velocity.x;
	}
}

inline static void _logo2_render(object*o) {
	SDL_Rect dest = { (int) o->position.x, (int) o->position.y, 100, 100 };
	SDL_RenderCopy(renderer, texture[o->texture_id], NULL, &dest);
}
//-------------------------------------------------------------- object logo3
inline static void _logo3_init(object*o) {
	o->position.x = 100;
	o->position.y = 300;
	o->velocity.x = -200;
}

inline static void _logo3_update(object*o, float dt) {
	if (o->position.x > 400 || o->position.x < 50)
		o->velocity.x = -o->velocity.x;
}

inline static void _logo3_render(object*o) {
	SDL_Rect dest = { (int) o->position.x, (int) o->position.y, 20, 20 };
	SDL_RenderCopy(renderer, texture[0], NULL, &dest);
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

//	o = object_alloc();
//	o->init = _logo_init;
//	o->render = _logo_render;
//	o->init(o);

	o = object_alloc();
	o->init = _logo2_init;
	o->update = _constrain;
	o->render = _render_texture;
	o->init(o);

//	object_free(o);
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
