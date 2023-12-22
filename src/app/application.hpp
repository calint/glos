#pragma once
#include "santa.hpp"

class application final {
public:
  void init() {
    object *o;
    glos.load_from_file("obj/skydome.obj");
    glos.load_from_file("obj/grid.obj");
    glos.load_from_file("obj/santa.obj");

    o = new object{};
    objects.store.push_back(o);
    o->name = "skydome";
    o->node.glo = glos.find_by_name("skydome");
    o->volume.scale = {15, 15, 15};
    o->volume.radius = 10 * 15;
    // o->physics.angular_velocity = {0, 0.5f, 0};
    // o->physics_nxt = o->physics;

    o = new object{};
    objects.store.push_back(o);
    o->name = "grid";
    o->node.glo = glos.find_by_name("grid");
    o->volume.scale = {10, 10, 10};
    o->volume.radius = 2 * 10;

    o = new santa{};
    objects.store.push_back(o);
    o->name = "santa2";
    o->node.glo = glos.find_by_name("santa");
    o->volume.scale = {10, 10, 10};
    o->volume.radius = .5f;
    o->physics.position = {-30, 2, -30};
    o->physics_nxt = o->physics;
    o->keys_ptr = &net.state_current[2].keys;

    o = new santa{};
    objects.store.push_back(o);
    o->name = "santa1";
    o->node.glo = glos.find_by_name("santa");
    o->volume.scale = {10, 10, 10};
    o->physics.position = {30, 2, 30};
    o->physics.angular_velocity = {0, 0.2f, 0};
    o->physics_nxt = o->physics;
    o->volume.radius = .5f;
    o->keys_ptr = &net.state_current[1].keys;

    game.follow_object = o;
    // camera.position = {30, 120, 140};
    camera.position = {0, 10, 100};

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

  void free() {}

  void at_frame_completed() {}
};

static application application{};
