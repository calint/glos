#pragma once
#include"object.h"
#define objects_count 1024

static object objects[objects_count];
static object*objects_start_ptr=objects;
static object*objects_seek_ptr=objects;
static object*objects_end_ptr=objects+objects_count;
//----------------------------------------------------------------------- bits
static bits objects_bits[objects_count];
static bits*objects_bits_start_ptr=objects_bits;
static bits*objects_bits_seek_ptr=objects_bits;
static bits*objects_bits_end_ptr=objects_bits+objects_count;
//------------------------------------------------------------------------- new
inline static object*alloc(object*initializer){
	int iterate_to_scan_the_table=2;
	while(iterate_to_scan_the_table--){
		while(objects_bits_seek_ptr<objects_bits_end_ptr){
			if(bits_is_bit_set(objects_bits_seek_ptr,bit_object_allocated)){
				objects_bits_seek_ptr++; // allocated, next
				objects_seek_ptr++;
				continue;
			}
			bits_set_bit(objects_bits_seek_ptr,0); // allocate //? racing
			object*o=objects_seek_ptr;
			objects_seek_ptr++;
			objects_bits_seek_ptr++;      // unset alloc bit
			*o=initializer?*initializer:object_def;
			o->bits_ref=objects_bits_seek_ptr; // used at free to
			if (o->init)o->init(o);
			return o;
		}
		objects_bits_seek_ptr=objects_bits_start_ptr;
		objects_seek_ptr=objects_start_ptr;
	}
	perror("out of objects");
	exit(6);
}
//---------------------------------------------------------------------- delete
inline static void delete(object*o){
	bits_clear(o->bits_ref,bit_object_allocated); //? racing
}
//-----------------------------------------------------------------------------
