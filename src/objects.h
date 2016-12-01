#pragma once
#include "lib.h"
#include "window.h"
#include "textures.h"
#include "object.h"

//-------------------------------------------------------------------- objects

#define objects_count 1024

static object objects[objects_count];

static object*objects_start_ptr = objects;

static object*objects_ptr = objects;

static bits objects_bits[objects_count];

static bits*objects_bits_start_ptr = objects_bits;

static bits*objects_bits_ptr = objects_bits;

static bits*objects_bits_end_ptr = objects_bits + objects_count;

#define object_bit_allocated 0

//---------------------------------------------------------------------- alloc

inline static object*object_alloc(object*initializer) {
	int i = 2;
	while (i--) {
		while (objects_bits_ptr < objects_bits_end_ptr) {
			if (bits_is_set(objects_bits_ptr, object_bit_allocated)) {
				objects_bits_ptr++;
				objects_ptr++;
				continue;
			}
			bits_set(objects_bits_ptr, 0);
			object*o = objects_ptr;
			if (initializer)
				*o = *initializer;
			else
				*o = default_object;
			if (o->init)
				o->init(o);
			o->bits = objects_bits_ptr;
			objects_bits_ptr++;
			objects_ptr++;
			return o;
		}

		objects_bits_ptr = objects_bits_start_ptr;
		objects_ptr = objects_start_ptr;
	}

	perror("out of objects");
	exit(6);
}

//------------------------------------------------------------------------ free

inline static void object_free(object*o) {
	bits_unset(o->bits, 0);
}

//----------------------------------------------------------------------- init

inline static void objects_init() {
}

//----------------------------------------------------------------------- free

inline static void objects_free() {
	object*o = objects;
	int i = objects_count;
	while (i--) {
		if (!bits_is_set(o->bits, object_bit_allocated))
			continue;
		if (o->free)
			o->free(o);

		o++;
	}
}

//--------------------------------------------------------------------- update

inline static void objects_update(float dt) {
	object*o = objects;
	int i = objects_count;
	while (i--) {
		o->position.x += o->velocity.x * dt;
		o->position.y += o->velocity.y * dt;
		o->position.z += o->velocity.z * dt;
		o->angle.x += o->angular_velocity.x * dt;
		o->angle.y += o->angular_velocity.y * dt;
		o->angle.x += o->angular_velocity.z * dt;

		if (o->update)
			o->update(o, dt);

		o++;
	}
}
//--------------------------------------------------------------------- render

inline static void objects_render() {
	object*o = objects;
	int i = objects_count;
	while (i--) {
		if (o->render)
			o->render(o);
		o++;
	}
}

//----------------------------------------------------------------------------
