#pragma once
#include "objects/cube.hpp"
#include "objects/santa.hpp"
#include "objects/sphere.hpp"

class application final {
public:
  void init() {
    printf("class sizes:\n");
    printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
    printf(": %15s : %-9s :\n", "class", "bytes");
    printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
    printf(": %15s : %-9ld :\n", "santa", sizeof(santa));
    printf(": %15s : %-9ld :\n", "sphere", sizeof(sphere));
    printf(": %15s : %-9ld :\n", "sphere", sizeof(cube));
    printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
    puts("");

    static_assert(sizeof(santa) <= objects_instance_size_B, "");
    static_assert(sizeof(sphere) <= objects_instance_size_B, "");
    static_assert(sizeof(cube) <= objects_instance_size_B, "");

    // the load order matches the 'glo_index_*' configuration
    glo_skydome_ix = glos::glos.load("obj/skydome.obj", nullptr);
    glo_grid_ix = glos::glos.load("obj/grid.obj", nullptr);
    glo_santa_ix = glos::glos.load("obj/santa.obj", nullptr);
    glo_sphere_ix = glos::glos.load("obj/icosphere.obj", nullptr);
    glo_cube_ix = glos::glos.load("obj/cube.obj", "obj/bv-cube.obj");

    constexpr float world_size = grid_cell_size * grid_ncells_wide;

    {
      glos::object *o = new (glos::objects.alloc()) glos::object{};
      o->name = "skydome";
      o->node.glo_ix = glo_skydome_ix;
      const glos::glo &g = glos::glos.get_by_index(o->node.glo_ix);
      const float skydome_scale = world_size / (2 * g.bounding_radius);
      o->volume.scale = {skydome_scale, skydome_scale, skydome_scale};
      o->volume.radius = g.bounding_radius * skydome_scale;
      // printf("skydome bounding radius: %0.3f\n", o->volume.radius);
    }

    {
      glos::object *o = new (glos::objects.alloc()) glos::object{};
      o->name = "grid";
      o->node.glo_ix = glo_grid_ix;
      const glos::glo &g = glos::glos.get_by_index(o->node.glo_ix);
      constexpr float grid_scale = world_size / 16;
      // note. 16 is the model coordinates span from -8 to 8
      o->volume.scale = {grid_scale, grid_scale, grid_scale};
      o->volume.radius = g.bounding_radius * grid_scale;
    }

    {
      cube *o = new (glos::objects.alloc()) cube{};
      o->name = "cube";
      o->physics_nxt.mass = 1;
      o->physics_nxt.position = {0, 1, 0};
      o->physics_nxt.velocity = {0, 0, 0};
      // o->physics_nxt.angular_velocity = {glm::radians(10.0f),
      //                                    glm::radians(10.0f), 0};
      o->physics = o->physics_nxt;
    }

    {
      sphere *o = new (glos::objects.alloc()) sphere{};
      o->name = "sphere";
      o->physics_nxt.mass = 1;
      o->physics_nxt.position = {6, 1, 0};
      o->physics = o->physics_nxt;
      o->net_state = &glos::net.states[1];
    }

    ambient_light = glm::normalize(glm::vec3{0, 1, 0});

    // glos::camera.type = glos::camera::type::LOOK_AT;
    // glos::camera.position = {0, 50, 50};
    // glos::camera.look_at = {0, 0, 0};

    glos::camera.type = glos::camera::type::ORTHO;
    glos::camera.position = {0, 50, 0};
    glos::camera.look_at = {0, 0, -0.0001f};
    // note. -0.0001f because of the math of 'look at'
    glos::camera.ortho_min_x = -10;
    glos::camera.ortho_min_y = -10;
    glos::camera.ortho_max_x = 10;
    glos::camera.ortho_max_y = 10;

    // fit the 'grid' size
    // camera.ortho_min_x = -80;
    // camera.ortho_min_y = -80;
    // camera.ortho_max_x = 80;
    // camera.ortho_max_y = 80;

    // for (float y = -80; y <= 80; y += 1) {
    //   for (float x = -80; x <= 80; x += 1) {
    //     object *o = new (objects.alloc()) santa{};
    //     o->name = "santa1";
    //     o->physics_nxt.position = {x, o->volume.radius, y};
    //     o->physics = o->physics_nxt;
    //     o->net_state = &net.states[1];
    //   }
    // }
  }

  void free() {}

  void at_frame_end() {}
};

static application application{};
