#pragma once
#include "functions.h"

//--------------------------------------------------------------------- logo_1

inline static void _init_logo_1(object*o) {
	o->position = (position){100,100,0,0};
	_set_bounding_radius_from_xy_scale(o);
}

//---------------------------------------------------------------------------

static object _default_logo_1 = {
		.type = {{'a',0,0,0,0,0,0,0}},
		.init = _init_logo_1,
		.scale = { 50, 50, 50, 0 },
		.texture_id = 0,
		.render = _draw_texture,
};

//---------------------------------------------------------------------------
