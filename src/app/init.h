#pragma once
#include"ninja.h"
#include"santa.h"


inline static void main_init_scene_1D(){
	glos_load_all_from_file("obj/rigg1d/sphere.obj");
	glos_load_all_from_file("obj/rigg1d/rigg1d.obj");
	glos_load_all_from_file("obj/rigg1d/bounding-radius.obj");
	glos_load_all_from_file("obj/santa.obj");
//	glos_load_all_from_file("obj/skydome.obj");


	camera.eye=(position){0,11,11,0};
	camera.lookat=(position){0,0,0,0};

	object*o;
	santa*p;


	o=object_alloc(&object_def);
	str_setz(&o->name,"sphere");
//	o->n.glo_ptr=glos_find_by_name("sphere");
//	o->v.render=object_render_glo;

	p=malloc(sizeof(santa));
	*p=santa_def;
	p->bounding_glo_ptr=glos_find_by_name("bounding-radius");
	p->keybits_ptr=&net_state_current[1].keybits;
	o->part[0]=(part*)p;





//	o=object_alloc(&object_def);
//	str_setz(&o->name,"skydome");
//	o->n.glo_ptr=glos_find_by_name("skydome");
//	o->v.render=object_render_glo;
//
//	o=object_alloc(&object_def);
//	o->n.glo_ptr=glos_find_by_name("rigg1d");
//	o->v.render=object_render_glo;
//
//
//	o=object_alloc(&object_def);
//	str_setz(&o->name,"Sl");
//	o->n.glo_ptr=glos_find_by_name("sphere");
//	o->v.render=object_render_glo;
//	o->p.p.x=-4;
//	o->p.p.y=o->b.s.y;
//
//	o=object_alloc(&object_def);
//	str_setz(&o->name,"Sr");
//	o->n.glo_ptr=glos_find_by_name("sphere");
//	o->v.render=object_render_glo;
//	o->p.p.x=4;
//	o->p.p.y=o->b.s.y;
}

//static float ground_base_y=.25f;

inline static void main_init_scene(){
	main_init_scene_1D();
	return;
	object*o;
	glos_load_all_from_file("obj/skydome.obj");
	glos_load_all_from_file("obj/board.obj");
	glos_load_first_from_file("obj/grid.obj");
	glos_load_first_from_file("obj/santa.obj");
	glos_load_first_from_file("obj/sphere.obj");

	o=object_alloc(&object_def);
	str_setz(&o->name,"skydome");
	o->n.glo_ptr=glos_find_by_name("skydome");
	o->v.render=object_render_glo;
	o->b.s=(scale){15,15,15,0};
	o->b.r=10*15;

//	o=object_alloc(&object_def);
//	o->n.glo_ptr=glos_find_by_name("board");
//	o->v.render=object_render_glo;
//	o->p.p.x=-80;
//	o->p.p.y=10;
//	o->p.p.z=-80;
//	o->b.r=10;
//
//	o=object_alloc(&object_def);
//	o->n.glo_ptr=glos_find_by_name("board");
//	o->v.render=object_render_glo;
//	o->p.p.x=80;
//	o->p.p.y=10;
//	o->p.p.z=80;
//	o->b.r=10;

	o=object_alloc(&object_def);
	str_setz(&o->name,"grid");
	o->n.glo_ptr=glos_find_by_name("grid");
	o->v.render=object_render_glo;
	o->b.s=(scale){10,10,10,0};
	o->p.p=(position){0,0,0,0};
	o->b.r=2*10;

	game.follow_ptr=o;
	santa*p;

	o=santa_alloc_def();
	str_setz(&o->name,"santa2");
	o->n.glo_ptr=glos_find_by_name("santa");
	o->b.s=(scale){10,10,10,0};
	o->p.p=(position){-30,2,-30,0};
	o->b.r=.5f;
	p=(santa*)o->part[0];
	p->keybits_ptr=&net_state_current[2].keybits;

	o=santa_alloc_def();
	str_setz(&o->name,"santa1");
	o->n.glo_ptr=glos_find_by_name("santa");
	o->b.s=(scale){10,10,10,0};
	o->p.p=(position){30,2,30,0};
	o->b.r=.5f;
	p=(santa*)o->part[0];
	p->keybits_ptr=&net_state_current[1].keybits;


	camera.eye=(position){30,120,140,0};
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
