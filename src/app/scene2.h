#pragma once
inline static void main_init_scene_2() {
  glos_load_all_from_file("obj/skydome.obj");
  glos_load_all_from_file("obj/rigg1d/sphere.obj");

  object *o = object_alloc(&object_def);
  str_setz(&o->name, "skydome");
  o->node.glo = glos_find_by_name("skydome");

  camera.eye = (position){0, 15, -15, 0};
  camera.lookat = (position){0, 0, 0, 0};

  glos_load_all_from_file("obj/rigg1d/sphere.obj");

  o = object_alloc(&object_def);
  str_setz(&o->name, "stopper left");
  o->physics_nxt.position.x = -8;
  o->node.glo = glos_find_by_name("sphere");
  o->grid_ifc.collide_bits = 1;

  o = object_alloc(&object_def);
  str_setz(&o->name, "S1");
  o->physics_nxt.position.x = -2;
  o->physics_nxt.velocity.x = 1;
  o->node.glo = glos_find_by_name("sphere");
  o->grid_ifc.collide_bits = 1;
  o->grid_ifc.collide_mask = 1;

  o = object_alloc(&object_def);
  str_setz(&o->name, "S2");
  o->physics_nxt.position.x = 2;
  o->physics_nxt.velocity.x = -1;
  o->node.glo = glos_find_by_name("sphere");
  o->grid_ifc.collide_bits = 1;
  o->grid_ifc.collide_mask = 1;

  o = object_alloc(&object_def);
  str_setz(&o->name, "stopper right");
  o->physics_nxt.position.x = 8;
  o->node.glo = glos_find_by_name("sphere");
  o->grid_ifc.collide_bits = 1;
}
