#pragma once
#include <SDL2/SDL2_gfxPrimitives.h>

#include "functions.h"

//--------------------------------------------------------------------- logo_2

inline static void _init_logo_2(object*o) {
	o->position = (position){200,100,0,0};
	o->velocity.x = 100;
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
		.type = {{1,0,0,0,0,0,0,0}},
		.position = {0, 0, 0, 0},
		.velocity = {0, 0, 0, 0},
		.angle = {0, 0, 0, 0},
		.angular_velocity = {0, 0, 0, 0},
		.scale = { 20, 20, 20, 0 },
		.texture_id = 1,
		.bounding_radius =  28.284271f,
		.bits = 0,
		.oid=0,
		.color=0,
		.parent=0,
		.init = _init_logo_2,
		.update = _constrain_logo_2,
		.collision=NULL,
		.render = _draw_texture,
		.free=NULL,
};

//---------------------------------------------------------------------------
