#pragma once
//-----------------------------------------------------------------------------
// object allocator generated from template 
//-----------------------------------------------------------------------------
#include"object.h"
#define object_count 1024
//--------------------------------------------------------------------- storage
static object objects[object_count];
static object*objects_start_ptr=objects;
static object*objects_seek_ptr=objects;
static object*objects_end_ptr=objects+object_count;
//------------------------------------------------------------------------ bits
static bits objects_bits[object_count];
static bits*objects_bits_start_ptr=objects_bits;
static bits*objects_bits_seek_ptr=objects_bits;
static bits*objects_bits_end_ptr=objects_bits+object_count;
//----------------------------------------------------------------------- alloc
inline static object*object_alloc(object*initializer){
	int iterate_to_scan_the_table=2;
	while(iterate_to_scan_the_table--){
		while(objects_bits_seek_ptr<objects_bits_end_ptr){
			if(bits_is_bit_set(objects_bits_seek_ptr,bit_object_allocated)){
				objects_bits_seek_ptr++;
				objects_seek_ptr++;
				continue;
			}
			bits_set_bit(objects_bits_seek_ptr,bit_object_allocated);
			object*o=objects_seek_ptr;
			objects_seek_ptr++;
			objects_bits_seek_ptr++;
			*o=initializer?*initializer:object_def;
			o->ptr_bits=objects_bits_seek_ptr;
			if (o->init)o->init(o);
			return o;
		}
		objects_bits_seek_ptr=objects_bits_start_ptr;
		objects_seek_ptr=objects_start_ptr;
	}
	perror("out of objects");
	exit(6);
}
//--------------------------------------------------------------------- recycle
inline static void object_free(object*o){
	bits_clear(o->ptr_bits,bit_object_allocated);
}
//-----------------------------------------------------------------------------
