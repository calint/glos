#include "../engine/engine.hpp"
//
using namespace glos;
using namespace glm;
//
#include "objects/asteroid_large.hpp"
#include "objects/fragment.hpp"
#include "objects/ship.hpp"
//
static void application_init_shaders();
static void create_asteroids(int num);
//
static int level = 0;
//
void application_init() {
  application_init_shaders();

  printf("class sizes:\n");
  printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
  printf(": %15s : %-9s :\n", "class", "bytes");
  printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
  printf(": %15s : %-9ld :\n", "ship", sizeof(ship));
  printf(": %15s : %-9ld :\n", "asteroid L", sizeof(asteroid_large));
  printf(": %15s : %-9ld :\n", "asteroid M", sizeof(asteroid_medium));
  printf(": %15s : %-9ld :\n", "asteroid S", sizeof(asteroid_small));
  printf(": %15s : %-9ld :\n", "bullet", sizeof(bullet));
  printf(": %15s : %-9ld :\n", "fragment", sizeof(fragment));
  printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
  puts("");

  // assert that classes use fit in objects store slot
  static_assert(sizeof(ship) <= objects_instance_size_B, "");
  static_assert(sizeof(asteroid_large) <= objects_instance_size_B, "");
  static_assert(sizeof(asteroid_medium) <= objects_instance_size_B, "");
  static_assert(sizeof(asteroid_small) <= objects_instance_size_B, "");
  static_assert(sizeof(bullet) <= objects_instance_size_B, "");
  static_assert(sizeof(fragment) <= objects_instance_size_B, "");

  // load the objects and assign the indexes
  glob_ix_skydome = globs.load("assets/obj/skydome.obj", nullptr);

  glob_ix_grid = globs.load("assets/obj/grid.obj", nullptr);

  glob_ix_ship = globs.load("assets/obj/asteroids/ship.obj",
                            "assets/obj/asteroids/ship.obj");

  glob_ix_ship_engine_on =
      globs.load("assets/obj/asteroids/ship_engine_on.obj",
                 "assets/obj/asteroids/ship_engine_on.obj");

  glob_ix_bullet = globs.load("assets/obj/asteroids/bullet.obj",
                              "assets/obj/asteroids/bullet.obj");
  glob_ix_asteroid_large =
      globs.load("assets/obj/asteroids/asteroid_large.obj",
                 "assets/obj/asteroids/asteroid_large.obj");

  glob_ix_asteroid_medium =
      globs.load("assets/obj/asteroids/asteroid_medium.obj",
                 "assets/obj/asteroids/asteroid_medium.obj");

  glob_ix_asteroid_small =
      globs.load("assets/obj/asteroids/asteroid_small.obj",
                 "assets/obj/asteroids/asteroid_small.obj");

  glob_ix_fragment = globs.load("assets/obj/asteroids/fragment.obj", nullptr);

  glob_ix_power_up = globs.load("assets/obj/asteroids/power_up.obj", nullptr);

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

  // setup the light
  ambient_light = normalize(vec3{1, 1, 1});

  // setup the camera
  // camera.type = camera::type::LOOK_AT;
  // camera.position = {0, 20, 20};
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

  {
    ship *o = new (objects.alloc()) ship{};
    o->net_state = &net.states[1];
  }
}

void application_at_frame_end() {
  if (asteroids_alive == 0) {
    level++;
    create_asteroids(level);
  }
}

void application_free() {}

static void create_asteroids(int num) {
  constexpr int v = asteroid_large_speed;
  constexpr int d = int(game_area_max_x - game_area_min_x);
  for (int i = 0; i < num; i++) {
    asteroid_large *o = new (objects.alloc()) asteroid_large{};
    o->position.x = rand() % d - d / 2;
    o->position.z = rand() % d - d / 2;
    o->velocity.x = rand() % v - v / 2;
    o->velocity.z = rand() % v - v / 2;
  }
}

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