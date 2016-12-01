#pragma once
#include "functions.h"

//--------------------------------------------------------------------- logo_4

inline static void _init_logo_4(object*o) {
	o->position = (position){100,400,0,0};
	o->color = 0xff008000;
	o->texture_id = 1;
	_set_bounding_radius_from_xy_scale(o);
}

//---------------------------------------------------------------------------

static object _default_logo_4 = {
		.init = _init_logo_4,
		.scale = { 40, 40, 40, 0 },
		.texture_id = 1,
		.render = _draw_texture_and_bounding_sphere,
};

//---------------------------------------------------------------------------
