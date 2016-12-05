//-----------------------------------------------------------------------------
#pragma once
#include"object.h"
#define object_count 1024
//--------------------------------------------------------------------- storage
static object objects[objects_count];
static object*objects_start_ptr=objects;
static object*objects_seek_ptr=objects;
static object*objects_end_ptr=objects+objects_count;
//------------------------------------------------------------------------ bits
static bits objects_bits[objects_count];
static bits*objects_bits_start_ptr=objects_bits;
static bits*objects_bits_seek_ptr=objects_bits;
static bits*objects_bits_end_ptr=objects_bits+objects_count;
//----------------------------------------------------------------------- alloc
inline static object*alloc(object*initializer){
	int iterate_to_scan_the_table=2;
	while(iterate_to_scan_the_table--){
		while(objects_bits_seek_ptr<objects_bits_end_ptr){
			if(bits_is_bit_set(object_bits_seek_ptr,bit_object_allocated)){
				object_bits_seek_ptr++;
				object_seek_ptr++;
				continue;
			}
			bits_set_bit(object_bits_seek_ptr,bit_object_allocated);
			object*o=object_seek_ptr;
			object_seek_ptr++;
			object_bits_seek_ptr++;
			*o=initializer?*initializer:object_def;
			o->ptr_bits=object_bits_seek_ptr;
			if (o->init)o->init(o);
			return o;
		}
		object_bits_seek_ptr=object_bits_start_ptr;
		object_seek_ptr=object_start_ptr;
	}
	perror("out of objects");
	exit(6);
}
//--------------------------------------------------------------------- recycle
inline static void recycle(object*o){
	bits_clear(o->ptr_bits,bit_object_allocated);
}
//-----------------------------------------------------------------------------
