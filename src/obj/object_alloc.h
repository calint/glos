#pragma once
//-----------------------------------------------------------------------------
// object allocator generated from template ... do not modify
//-----------------------------------------------------------------------------
#include"object.h"
#include"part.h"
#define object_cap 1024*8
#define object_assert_free
#define object_assert_bounds
#define object_assert_cast
//--------------------------------------------------------------------- storage
static object objects[object_cap];
static object*objects_start_ptr=objects;
static object*objects_seek_ptr=objects;
static object*objects_end_ptr=objects+object_cap;
//------------------------------------------------------------------------ bits
static bits objects_bits[object_cap];
static bits*objects_bits_start_ptr=objects_bits;
static bits*objects_bits_seek_ptr=objects_bits;
static bits*objects_bits_end_ptr=objects_bits+object_cap;
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
			metrics.objects_allocated++;
			object*o=objects_seek_ptr++;
			*o=initializer?*initializer:object_def;
			o->alloc_bits_ptr=objects_bits_seek_ptr++;

			if (o->v.init)
				o->v.init(o);

			return o;
		}
		objects_bits_seek_ptr=objects_bits_start_ptr;
		objects_seek_ptr=objects_start_ptr;
	}
	fprintf(stderr,"\n    out of objects\n");
	fprintf(stderr,"           in %s at line %d\n\n",__FILE__,__LINE__);
	exit(-1);
}
//------------------------------------------------------------------------ free
inline static void object_dealloc(object*o){
#ifdef object_assert_free
	if(*o->alloc_bits_ptr&2){ //? reallocated?
		fprintf(stderr,"\n    object %p already freed\n",(void*)o);
		fprintf(stderr,"           in %s at line %d\n\n",__FILE__,__LINE__);
		exit(-1);
	}
#endif
	*o->alloc_bits_ptr=2;// flag not allocated and deleted
	metrics.objects_allocated--;
}
//------------------------------------------------------------------- accessors
inline static void _object_assert_bounds(unsigned i){
	if(i<object_cap)
		return;
	fprintf(stderr,"\n    object index %u out of bounds %u\n",
			i,object_cap);
	fprintf(stderr,"           in %s at line %d\n\n",__FILE__,__LINE__);
	exit(-1);
}

//-----------------------------------------------------------------------------
inline static const object*object_ptr_const(const void*p){
	return(const object*)p;
}
//-----------------------------------------------------------------------------
inline static object*object_ptr(void*p){return(object*)p;}
//-----------------------------------------------------------------------------
inline static object*object_at(unsigned i){
#ifdef object_assert_bounds
	_object_assert_bounds(i);
#endif
	return&objects[i];
}
//-----------------------------------------------------------------------------
inline static const object*object_at_const(unsigned i){
#ifdef object_assert_bounds
	_object_assert_bounds(i);
#endif
	return&objects[i];
}
//-----------------------------------------------------------------------------
inline static void objects_free() {
	object*o=objects;
	while(o<objects_end_ptr){
		if(!o->alloc_bits_ptr){
			o++;
			continue;
		}

		{/*** critical ****/
		if(*o->alloc_bits_ptr&1){
			*o->alloc_bits_ptr&=(unsigned char)~(1|2);
		}else{
			o++;
			continue;
		}
		/*** critical done ****/}

		if(o->v.free)
			o->v.free(o);

		for(int i=0;i<object_part_cap;i++){
			if(!o->part[i])
				continue;
			part*p=(part*)o->part[i];
			if(p->free)
				p->free(o,p);
		}

		metrics.objects_allocated--;
		o++;
	}
}

//-----------------------------------------------------------------------------
#define foa(body)({void __fn__ (object*o) body __fn__;})
#define fob(body)({int __fn__ (object*o) body __fn__;})
//-----------------------------------------------------------------------------
inline static void objects_foreach_allocated(int(*f)(object*)){
	object*o=objects;
	while(o<objects_end_ptr){
		if(!o->alloc_bits_ptr||!(*o->alloc_bits_ptr&1)){
			o++;
			continue;
		}
		if(f(o))
			break;
		o++;
	}
}
//-----------------------------------------------------------------------------
inline static void objects_foreach_allocated_all(void(*f)(object*)){
	object*o=objects;
	while(o<objects_end_ptr){
		if(!o->alloc_bits_ptr||!(*o->alloc_bits_ptr&1)){
			o++;
			continue;
		}
		f(o);
		o++;
	}
}
//-----------------------------------------------------------------------------
inline static void objects_foreach(int(*filter)(object*),int(*f)(object*)){
	object*o=objects;
	while(o<objects_end_ptr){
		if(!o->alloc_bits_ptr||!(*o->alloc_bits_ptr&1)){
			o++;
			continue;
		}
		if(filter(o)){
			o++;
			continue;
		}
		if(f(o))
			break;
		o++;
	}
}
//-----------------------------------------------------------------------------
