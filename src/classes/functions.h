#pragma once
#include <SDL2/SDL2_gfxPrimitives.h>

//----------------------------------------------------------- object functions

inline static void _set_bounding_radius_from_xy_scale(object*o) {
	o->bounding_radius = (bounding_radius) sqrtf(
			o->scale.x * o->scale.x + o->scale.y * o->scale.y);
}

inline static void _draw_texture(object*o) {
	SDL_Rect dest = { (int) (o->position.x - o->scale.x), (int) (o->position.y
			- o->scale.y), (int) o->scale.x * 2, (int) o->scale.y * 2 };
	SDL_RenderCopy(renderer, texture[o->texture_id], NULL, &dest);
}

inline static void _draw_bounding_sphere(object*o) {
	circleColor(renderer, (short) o->position.x, (short) o->position.y,
			(short) o->bounding_radius, o->color);
}

inline static void _draw_texture_and_bounding_sphere(object*o) {
	_draw_texture(o);
	_draw_bounding_sphere(o);
}

//---------------------------------------------------------------------------
