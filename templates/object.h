//-----------------------------------------------------------------------------
#pragma once
#include"object.h"
#define object_count 1024
//--------------------------------------------------------------------- storage
static object ${nameplr}[${nameplr}_count];
static object*${nameplr}_start_ptr=${nameplr};
static object*${nameplr}_seek_ptr=${nameplr};
static object*${nameplr}_end_ptr=${nameplr}+${nameplr}_count;
//------------------------------------------------------------------------ bits
static bits ${nameplr}_bits[${nameplr}_count];
static bits*${nameplr}_bits_start_ptr=${nameplr}_bits;
static bits*${nameplr}_bits_seek_ptr=${nameplr}_bits;
static bits*${nameplr}_bits_end_ptr=${nameplr}_bits+${nameplr}_count;
//----------------------------------------------------------------------- alloc
inline static object*alloc(object*initializer){
	int iterate_to_scan_the_table=2;
	while(iterate_to_scan_the_table--){
		while(objects_bits_seek_ptr<objects_bits_end_ptr){
			if(bits_is_bit_set(object_bits_seek_ptr,bit_object_allocated)){
				object_bits_seek_ptr++; // allocated, next
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
