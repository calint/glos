#pragma once
//-----------------------------------------------------------------------------
// object allocator generated from template ... do not modify
//-----------------------------------------------------------------------------
#include"object.h"
#include"part.h"
#define object_count 1024
#define object_assert_free
#define object_assert_bounds
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

			// *** critical begin
			if(*objects_bits_seek_ptr&1){// is allocated
				objects_bits_seek_ptr++;
				objects_seek_ptr++;
				continue;
			}
			*objects_bits_seek_ptr=1;// allocate
			// *** critical end

			object*o=objects_seek_ptr++;
			*o=initializer?*initializer:object_def;
			o->ptr_to_bits=objects_bits_seek_ptr++;

			if (o->init)
				o->init(o);

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
#ifdef object_assert_free
	if(*o->ptr_to_bits&2){
		fprintf(stderr,"\nobject %p already deleted\n",(void*)o);
		fprintf(stderr,"\n\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
		exit(-1);
	}
#endif
	*o->ptr_to_bits=2;// flag not allocated and deleted
}
//------------------------------------------------------------------- accessors

inline static void _object_assert_bounds(arrayix i){
	if(i<object_count)
		return;
	fprintf(stderr,"\nobject index %lu out of bounds %lu\n",
			i,(arrayix)object_count);
	fprintf(stderr,"\n\tfile: '%s'  line: %d\n\n",__FILE__,__LINE__);
	exit(-1);
}

inline static object*object_at(arrayix i){
#ifdef object_assert_bounds
	_object_assert_bounds(i);
#endif
	return&objects[i];
}

inline static const object*object_at_const(arrayix i){
	return&objects[i];
}

inline static void objects_free() {
	object*o=objects;
	while(o<objects_end_ptr){
		if(!o->ptr_to_bits){
			o++;
			continue;
		}

		{/*** critical ****/
		if(*o->ptr_to_bits&1){
			*o->ptr_to_bits&=(unsigned char)~(1|2);
		}else{
			o++;
			continue;
		}
		/*** critical done ****/}

		if(o->free)
			o->free(o);

		for(int i=0;i<object_part_cap;i++){
			if(!o->part[i])
				continue;
			part*p=(part*)o->part[i];
			if(p->free)
				p->free(o,p);
		}

		o++;
	}
}
