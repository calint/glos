#pragma once
#include "lib.h"
#include "window.h"
#include "textures.h"

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
	type type;
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

static object _default_object = {

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

//---------------------------------------------------------------------------
