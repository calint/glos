#pragma once
#include"object.h"
//------------------------------------------------------------------------- new
inline static object*new(object*initializer){
	int iterate_to_scan_the_table=2;
	while(iterate_to_scan_the_table--){
		while(object_bits_seek_ptr<objects_bits_end_ptr){
			if(bits_is_bit_set(object_bits_seek_ptr,bit_object_allocated)){
				object_bits_seek_ptr++; // allocated, next
				object_seek_ptr++;
				continue;
			}
			bits_set_bit(object_bits_seek_ptr,0); // allocate //? racing
			object*o=object_seek_ptr;
			object_seek_ptr++;
			object_bits_seek_ptr++;      // unset alloc bit
			*o=initializer?*initializer:_object_;
			o->bits_ref=object_bits_seek_ptr; // used at free to
			if (o->init)o->init(o);
			return o;
		}
		object_bits_seek_ptr=objects_bits_start_ptr;
		object_seek_ptr=objects_start_ptr;
	}
	perror("out of objects");
	exit(6);
}
//---------------------------------------------------------------------- delete
inline static void delete(object*o){
	bits_clear(o->bits_ref,bit_object_allocated); //? racing
}
//-----------------------------------------------------------------------------
