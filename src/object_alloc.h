#pragma once
//-----------------------------------------------------------------------------
// object allocator generated from template ... do not modify
//-----------------------------------------------------------------------------
#include"object.h"
#define object_count 1024
#define object_free_assert_not_deleted 1
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
			if(*objects_bits_seek_ptr&1){
				objects_bits_seek_ptr++;
				objects_seek_ptr++;
				continue;
			}
			*objects_bits_seek_ptr|=1;
			object*o=objects_seek_ptr;
			objects_seek_ptr++;
			objects_bits_seek_ptr++;
			*o=initializer?*initializer:object_def;
			o->ptr_to_bits=objects_bits_seek_ptr;
			if (o->init)o->init(o);
			return o;
		}
		objects_bits_seek_ptr=objects_bits_start_ptr;
		objects_seek_ptr=objects_start_ptr;
	}
	perror("out of objects");
	exit(6);
}
//------------------------------------------------------------------------ free
inline static void object_free(object*o){
#ifdef object_free_assert_not_deleted
	if((*o->ptr_to_bits|2)==2){ // is flag deleted
		fprintf(stderr,"\nobject-already-deleted");
		fprintf(stderr,"\tfile: '%s'  line: %d\n%p\n",
				__FILE__,__LINE__,
				(void*)o);
		exit(-1);
	}
#endif
	*o->ptr_to_bits=2;// flag not allocated and deleted
}
//------------------------------------------------------------------- accessors
inline static object*object_at(arrayix i){return&objects[i];}
inline static const object*object_at_const(arrayix i){return&objects[i];}
//-----------------------------------------------------------------------------
