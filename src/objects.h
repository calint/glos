#pragma once
#include "lib.h"
#include "window.h"
#include "textures.h"
#include "object.h"

//---------------------------------------------------------------------- lib

inline static int is_bit_set(bits*b,int bit_number_starting_at_zero){
	return(*b&(1<<bit_number_starting_at_zero))!=0;
}

inline static void set_bit(bits*b,int bit_number_starting_at_zero){
	*b|=(bits)(1<<bit_number_starting_at_zero);
}

inline static void clear_bit(bits*b,int bit_number_starting_at_zero){
	*b&=(bits)~(1<<bit_number_starting_at_zero);
}

inline static void add_dt(vec4*this,vec4*other,dt dt){
	this->x+=other->x*dt;
	this->y+=other->y*dt;
	this->z+=other->z*dt;
}

//-------------------------------------------------------------------- objects

#define object_count 1024

static object objects[object_count];

static object*objects_start_ptr=objects;

static object*object_seek_ptr=objects;

static object*objects_end_ptr=objects+object_count;

//----------------------------------------------------------------------- bits

static bits objects_bits[object_count];

static bits*objects_bits_start_ptr=objects_bits;

static bits*object_bits_seek_ptr=objects_bits;

static bits*objects_bits_end_ptr=objects_bits+object_count;


#define bit_object_allocated 0

//------------------------------------------------------------------------ init

inline static void init_objects(){}

//------------------------------------------------------------------------ free

inline static void free_objects() {
	object*o=objects;
	while(o<objects_end_ptr){
		if(o->bits&&bits_is_set(o->bits,bit_object_allocated))
			if(o->free)o->free(o);
		o++;
	}
}

//---------------------------------------------------------------------- alloc

inline static object*alloc(object*initializer){
	int iterate_to_scan_the_table=2;
	while(iterate_to_scan_the_table--){
		while(object_bits_seek_ptr<objects_bits_end_ptr){
			if(is_bit_set(object_bits_seek_ptr,bit_object_allocated)){
//			if (bits_is_set(object_bits_seek_ptr,object_bit_allocated)){
				object_bits_seek_ptr++; // allocated, next
				object_seek_ptr++;
				continue;
			}
			set_bit(object_bits_seek_ptr,0); // allocate //? racing
//			bits_set(object_bits_seek_ptr,0); // allocate //? racing
			object*o=object_seek_ptr;
			*o=initializer?*initializer:default_object;
			if (o->init)o->init(o);
			o->bits=object_bits_seek_ptr; // used at free to
			object_bits_seek_ptr++;      // unset alloc bit
			object_seek_ptr++;
			return o;
		}
		object_bits_seek_ptr=objects_bits_start_ptr;
		object_seek_ptr=objects_start_ptr;
	}
	perror("out of objects");
	exit(6);
}

//------------------------------------------------------------------------ free

inline static void free_object(object*o){
	clear_bit(o->bits,bit_object_allocated); //? racing
}
//--------------------------------------------------------------------- update

inline static void update_objects(dt dt){
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

inline static void render_objects() {
	object*o=objects;
	while(o<objects_end_ptr){
		if (o->render)o->render(o);
		o++;
	}
}

//----------------------------------------------------------------------------
