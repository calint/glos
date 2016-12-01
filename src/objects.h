#pragma once
#include "types.h"
#include "window.h"
#include "textures.h"
//--------------------------------------------------------------------- object
typedef struct type {
	char path[8];
} type;

typedef int oid;
typedef unsigned int bits;

inline static int bits_is_set(bits*b, int bit_number_starting_at_zero) {
	bits bb = (bits) (1 << bit_number_starting_at_zero);
	return (*b & bb) != 0;
}

inline static void bits_set(bits*b, int bit_number_starting_at_zero) {
	bits bb = (bits) (1 << bit_number_starting_at_zero);
	*b |= bb;
}

inline static void bits_unset(bits*b, int bit_number_starting_at_zero) {
	bits bb = (bits) (1 << bit_number_starting_at_zero);
	*b &= (bits) ~bb;
}

typedef struct object {
	position position;
	velocity velocity;
	angle angle;
	angular_velocity angular_velocity;

	scale scale;
	int texture_id;

	bounding_radius bouning_radius;
	bits*bits;
	type type;
	oid oid;
	struct object*parent;
	void (*init)(struct object*);
	void (*update)(struct object*, float);
	void (*collision)(struct object*, struct object*, float);
	void (*render)(struct object*);
	void (*free)(struct object*);

} object;

//--------------------------------------------------------------------- objects
#define objects_count 2

static object objects[objects_count];

static object*objects_start_ptr = objects;
static object*objects_ptr = objects;

static bits objects_bits[objects_count];

static bits*objects_bits_start_ptr = objects_bits;
static bits*objects_bits_ptr = objects_bits;
static bits*objects_bits_end_ptr = objects_bits + objects_count * sizeof(bits);

#define object_bit_allocated 0

//--------------------------------------------------------------------- alloc
inline static object*object_alloc() {
	while (objects_bits_ptr < objects_bits_end_ptr) {
		if (bits_is_set(objects_bits_ptr, object_bit_allocated)) {
			objects_bits_ptr++;
			objects_ptr++;
			continue;
		}
		bits_set(objects_bits_ptr, 0);
		object*ret = objects_ptr;
		memset(ret, 0, sizeof(object));
		ret->bits = objects_bits_ptr;
		objects_bits_ptr++;
		objects_ptr++;
		return ret;
	}

	objects_bits_ptr = objects_bits_start_ptr;
	objects_ptr = objects_start_ptr;

	while (objects_bits_ptr < objects_bits_end_ptr) {
		if (bits_is_set(objects_bits_ptr, object_bit_allocated)) {
			objects_bits_ptr++;
			objects_ptr++;
			continue;
		}
		bits_set(objects_bits_ptr, 0);
		object*ret = objects_ptr;
		memset(ret, 0, sizeof(object));
		ret->bits = objects_bits_ptr;
		objects_bits_ptr++;
		objects_ptr++;
		return ret;
	}
	perror("out of objects");
	exit(6);
}
//--------------------------------------------------------------------- free
inline static void object_free(object*o) {
	bits_unset(o->bits, 0);
}
//--------------------------------------------------------------------- init
inline static void objects_init() {
	object*o = objects;
	int i = objects_count;
	while (i--) {
		if (o->init)
			o->init(o);
		o++;
	}
}
//--------------------------------------------------------------------- free
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
//--------------------------------------------------------------------- done
