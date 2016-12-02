#pragma once
#include "../object.h"

//--------------------------------------------------------------------- logo_1

inline static void init_logo_1(object*o) {
	o->position = (position){100,100,0,0};
	update_bounding_radius_using_scale(o);
}

//---------------------------------------------------------------------------

static object default_logo_1 = {
		.type = {{'a',0,0,0,0,0,0,0}},
		.init = init_logo_1,
		.scale = { 50, 50, 50, 0 },
		.texture_id = 0,
		.render = draw_texture,
};

//---------------------------------------------------------------------------
