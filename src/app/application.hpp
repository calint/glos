#pragma once
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
    printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
    puts("");

    static_assert(sizeof(santa) <= objects_instance_size_B, "");

    // the load order matches the 'glo_index_*' configuration
    glos.load_from_file("obj/skydome.obj");
    glos.load_from_file("obj/grid.obj");
    glos.load_from_file("obj/icosphere.obj");

    constexpr float world_size = grid_cell_size * grid_ncells_wide;

    {
      object *o = new (objects.alloc()) object{};
      o->name = "skydome";
      o->node.glo = glos.get_by_index(glo_index_skydome);
      const float skydome_scale =
          world_size / (2 * o->node.glo->bounding_radius);
      o->volume.scale = {skydome_scale, skydome_scale, skydome_scale};
      o->volume.radius = o->node.glo->bounding_radius * skydome_scale;
      // printf("skydome bounding radius: %0.3f\n", o->volume.radius);
    }

    {
      object *o = new (objects.alloc()) object{};
      o->name = "grid";
      o->node.glo = glos.get_by_index(glo_index_grid);
      constexpr float grid_scale = world_size / 16;
      // note. 16 is the model coordinates span from -8 to 8
      o->volume.scale = {grid_scale, grid_scale, grid_scale};
      o->volume.radius = o->node.glo->bounding_radius * grid_scale;
    }

    // {
    //   object *o = new (objects.alloc()) santa{};
    //   o->name = "santa1";
    //   o->physics_nxt.mass = std::numeric_limits<float>::max();
    //   o->physics_nxt.position = {-6, o->volume.radius, 0};
    //   o->physics_nxt.velocity = {0, 0, 0};
    //   o->physics = o->physics_nxt;
    // }

    {
      sphere *o = new (objects.alloc()) sphere{};
      o->name = "sphere1";
      o->physics_nxt.mass = 3;
      o->physics_nxt.position = {-5, o->volume.radius, 0};
      o->physics_nxt.velocity = {4, 0, 0};
      o->physics = o->physics_nxt;
    }

    {
      sphere *o = new (objects.alloc()) sphere{};
      o->name = "sphere2";
      o->physics_nxt.mass = 5;
      o->physics_nxt.position = {5, o->volume.radius, 0};
      o->physics_nxt.velocity = {-6, 0, 0};
      o->physics = o->physics_nxt;
    }

    ambient_light = glm::normalize(glm::vec3{0, 1, 0});

    camera.type = LOOK_AT;
    camera.position = {0, 20, 20};
    camera.look_at = {0, 0, 0};

    // camera.type = ORTHO;
    // camera.position = {0, 50, 0};
    // camera.look_at = {0, 0, -0.0001f};
    // // note. -0.0001f because of the math of 'look at'

    // // fit the 'grid' size
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
