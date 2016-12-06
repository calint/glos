#pragma once
#include"object_alloc.h"
#include"part.h"
//------------------------------------------------------------------------ init
inline static void objects_init(){}
//--------------------------------------------------------------------- update
inline static void objects_update(dt dt){
	object*o=objects;
	while(o<objects_end_ptr){
		if(!o->ptr_to_bits){
			o++;
			continue;
		}
		if(!(*o->ptr_to_bits&1)){
			o++;
			continue;
		}

		if(o->update)
			o->update(o,dt);

		for(int i=0;i<object_part_cap;i++){
			if(!o->part[i])
				continue;
			part*p=(part*)o->part[i];
			if(p->update)
				p->update(o,p,dt);
		}
		o++;
	}
}
//--------------------------------------------------------------------- render
inline static void objects_render() {
	object*o=objects;
	while(o<objects_end_ptr){
		if(!o->ptr_to_bits){
			o++;
			continue;
		}
		if(!(*o->ptr_to_bits&1)){
			o++;
			continue;
		}

		if(o->render)
			o->render(o);

		for(int i=0;i<object_part_cap;i++){
			if(!o->part[i])
				continue;
			part*p=(part*)o->part[i];
			if(p->render)
				p->render(o,p);
		}
		o++;
	}
}
//----------------------------------------------------------------------------
