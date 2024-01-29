// reviewed: 2024-01-04
// reviewed: 2024-01-08

// make common used namespace available in game code
using namespace glos;
using namespace glm;

// game state
static unsigned level = 0; // no racing
static unsigned score = 0; // racing ok
static unsigned score_prv = score;
static std::atomic<unsigned> asteroids_alive{0};

// objects
#include "objects/asteroid_large.hpp"
#include "objects/cube.hpp"
#include "objects/fragment.hpp"
#include "objects/ship.hpp"
#include "objects/sphere.hpp"
#include "objects/tetra.hpp"

// forward declarations
static void application_init_shaders();
static void create_asteroids(unsigned num);
static void create_cubes(unsigned num);

// engine interface
static void application_init() {
  application_init_shaders();

  printf("\ntime is %lu ms\n\n", frame_context.ms);

  printf("class sizes:\n");
  printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
  printf(": %15s : %-9s :\n", "class", "bytes");
  printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
  printf(": %15s : %-9zu :\n", "asteroid_large", sizeof(asteroid_large));
  printf(": %15s : %-9zu :\n", "asteroid_medium", sizeof(asteroid_medium));
  printf(": %15s : %-9zu :\n", "asteroid_small", sizeof(asteroid_small));
  printf(": %15s : %-9zu :\n", "bullet", sizeof(bullet));
  printf(": %15s : %-9zu :\n", "fragment", sizeof(fragment));
  printf(": %15s : %-9zu :\n", "power_up", sizeof(power_up));
  printf(": %15s : %-9zu :\n", "ship", sizeof(ship));
  printf(": %15s : %-9zu :\n", "cube", sizeof(cube));
  printf(": %15s : %-9zu :\n", "sphere", sizeof(sphere));
  printf(": %15s : %-9zu :\n", "tetra", sizeof(tetra));
  printf(":-%15s-:-%-9s-:\n", "---------------", "---------");
  puts("");

  // assert that classes used fit in objects store slot
  static_assert(sizeof(asteroid_large) <= objects_instance_size_B);
  static_assert(sizeof(asteroid_medium) <= objects_instance_size_B);
  static_assert(sizeof(asteroid_small) <= objects_instance_size_B);
  static_assert(sizeof(bullet) <= objects_instance_size_B);
  static_assert(sizeof(fragment) <= objects_instance_size_B);
  static_assert(sizeof(power_up) <= objects_instance_size_B);
  static_assert(sizeof(ship) <= objects_instance_size_B);
  static_assert(sizeof(cube) <= objects_instance_size_B);
  static_assert(sizeof(sphere) <= objects_instance_size_B);
  static_assert(sizeof(tetra) <= objects_instance_size_B);

  // load the objects and assign the glob indexes

  // stock objects
  glob_ix_cube = globs.load("assets/obj/cube.obj", "assets/obj/cube-bp.obj");
  glob_ix_tetra = globs.load("assets/obj/tetra.obj", "assets/obj/tetra.obj");
  glob_ix_sphere = globs.load("assets/obj/sphere.obj", nullptr);

  // game objects
  glob_ix_ship = globs.load("assets/obj/asteroids/ship.obj",
                            "assets/obj/asteroids/ship.obj");
  glob_ix_ship_engine_on =
      globs.load("assets/obj/asteroids/ship_engine_on.obj",
                 "assets/obj/asteroids/ship_engine_on.obj");
  glob_ix_bullet = globs.load("assets/obj/asteroids/bullet.obj",
                              "assets/obj/asteroids/bullet-bp.obj");
  glob_ix_asteroid_large =
      globs.load("assets/obj/asteroids/asteroid_large.obj",
                 "assets/obj/asteroids/asteroid_large-bp.obj");
  glob_ix_asteroid_medium =
      globs.load("assets/obj/asteroids/asteroid_medium.obj",
                 "assets/obj/asteroids/asteroid_medium.obj");
  glob_ix_asteroid_small =
      globs.load("assets/obj/asteroids/asteroid_small.obj",
                 "assets/obj/asteroids/asteroid_small.obj");
  glob_ix_fragment = globs.load("assets/obj/asteroids/fragment.obj", nullptr);
  glob_ix_power_up = globs.load("assets/obj/asteroids/power_up.obj", nullptr);

  // object *skydome = new (objects.alloc()) object{};
  // skydome->glob_ix = globs.load("assets/obj/skydome.obj", nullptr);
  // skydome->bounding_radius = 50;
  // skydome->scale = {50.0f, 50.0f, 50.0f};

  if (not performance_test) {
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
      ship *p = new (objects.alloc()) ship{};
      p->net_state = &net.states[1];
    }
  } else {
    create_cubes(objects_count);
  }

  // setup light and camera
  ambient_light = normalize(vec3{1, 1, 1});

  camera.type = camera::type::ORTHOGONAL;
  camera.position = {0, 50, 0};
  camera.look_at = {0, 0, -0.0001f};
  // note. -0.0001f because of the math of 'look at'
  camera.ortho_min_x = -game_area_half_x;
  camera.ortho_min_y = -game_area_half_z;
  camera.ortho_max_x = game_area_half_x;
  camera.ortho_max_y = game_area_half_z;

  // camera.type = camera::type::LOOK_AT;
  // camera.position = {0, 30, 30};
  // camera.look_at = {0, 0, -0.0001f};

  hud.load_font("assets/fonts/digital-7 (mono).ttf", 20);
}

