#include "../engine/engine.hpp"
//
using namespace glos;
using namespace glm;
//
static int score = 0;
//
#include "objects/asteroid_large.hpp"
#include "objects/fragment.hpp"
#include "objects/ship.hpp"
//
static void application_init_shaders();
static void create_asteroids(int const num);
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
  printf(": %15s : %-9ld :\n", "asteroid_large", sizeof(asteroid_large));
  printf(": %15s : %-9ld :\n", "asteroid_medium", sizeof(asteroid_medium));
  printf(": %15s : %-9ld :\n", "asteroid_small", sizeof(asteroid_small));
  printf(": %15s : %-9ld :\n", "bullet", sizeof(bullet));
  printf(": %15s : %-9ld :\n", "fragment", sizeof(fragment));
  printf(": %15s : %-9ld :\n", "power_up", sizeof(power_up));
  printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
  puts("");

  // assert that classes use fit in objects store slot
  static_assert(sizeof(asteroid_large) <= objects_instance_size_B, "");
  static_assert(sizeof(asteroid_medium) <= objects_instance_size_B, "");
  static_assert(sizeof(asteroid_small) <= objects_instance_size_B, "");
  static_assert(sizeof(bullet) <= objects_instance_size_B, "");
  static_assert(sizeof(fragment) <= objects_instance_size_B, "");
  static_assert(sizeof(power_up) <= objects_instance_size_B, "");
  static_assert(sizeof(ship) <= objects_instance_size_B, "");

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

  // setup scene
  if (net.enabled) {
    // multiplayer mode
    ship *p1 = new (objects.alloc()) ship{};
    p1->position.x = -5;
    p1->net_state = &net.states[1];

    ship *p2 = new (objects.alloc()) ship{};
    p2->position.x = 5;
    p2->net_state = &net.states[2];
  } else {
    // single player mode
    ship *o = new (objects.alloc()) ship{};
    o->net_state = &net.states[1];
  }

  // setup light and camera
  ambient_light = normalize(vec3{1, 1, 1});

  camera.type = camera::type::ORTHO;
  camera.position = {0, 50, 0};
  camera.look_at = {0, 0, -0.0001f};
  // note. -0.0001f because of the math of 'look at'
  camera.ortho_min_x = -20;
  camera.ortho_min_y = -20;
  camera.ortho_max_x = 20;
  camera.ortho_max_y = 20;

  // camera.type = camera::type::LOOK_AT;
  // camera.position = {0, 32, 0};
  // camera.look_at = {0, 0, -0.00001f};

  // assumes grid is a square
  // constexpr float world_size = grid_cell_size * grid_ncells_wide;

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

  hud.load_font("assets/fonts/digital-7 (mono).ttf", 20);
}

void application_on_update_done() {
  if (asteroids_alive == 0) {
    level++;
    create_asteroids(level);
  }
}

void application_on_render_done() {
  char buf[256];
  sprintf(buf, "score: %06d", score);
  hud.print(buf, SDL_Color{255, 0, 0, 255}, 60, 10);
}

void application_free() {}

static void create_asteroids(int const num) {
  constexpr float v = asteroid_large_speed;
  constexpr float d = game_area_max_x - game_area_min_x;
  for (int i = 0; i < num; i++) {
    asteroid_large *o = new (objects.alloc()) asteroid_large{};
    o->position.x = rnd1(d);
    o->position.z = rnd1(d);
    o->velocity.x = rnd1(v);
    o->velocity.z = rnd1(v);
  }
}

static void application_init_shaders() {
  {
    char const *vtx = R"(
#version 330 core
uniform mat4 umtx_mw; // model-to-world-matrix
uniform mat4 umtx_wvp;// world-to-view-to-projection
layout(location = 0) in vec3 apos;
layout(location = 1) in vec4 argba;
layout(location = 2) in vec3 anorm;
layout(location = 3) in vec2 atex;

void main() {
  gl_Position = umtx_wvp * umtx_mw * vec4(apos, 1);
}
    )";

    char const *frag = R"(
#version 330 core
out vec4 rgba;
void main() {
  rgba = vec4(vec3(gl_FragCoord.z), 1.0);
}
    )";

    glos::shaders.load_program_from_source(vtx, frag);
  }
  {
    char const *vtx = R"(
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

    char const *frag = R"(
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