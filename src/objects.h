#pragma once
#include"object_alloc.h"
#include"part.h"
//------------------------------------------------------------------------ init
inline static void objects_init(){}
//--------------------------------------------------------------------- update
inline static void objects_update(dt dt){
	object*o=objects;
	while(o<objects_end_ptr){
		if(!o->ptr_to_bits||!(*o->ptr_to_bits&1)){
			o++;
			continue;
		}

		if(o->update){
			o->update(o,dt);
			metrics.objects_updated_last_frame++;
		}

		for(int i=0;i<object_part_cap;i++){
			if(!o->part[i])
				continue;
			part*p=(part*)o->part[i];
			if(p->update){
				p->update(o,p,dt);
				metrics.parts_updated_last_frame++;
			}
		}
		o++;
	}
}
//--------------------------------------------------------------------- render
inline static void objects_render() {
	object*o=objects;
	while(o<objects_end_ptr){
		if(!o->ptr_to_bits||!(*o->ptr_to_bits&1)){
			o++;
			continue;
		}

		if(o->render){
			o->render(o);
			metrics.objects_rendered_last_frame++;
		}

		for(int i=0;i<object_part_cap;i++){
			if(!o->part[i])
				continue;
			part*p=(part*)o->part[i];
			if(p->render){
				p->render(o,p);
				metrics.objects_rendered_last_frame++;
			}
		}
		o++;
	}
}
//----------------------------------------------------------------------------
