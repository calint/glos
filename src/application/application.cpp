#include "../engine/engine.hpp"
//
using namespace glos;
using namespace glm;
//
#include "objects/asteroid_large.hpp"
#include "objects/ship.hpp"
//
static void application_init_shaders();
//
void application_init() {
  application_init_shaders();

  printf("class sizes:\n");
  printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
  printf(": %15s : %-9s :\n", "class", "bytes");
  printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
  printf(": %15s : %-9ld :\n", "ship", sizeof(ship));
  printf(": %15s : %-9ld :\n", "ship", sizeof(asteroid_large));
  printf(": %15s : %-9ld :\n", "ship", sizeof(asteroid_medium));
  printf(": %15s : %-9ld :\n", "ship", sizeof(asteroid_small));
  printf(": %15s : %-9ld :\n", "ship", sizeof(bullet));
  printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
  puts("");

  // assert that classes use fit in objects store slot
  static_assert(sizeof(ship) <= objects_instance_size_B, "");
  static_assert(sizeof(asteroid_large) <= objects_instance_size_B, "");
  static_assert(sizeof(asteroid_medium) <= objects_instance_size_B, "");
  static_assert(sizeof(asteroid_small) <= objects_instance_size_B, "");
  static_assert(sizeof(bullet) <= objects_instance_size_B, "");

  // load the objects and assign the indexes
  glob_skydome_ix = globs.load("assets/obj/skydome.obj", nullptr);

  glob_grid_ix = globs.load("assets/obj/grid.obj", nullptr);

  glob_ship_ix = globs.load("assets/obj/asteroids/ship.obj",
                            "assets/obj/asteroids/ship.obj");

  glob_bullet_ix = globs.load("assets/obj/asteroids/bullet.obj",
                              "assets/obj/asteroids/bullet.obj");
  glob_asteroid_large_ix =
      globs.load("assets/obj/asteroids/asteroid_large.obj",
                 "assets/obj/asteroids/asteroid_large.obj");

  glob_asteroid_medium_ix =
      globs.load("assets/obj/asteroids/asteroid_medium.obj",
                 "assets/obj/asteroids/asteroid_medium.obj");

  glob_asteroid_small_ix =
      globs.load("assets/obj/asteroids/asteroid_small.obj",
                 "assets/obj/asteroids/asteroid_small.obj");

  // assumes grid is a square
  constexpr float world_size = grid_cell_size * grid_ncells_wide;

  // setup the initial scene
  // {
  //   object *o = new (objects.alloc()) object{};
  //   o->name = "grid";
  //   o->glob_ix = glob_grid_ix;
  //   o->position.y = -5;
  //   const glob &g = globs.at(o->glob_ix);
  //   constexpr float grid_scale = world_size / 2;
  //   // note. 2 because model coordinates span is -1 to 1
  //   o->scale = {grid_scale, grid_scale, grid_scale};
  //   o->radius = g.bounding_radius * grid_scale;
  // }

  // {
  //   object *o = new (objects.alloc()) object{};
  //   o->name = "skydome";
  //   o->glob_ix = glob_skydome_ix;
  //   const glob &g = globs.at(o->glob_ix);
  //   constexpr float skydome_scale = world_size / 2;
  //   // note. 2 because model coordinates span is -1 to 1
  //   o->scale = {skydome_scale, skydome_scale, skydome_scale};
  //   o->radius = g.bounding_radius * skydome_scale;
  // }

  // {
  //   cube *o = new (objects.alloc()) cube{};
  //   o->name = "cube";
  //   o->mass = 1;
  //   o->position = {0, o->scale.y, 0};
  //   o->net_state = &net.states[1];
  // }

  // {
  //   sphere *o = new (objects.alloc()) sphere{};
  //   o->name = "sphere";
  //   o->mass = 1;
  //   o->position = {6, o->scale.y, 0};
  //   o->net_state = &net.states[1];
  // }

  // setup the light
  ambient_light = normalize(vec3{1, 1, 1});

  // setup the camera
  // camera.type = camera::type::LOOK_AT;
  // camera.position = {0, 50, 50};
  // camera.look_at = {0, 0, 0};

  camera.type = camera::type::ORTHO;
  camera.position = {0, 50, 0};
  camera.look_at = {0, 0, -0.0001f};
  // note. -0.0001f because of the math of 'look at'
  camera.ortho_min_x = -20;
  camera.ortho_min_y = -20;
  camera.ortho_max_x = 20;
  camera.ortho_max_y = 20;

  // fit the 'grid' size
  // camera.ortho_min_x = -80;
  // camera.ortho_min_y = -80;
  // camera.ortho_max_x = 80;
  // camera.ortho_max_y = 80;

  // for (float y = -80; y <= 80; y += 1) {
  //   for (float x = -80; x <= 80; x += 1) {
  //     sphere *o = new (objects.alloc()) sphere{};
  //     o->position = {x, o->radius, y};
  //     o->angular_velocity.y = radians(40.0f);
  //   }
  // }

  // {
  //   sphere *o = new (objects.alloc()) sphere{};
  //   o->name = "sphere";
  //   o->mass = 1;
  //   o->position = {-20, o->scale.y, 0};
  //   o->net_state = &net.states[1];
  // }

  {
    ship *o = new (objects.alloc()) ship{};
    o->net_state = &net.states[1];
  }

  for (int i = 0; i < 10; i++) {
    asteroid_large *o = new (objects.alloc()) asteroid_large{};
    o->position.x = rand() % 40 - 20;
    o->position.z = rand() % 40 - 20;
    o->velocity.x = rand() % 10 - 5;
    o->velocity.z = rand() % 10 - 5;
  }
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