// engine interface
static void application_on_update_done() {
  if (performance_test) {
    return;
  }

  if (asteroids_alive == 0) {
    ++level;
    create_asteroids(level * asteroid_level);
  }
}

// engine interface
static void application_on_render_done() {
  if (score != score_prv) {
    score_prv = score;
    std::array<char, 256> buf{};
    sprintf(buf.data(), "score: %06u", score);
    hud.print(buf.data(), SDL_Color{255, 0, 0, 255}, 60, 10);
  }
}

// engine interface
static void application_free() {}

static void create_asteroids(unsigned const num) {
  constexpr float v = asteroid_large_speed;
  constexpr float d = game_area_max_x - game_area_min_x;
  for (unsigned i = 0; i < num; ++i) {
    asteroid_large *o = new (objects.alloc()) asteroid_large{};
    o->position.x = rnd1(d);
    o->position.z = rnd1(d);
    o->velocity.x = rnd1(v);
    o->velocity.z = rnd1(v);
  }
}

static void create_cubes(unsigned const num) {
  constexpr float v = cube_speed;
  constexpr float d = game_area_max_x - game_area_min_x;
  for (unsigned i = 0; i < num; ++i) {
    cube *o = new (objects.alloc()) cube{};
    o->position.x = rnd1(d);
    o->position.z = rnd1(d);
    o->velocity.x = rnd1(v);
    o->velocity.z = rnd1(v);
  }
}

// some additional shaders
static void application_init_shaders() {
  {
    constexpr char const *vtx = R"(
#version 330 core
uniform mat4 umtx_mw; // model-to-world-matrix
uniform mat4 umtx_wvp;// world-to-view-to-projection
layout(location = 0) in vec4 apos;
layout(location = 1) in vec4 argba;
layout(location = 2) in vec3 anorm;
layout(location = 3) in vec2 atex;

void main() {
  gl_Position = umtx_wvp * umtx_mw * apos;
}
    )";

    constexpr char const *frag = R"(
#version 330 core
out vec4 rgba;
void main() {
  rgba = vec4(vec3(gl_FragCoord.z), 1.0);
}
    )";

    glos::shaders.load_program_from_source(vtx, frag);
  }
  {
    constexpr char const *vtx = R"(
#version 330 core
uniform mat4 umtx_mw; // model-to-world-matrix
uniform mat4 umtx_wvp;// world-to-view-to-projection
in vec4 apos;
in vec4 argba;
out vec4 vrgba;
void main() {
  gl_Position = umtx_wvp * umtx_mw * apos;
  vrgba = argba;
}
    )";

    constexpr char const *frag = R"(
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