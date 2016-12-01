#pragma once
#include "lib.h"
#include "window.h"
#include "textures.h"
#include <SDL2/SDL2_gfxPrimitives.h>

//--------------------------------------------------------------------- object

typedef struct object {

	position position;
	velocity velocity;
	angle angle;
	angular_velocity angular_velocity;
	scale scale;
	id texture_id;
	bounding_radius bounding_radius;
	bits*bits;
	class type;
	id oid;
	color color;
	struct object*parent;

	void (*init)(struct object*);
	void (*update)(struct object*, dt);
	void (*collision)(struct object*, struct object*, dt);
	void (*render)(struct object*);
	void (*free)(struct object*);

} object;

//-------------------------------------------------------------------- default

static object default_object = {

		.position = {0, 0, 0, 0},
		.velocity = {0, 0, 0, 0},
		.angle = {0, 0, 0, 0},
		.angular_velocity = {0, 0, 0, 0},
		.scale = { 0, 0, 0, 0 },
		.texture_id = 0,
		.bounding_radius = 0,
		.bits = 0,
		.type = {{0,0,0,0,0,0,0,0}},
		.oid=0,
		.color=0,
		.parent=0,

		.init=NULL,
		.update=NULL,
		.collision=NULL,
		.render=NULL,
		.free=NULL,

};

//----------------------------------------------------------- ------ functions

inline static void set_bounding_radius_from_xy_scale(object*o) {
	o->bounding_radius=(bounding_radius)sqrtf(
			o->scale.x*o->scale.x+o->scale.y*o->scale.y
		);
}

//----------------------------------------------------------------------------

inline static void _draw_texture(object*o) {

	SDL_Rect dest={
			(int)(o->position.x-o->scale.x),
			(int)(o->position.y-o->scale.y),
			(int)o->scale.x*2,
			(int)o->scale.y*2
	}	;

	SDL_RenderCopy(window.renderer,texture[o->texture_id].ptr,NULL,&dest);
}

//----------------------------------------------------------------------------

inline static void _draw_bounding_sphere(object*o) {

	circleColor(
			window.renderer,
			(short)o->position.x,
			(short)o->position.y,
			(short)o->bounding_radius,
			o->color
		);

}

//----------------------------------------------------------------------------

inline static void _draw_texture_and_bounding_sphere(object*o) {
	_draw_texture(o);
	_draw_bounding_sphere(o);
}

//----------------------------------------------------------------------------
