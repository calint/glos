#pragma once
#include "../object.h"

//--------------------------------------------------------------------- logo_3

inline static void init_logo_3(object*o) {
	o->position = (position){100,300,0,0};
	o->velocity.x = -200;
	object_update_bounding_radius_using_scale(o);
}

//----------------------------------------------------------------------------

inline static void constrain_logo_3(object*o, dt dt) {
	if (o->position.x > 400 || o->position.x < 50)
		o->velocity.x = -o->velocity.x;
}

//----------------------------------------------------------------------------

static object default_logo_3 = {
		.type = {{'c',0,0,0,0,0,0,0}},
		.init = init_logo_3,
		.scale = { 20, 20, 20, 0 },
		.texture_id = 1,
		.update = constrain_logo_3,
		.render = _draw_texture_,
};

//----------------------------------------------------------------------------
