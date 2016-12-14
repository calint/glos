#pragma once
inline static void main_init_scene_5(){
	glos_load_all_from_file("obj/skydome.obj");
	glos_load_all_from_file("obj/rigg1d/sphere.obj");

	object*o=object_alloc(&object_def);
	str_setz(&o->name,"skydome");
	o->n.glo_ptr=glos_find_by_name("skydome");

	camera.eye=(position){0,15,-15,0};
	camera.lookat=(position){0,0,0,0};

	glos_load_all_from_file("obj/rigg1d/sphere.obj");

	o=object_alloc(&object_def);
	str_setz(&o->name,"stopper left");
	o->p_nxt.p.x=-10;
	o->n.glo_ptr=glos_find_by_name("sphere");
	o->g.collide_bits=1;

	o=object_alloc(&object_def);
	str_setz(&o->name,"S1");
	o->p_nxt.p.x=-8;
	o->p_nxt.v.x=1;
	o->n.glo_ptr=glos_find_by_name("sphere");
	o->g.collide_bits=1;
	o->g.collide_mask=1;

	o=object_alloc(&object_def);
	str_setz(&o->name,"S2");
	o->p_nxt.p.x=-6;
	o->p_nxt.v.x=1;
	o->n.glo_ptr=glos_find_by_name("sphere");
	o->g.collide_bits=1;
	o->g.collide_mask=1;

	o=object_alloc(&object_def);
	str_setz(&o->name,"S3");
	o->p_nxt.p.x=0;
	o->n.glo_ptr=glos_find_by_name("sphere");
	o->g.collide_bits=1;
	o->g.collide_mask=1;

	o=object_alloc(&object_def);
	str_setz(&o->name,"S4");
	o->p_nxt.p.x=8;
	o->p_nxt.v.x=-1;
	o->n.glo_ptr=glos_find_by_name("sphere");
	o->g.collide_bits=1;
	o->g.collide_mask=1;

	o=object_alloc(&object_def);
	str_setz(&o->name,"S5");
	o->p_nxt.p.x=6;
	o->p_nxt.v.x=-1;
	o->n.glo_ptr=glos_find_by_name("sphere");
	o->g.collide_bits=1;
	o->g.collide_mask=1;


	o=object_alloc(&object_def);
	str_setz(&o->name,"stopper right");
	o->p_nxt.p.x=10;
	o->n.glo_ptr=glos_find_by_name("sphere");
	o->g.collide_bits=1;
}
