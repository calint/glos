#pragma once
#include "../object.h"

//--------------------------------------------------------------------- logo_4

inline static void init_logo_4(object*o) {
	o->position = (position){100,400,0,0};
	o->color = 0xff008000;
	o->texture_id = 1;
	set_bounding_radius_from_xy_scale(o);
}

//---------------------------------------------------------------------------

static object default_logo_4 = {
		.type = {{'d',0,0,0,0,0,0,0}},
		.init = init_logo_4,
		.scale = { 40, 40, 40, 0 },
		.texture_id = 1,
		.render = _draw_texture_and_bounding_sphere,
};

//---------------------------------------------------------------------------
