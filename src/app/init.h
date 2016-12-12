#pragma once
#include"ninja.h"
#include"santa.h"

static float ground_base_y=.25f;

inline static void main_init_scene(){
	object*o;

	o=object_alloc(&object_def);
	o->n.glo_ptr=glos_load_first_in_file("obj/skydome.obj");
	o->v.render=object_render_glo;

	o=object_alloc(&object_def);
	o->n.glo_ptr=glos_load_first_in_file("obj/board.obj");
	o->v.render=object_render_glo;

	o=santa_alloc_def();
	o->n.glo_ptr=glos_load_first_in_file("obj/santa.obj");
	o->p.p=(position){1,ground_base_y+1,0,0};
	santa*p=o->part[0];
	game.keybits_ptr=&p->keybits;
	game.follow_ptr=o;



	o=santa_alloc_def();
	o->n.glo_ptr=glos_load_first_in_file("obj/santa.obj");
	o->p.p=(position){-1,ground_base_y+1,0,0};


//	camera.eye=(position){-2,2,2,0};
	camera.eye=(position){0,5,30,0};
	camera.lookat=(position){0,0,0,0};
//
//	for(float y=0;y<5;y++){
//		for(float z=-10;z<=10;z++){
//			for(float x=-10;x<=10;x+=1){
//				object*o=ninja_alloc_def();
//				o->n.glo_ptr=glo_at(2);
//				o->p.p=(position){x,y+ground_base_y,z,0};
//				bvol_update_radius_using_scale(&o->b);
//			}
//		}
//	}
}
