#pragma once
#include"alloc.h"
#include"part.h"
//------------------------------------------------------------------------ init
inline static void objects_init(){}
//------------------------------------------------------------------------ free
inline static void objects_free() {
	for(int i=0;i<objects_count;i++){
		object*o=&objects[i];
		if(bits_is_bit_set(&objects_bits[i],bit_object_allocated))
			if(o->free)
				o->free(o);
	}
}
//--------------------------------------------------------------------- update
inline static void objects_update(dt dt){
	object*o=objects;
	while(o<objects_end_ptr){
		if(o->update)o->update(o,dt);
		for(int i=0;i<object_part_cap;i++){
			if(!o->part[i])continue;
			part*p=(part*)o->part[i];
			if(p->update)p->update(o,p,dt);
		}
		o++;
	}
}
//--------------------------------------------------------------------- render
inline static void objects_render() {
	object*o=objects;
	while(o<objects_end_ptr){
		if(o->render)o->render(o);
		for(int i=0;i<object_part_cap;i++){
			if(!o->part[i])continue;
			part*p=(part*)o->part[i];
			if(p->render)p->render(o,p);
		}
		o++;
	}
}
//----------------------------------------------------------------------------
