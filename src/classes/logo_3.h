#pragma once
#include <SDL2/SDL2_gfxPrimitives.h>

#include "functions.h"

//--------------------------------------------------------------------- logo_3

inline static void _init_logo_3(object*o) {
	o->position = (position){100,300,0,0};
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

//--------------------------------------------------------------------------
