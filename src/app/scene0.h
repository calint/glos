#pragma once
#include "ninja.h"
#include "santa.h"

inline static void main_init_scene_do() {
  object *o;
  glos_load_from_file("obj/skydome.obj");
  glos_load_from_file("obj/grid.obj");
  glos_load_from_file("obj/santa.obj");

  o = object_alloc(&object_def);
  // o->name = "skydome";
  o->node.glo = glos_find_by_name("skydome");
  o->vtbl.render = _object_render_glo_;
  o->bounding_volume.scale = {15, 15, 15, 0};
  o->bounding_volume.radius = 10 * 15;

  o = object_alloc(&object_def);
  // o->name = "grid";
  o->node.glo = glos_find_by_name("grid");
  o->vtbl.render = _object_render_glo_;
  o->bounding_volume.scale = {10, 10, 10, 0};
  o->physics.position = {0, 0, 0, 0};
  o->bounding_volume.radius = 2 * 10;

  santa *p;

  o = santa_alloc_def();
  // o->name = "santa2";
  o->node.glo = glos_find_by_name("santa");
  o->bounding_volume.scale = {10, 10, 10, 0};
  o->bounding_volume.radius = .5f;
  o->physics.position = {-30, 2, -30, 0};
  o->physics_nxt = o->physics;
  p = (santa *)o->part[0];
  p->keybits_ptr = &net_state_current[2].keybits;

  o = santa_alloc_def();
  // o->name = "santa1";
  o->node.glo = glos_find_by_name("santa");
  o->bounding_volume.scale = {10, 10, 10, 0};
  o->physics.position = {30, 2, 30, 0};
  o->physics_nxt = o->physics;
  o->bounding_volume.radius = .5f;
  p = (santa *)o->part[0];
  p->keybits_ptr = &net_state_current[1].keybits;

  game.follow_ptr = o;
  camera.eye = {30, 120, -140, 0};
  camera.lookat = {0, 0, 0, 0};

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
