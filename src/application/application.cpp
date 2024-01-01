#include "../engine/engine.hpp"
//
using namespace glos;
using namespace glm;
//
#include "objects/cube.hpp"
#include "objects/santa.hpp"
#include "objects/sphere.hpp"
//
static void application_init_shaders();
//
void application_init() {
  application_init_shaders();

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
  glob_skydome_ix = globs.load("obj/skydome.obj", nullptr);
  glob_grid_ix = globs.load("obj/grid.obj", nullptr);
  glob_santa_ix = globs.load("obj/santa.obj", nullptr);
  glob_sphere_ix = globs.load("obj/icosphere.obj", nullptr);
  glob_cube_ix = globs.load("obj/cube.obj", "obj/bv-cube.obj");

  constexpr float world_size = grid_cell_size * grid_ncells_wide;

  {
    object *o = new (objects.alloc()) object{};
    o->name = "grid";
    o->glob_ix = glob_grid_ix;
    const glob &g = globs.at(o->glob_ix);
    constexpr float grid_scale = world_size / 2;
    // note. 2 because model coordinates span is -1 to 1
    o->scale = {grid_scale, grid_scale, grid_scale};
    o->radius = g.bounding_radius * grid_scale;
  }

  {
    object *o = new (objects.alloc()) object{};
    o->name = "skydome";
    o->glob_ix = glob_skydome_ix;
    const glob &g = globs.at(o->glob_ix);
    constexpr float skydome_scale = world_size / 2;
    // note. 2 because model coordinates span is -1 to 1
    o->scale = {skydome_scale, skydome_scale, skydome_scale};
    o->radius = g.bounding_radius * skydome_scale;
  }

  {
    cube *o = new (objects.alloc()) cube{};
    o->name = "cube";
    o->mass = 1;
    o->position = {0, o->scale.y, 0};
    o->net_state = &net.states[1];
  }

  {
    sphere *o = new (objects.alloc()) sphere{};
    o->name = "sphere";
    o->mass = 1;
    o->position = {6, o->scale.y, 0};
    o->net_state = &net.states[1];
  }

  ambient_light = glm::normalize(glm::vec3{0, 1, 0});

  camera.type = camera::type::LOOK_AT;
  camera.position = {0, 50, 50};
  camera.look_at = {0, 0, 0};

  // camera.type = camera::type::ORTHO;
  // camera.position = {0, 50, 0};
  // camera.look_at = {0, 0, -0.0001f};
  // note. -0.0001f because of the math of 'look at'
  camera.ortho_min_x = -10;
  camera.ortho_min_y = -10;
  camera.ortho_max_x = 10;
  camera.ortho_max_y = 10;

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

void application_at_frame_end(glos::frame_context const &fc) {}

void application_free() {}

static void application_init_shaders() {
  {
    const char *vtx = R"(
#version 330 core
uniform mat4 umtx_mw; // model-to-world-matrix
uniform mat4 umtx_wvp;// world-to-view-to-projection
in vec3 apos;
out float depth;
void main() {
  gl_Position = umtx_wvp * umtx_mw * vec4(apos, 1);
  depth = gl_Position.z / 300; // magic number is camera far plane
}
    )";

    const char *frag = R"(
#version 330 core
in float depth;
out vec4 rgba;
void main() {
  rgba = vec4(vec3(depth), 1.0);
}
    )";

    glos::shaders.load_program_from_source(vtx, frag);
  }
  {
    const char *vtx = R"(
#version 330 core
uniform mat4 umtx_mw; // model-to-world-matrix
uniform mat4 umtx_wvp;// world-to-view-to-projection
in vec3 apos;
in vec4 argba;
out vec4 vrgba;
void main() {
  gl_Position = umtx_wvp * umtx_mw * vec4(apos, 1);
  vrgba = argba;
}
    )";

    const char *frag = R"(
#version 330 core
in vec4 vrgba;
out vec4 rgba;
void main(){
  rgba = vrgba;
}
    )";

    glos::shaders.load_program_from_source(vtx, frag);
  }
}