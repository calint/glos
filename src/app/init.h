#pragma once
#include"ninja.h"
#include"santa.h"

static float ground_base_y=.25f;

inline static void main_init_scene(){
	glos_load_scene_from_file("obj/skydome.obj");
	glos_load_scene_from_file("obj/board.obj");
	glos_load_scene_from_file("obj/blip.obj");

	for(float y=0;y<5;y++){
		for(float z=-10;z<=10;z++){
			for(float x=-10;x<=10;x+=1){
				object*o=ninja_alloc_def();
				o->n.glo=glo_at(2);
				o->p.p=(position){x,y+ground_base_y,z,0};
				bvol_update_radius_using_scale(&o->b);
			}
		}
	}
}
