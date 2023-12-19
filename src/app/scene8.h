#pragma once
#include "ninja.h"
#include "santa.h"

inline static void main_init_scene_8() {
  object *o;
  glos_load_all_from_file("obj/skydome.obj");
  glos_load_all_from_file("obj/board.obj");
  glos_load_first_from_file("obj/grid.obj");
  glos_load_first_from_file("obj/santa.obj");
  //	glos_load_first_from_file("obj/sphere.obj");

  o = object_alloc(&object_def);
  str_setz(&o->name, "skydome");
  o->node.glo = glos_find_by_name("skydome");
  o->vtbl.render = _object_render_glo_;
  o->bounding_volume.scale = (scale){15, 15, 15, 0};
  o->bounding_volume.radius = 10 * 15;

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

  o = object_alloc(&object_def);
  str_setz(&o->name, "grid");
  o->node.glo = glos_find_by_name("grid");
  o->vtbl.render = _object_render_glo_;
  o->bounding_volume.scale = (scale){10, 10, 10, 0};
  o->physics.position = (position){0, 0, 0, 0};
  o->bounding_volume.radius = 2 * 10;

  game.follow_ptr = o;
  santa *p;

  for (float y = -80; y <= 80; y += 2) {
    for (float x = -80; x <= 80; x += 2) {
      o = santa_alloc_def();
      str_setz(&o->name, "santa");
      o->node.glo = glos_find_by_name("santa");
      o->bounding_volume.scale = (scale){10, 10, 10, 0};
      o->physics.position = (position){x, 2, y, 0};
      o->physics_nxt = o->physics;
      o->bounding_volume.radius = .5f;
      p = (santa *)o->part[0];
      p->keybits_ptr = &net_state_current[1].keybits;
    }
  }

  camera.eye = (position){30, 120, -140, 0};
  camera.lookat = (position){0, 0, 0, 0};

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
