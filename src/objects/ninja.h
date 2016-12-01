#pragma once
#include "../object.h"

//--------------------------------------------------------------------- logo_1

inline static void init_ninja(object*o) {
	o->position = (position){100,100,0,0};
	set_bounding_radius_from_xy_scale(o);
}

//---------------------------------------------------------------------------

static object default_ninja={
		.type={{'e',0,0,0,0,0,0,0}},
		.init=init_ninja,
		.scale={50,50,50,0},
		.texture_id=10,
		.render=_draw_texture,
};

//---------------------------------------------------------------------------
