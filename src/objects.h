#pragma once
#include "lib.h"
#include "window.h"
#include "textures.h"
#include "object.h"

//-------------------------------------------------------------------- objects

#define object_count 1024

static object objects[object_count];

static object*objects_start_ptr=objects;

static object*objects_ptr=objects;

static object*objects_end_ptr=objects+object_count;


static bits objects_bits[object_count];

static bits*objects_bits_start_ptr=objects_bits;

static bits*objects_bits_ptr=objects_bits;

static bits*objects_bits_end_ptr=objects_bits+object_count;


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

inline static void object_free(object*o){bits_unset(o->bits,0);}

//----------------------------------------------------------------------- init

inline static void objects_init(){}

//----------------------------------------------------------------------- free

inline static void objects_free() {
	object*o = objects;
	int i=object_count;
	while(i--){
		if(o->bits&&bits_is_set(o->bits,object_bit_allocated))
			if(o->free)
				o->free(o);
		o++;
	}
}

//--------------------------------------------------------------------- update

inline static void add_dt(vec4*this,vec4*other,dt dt){
	this->x+=other->x*dt;
	this->y+=other->y*dt;
	this->z+=other->z*dt;
}

inline static void objects_update(dt dt){
	object*o=objects;
	while(o<objects_end_ptr){
		add_dt(&o->position,&o->velocity,dt);
		add_dt(&o->angle,&o->angular_velocity,dt);
		if (o->update)
			o->update(o, dt);
		o++;
	}
}
//--------------------------------------------------------------------- render

inline static void objects_render() {
	object*o = objects;
	int i = object_count;
	while (i--) {
		if (o->render)
			o->render(o);
		o++;
	}
}

//----------------------------------------------------------------------------
