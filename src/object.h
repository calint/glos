#pragma once
#include "lib.h"
#include "window.h"
#include "textures.h"
#include <SDL2/SDL2_gfxPrimitives.h>

//--------------------------------------------------------------------- object

typedef struct object{

	position position;

	velocity velocity;

	angle angle;

	angular_velocity angular_velocity;

	bounding_radius bounding_radius;

	scale scale;

	class type;

	id id;

	id texture_id;

	color color;

	bits*bits;


	void(*init)(struct object*);

	void(*update)(struct object*,dt);

	void(*collision)(struct object*,struct object*,dt);

	void(*render)(struct object*);

	void(*free)(struct object*);

	void*extension;

}object;

//-------------------------------------------------------------------- default

static object default_object={
	.position={0,0,0,0},
	.velocity={0,0,0,0},
	.angle={0,0,0,0},
	.angular_velocity={0,0,0,0},
	.scale={0,0,0,0},
	.texture_id=0,
	.bounding_radius=0,
	.bits=0,
	.type={{0,0,0,0,0,0,0,0}},
	.id=0,
	.color=0,
	.init=NULL,
	.update=NULL,
	.collision=NULL,
	.render=NULL,
	.free=NULL,
};

//----------------------------------------------------------- ------ functions

inline static void set_bounding_radius_using_scale(object*o) {
	o->bounding_radius=(bounding_radius)
		sqrtf(o->scale.x*o->scale.x+o->scale.y*o->scale.y);
}

//----------------------------------------------------------------------------

inline static void draw_texture(object*o) {
	SDL_Rect d={
		(int)(o->position.x-o->scale.x),
		(int)(o->position.y-o->scale.y),
		(int)o->scale.x*2,
		(int)o->scale.y*2
	};
	SDL_RenderCopy(window.renderer,texture[o->texture_id].ptr,NULL,&d);
}

//----------------------------------------------------------------------------

inline static void draw_bounding_sphere(object*o) {
	circleColor(
		window.renderer,
		(short)o->position.x,
		(short)o->position.y,
		(short)o->bounding_radius,
		o->color
	);
}

//----------------------------------------------------------------------------

inline static void draw_texture_and_bounding_sphere(object*o) {
	draw_texture(o);
	draw_bounding_sphere(o);
}

//----------------------------------------------------------------------------
