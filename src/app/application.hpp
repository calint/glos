#pragma once
#include "santa.hpp"

class application final {
public:
  void init() {
    constexpr unsigned cb_hero = 1;

    glos.load_from_file("obj/skydome.obj");
    glos.load_from_file("obj/grid.obj");
    glos.load_from_file("obj/santa.obj");
    glos.load_from_file("obj/sphere.obj");

    constexpr float world_size = grid_cell_size * grid_ncells_wide / 2;

    object *o;
    o = new object{};
    objects.store.push_back(o);
    o->name = "skydome";
    o->node.glo = glos.find_by_name("skydome");
    constexpr float skydome_size = 10;
    constexpr float skydome_scale = world_size / skydome_size;
    o->volume.scale = {skydome_scale, skydome_scale, skydome_scale};
    o->volume.radius = sqrtf(world_size * world_size * 2);

    o = new object{};
    objects.store.push_back(o);
    o->name = "grid";
    o->node.glo = glos.find_by_name("grid");
    constexpr float grid_size = 8;
    constexpr float grid_scale = world_size / grid_size;
    o->volume.scale = {grid_scale, grid_scale, grid_scale};
    o->volume.radius = sqrtf(world_size * world_size * 2);

    o = new santa{};
    objects.store.push_back(o);
    o->name = "santa2";
    o->node.glo = glos.find_by_name("sphere");
    o->volume.scale = {5, 5, 5};
    o->volume.radius = o->node.glo->bounding_radius * 5; // r * scale
    o->physics_nxt.position = {10, o->volume.radius, 10};
    o->physics = o->physics_nxt;
    o->state = &net.states[2];
    o->grid_ifc.collision_bits = cb_hero;
    o->grid_ifc.collision_mask = cb_hero;

    o = new santa{};
    objects.store.push_back(o);
    o->name = "santa1";
    o->node.glo = glos.find_by_name("sphere");
    o->volume.scale = {5, 5, 5};
    o->volume.radius = o->node.glo->bounding_radius * 5;
    o->physics_nxt.position = {-20, o->volume.radius, 20};
    // o->physics_nxt.angular_velocity = {0, 0.2f, 0};
    o->physics = o->physics_nxt;
    o->state = &net.states[1];
    o->grid_ifc.collision_bits = cb_hero;
    o->grid_ifc.collision_mask = cb_hero;

    // camera.type = LOOK_AT;
    // camera.position = {30, 40, 30};
    // player.object = o;

    camera.type = ORTHO;
    camera.position = {0, 50, 0};
    camera.look_at = {0, 0, -0.1f};
    camera.ortho_min_x = -90;
    camera.ortho_min_y = -90;
    camera.ortho_max_x = 90;
    camera.ortho_max_y = 90;

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

  void at_frame_end() {}
};

static application application{};
