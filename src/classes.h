#pragma once
#include <SDL2/SDL2_gfxPrimitives.h>

//----------------------------------------------------------- object functions

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

//--------------------------------------------------------------------- logo_1

inline static void _init_logo_1(object*o) {
	o->position.x = 100;
	o->position.y = 100;
	_set_bounding_radius_from_xy_scale(o);
}

static object _default_logo_1 = {
		.init = _init_logo_1,
		.scale = { 50, 50, 50, 0 },
		.texture_id = 0,
		.render = _draw_texture,
};

//--------------------------------------------------------------------- logo_2

inline static void _init_logo_2(object*o) {
	o->position.x = 200;
	o->position.y = 200;
	o->velocity.x = 100;
	_set_bounding_radius_from_xy_scale(o);
}

inline static void _constrain_logo_2(object*o, dt dt) {
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

static object _default_logo_2 = {
		.init = _init_logo_2,
		.scale = { 20, 20, 20, 0 },
		.update = _constrain_logo_2,
		.texture_id = 1,
		.render = _draw_texture,
};

//--------------------------------------------------------------------- logo_2

inline static void _init_logo_3(object*o) {
	o->position.x = 100;
	o->position.y = 300;
	o->velocity.x = -200;
	_set_bounding_radius_from_xy_scale(o);
}

inline static void _constrain_logo_3(object*o, dt dt) {
	if (o->position.x > 400 || o->position.x < 50)
		o->velocity.x = -o->velocity.x;
}

static object _default_logo_3 = {
		.init = _init_logo_3,
		.scale = { 20, 20, 20, 0 },
		.texture_id = 1,
		.update = _constrain_logo_3,
		.render = _draw_texture,
};

//--------------------------------------------------------------------- logo_4

inline static void _init_logo_4(object*o) {
	o->position.x = 100;
	o->position.y = 400;
	o->scale = (scale ) { 40, 40, 40, 0 };
	o->color = 0xff008000;
	o->texture_id = 1;
	_set_bounding_radius_from_xy_scale(o);
}

static object _default_logo_4 = {
		.init = _init_logo_4,
		.scale = { 40, 40, 40, 0 },
		.texture_id = 1,
		.render = _draw_texture_and_bounding_sphere,
};

//---------------------------------------------------------------------------
//
//.                                                    page 1
