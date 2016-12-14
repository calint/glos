#pragma once
inline static void main_init_scene_4(){
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
	o->p.p.x=-8;
	o->n.glo_ptr=glos_find_by_name("sphere");
	o->g.collide_bits=1;

	o=object_alloc(&object_def);
	str_setz(&o->name,"S1");
	o->p.p.x=-2;
	o->n.glo_ptr=glos_find_by_name("sphere");
	o->g.collide_bits=1;
	o->g.collide_mask=1;

	o=object_alloc(&object_def);
	str_setz(&o->name,"S2");
	o->p.p.x=2;
	o->p.v.x=-1;
	o->n.glo_ptr=glos_find_by_name("sphere");
	o->g.collide_bits=1;
	o->g.collide_mask=1;

	o=object_alloc(&object_def);
	str_setz(&o->name,"S3");
	o->p.p.x=4;
	o->p.v.x=-1;
	o->n.glo_ptr=glos_find_by_name("sphere");
	o->g.collide_bits=1;
	o->g.collide_mask=1;

	o=object_alloc(&object_def);
	str_setz(&o->name,"stopper right");
	o->p.p.x=8;
	o->n.glo_ptr=glos_find_by_name("sphere");
	o->g.collide_bits=1;
}
