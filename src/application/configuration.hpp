#pragma once
// reviewed: 2023-12-22
// reviewed: 2024-01-04
// reviewed: 2024-01-06
// reviewed: 2024-01-08
// reviewed: 2024-07-08

//
// configuration used by engine
//

// multithreaded grid
// note: in some cases multithreaded mode is a degradation of performance
// note: multiplayer mode cannot use 'threaded_grid' because of the
//       non-deterministic behavior
static constexpr bool threaded_grid = false;
static constexpr bool threaded_update = false;

// o1store debugging (assertions should be on in development)
static constexpr bool o1store_check_double_free = false;
static constexpr bool o1store_check_free_limits = false;

// metrics print to console
static constexpr bool metrics_print = true;

// initially render the hud
static constexpr bool hud_enabled = true;

// render debugging (runtime modifiable)
static bool debug_object_planes_normals = false;
static bool debug_object_bounding_sphere = false;

//
// configuration used by the game
//

// game or performance test
// 0: none (game)  1: cubes  2: spheres
static constexpr uint32_t performance_test_type = 0;

static constexpr bool is_performance_test = performance_test_type != 0;
static constexpr float cube_speed = 10;
static constexpr float sphere_speed = 10;

// grid dimensions
static constexpr float grid_size = is_performance_test ? 3200 : 40;
// square side in e.g. meters

static constexpr uint32_t grid_rows = is_performance_test ? 16 : 4;
static constexpr uint32_t grid_columns = grid_rows;
static constexpr float grid_cell_size = grid_size / grid_rows;

// window dimensions
static constexpr uint32_t window_width = 1024;
static constexpr uint32_t window_height = 1024;
static constexpr bool window_vsync = is_performance_test ? false : true;
// note: vsync should be on when not doing performance tests

// number of players in networked mode
static constexpr uint32_t net_players = 2;

// multiplayer debugging output
static constexpr bool debug_multiplayer = false;

// maximum size of any object instance in bytes
static constexpr size_t objects_instance_size_B = 512;

// number of preallocated objects
static constexpr uint32_t objects_count =
    is_performance_test ? 64 * 1024 : 1 * 1024;

// collision bits
static constexpr uint32_t cb_none = 0;
static constexpr uint32_t cb_hero = 1U << 0U;
static constexpr uint32_t cb_hero_bullet = 1U << 1U;
static constexpr uint32_t cb_asteroid = 1U << 2U;
static constexpr uint32_t cb_power_up = 1U << 3U;
static constexpr uint32_t cb_ufo = 1U << 4U;
static constexpr uint32_t cb_ufo_bullet = 1U << 5U;

// settings
static constexpr uint32_t asteroids_per_level = 2;

static constexpr float asteroid_large_agl_vel_rnd = glm::radians(75.0f);
static constexpr float asteroid_large_speed = 10;
static constexpr float asteroid_large_scale = 2;
static constexpr uint32_t asteroid_large_split = 4;
static constexpr float asteroid_large_split_speed = 6;
static constexpr float asteroid_large_split_agl_vel_rnd = glm::radians(120.0f);

static constexpr float asteroid_medium_scale = 1.2f;
static constexpr uint32_t asteroid_medium_split = 4;
static constexpr float asteroid_medium_split_speed = 6;
static constexpr float asteroid_medium_split_agl_vel_rnd = glm::radians(200.0f);

static constexpr float asteroid_small_scale = 0.75f;

static constexpr float ship_turn_rate = glm::radians(120.0f);
static constexpr float ship_speed = 6;
static constexpr float ship_bullet_speed = 17;

static constexpr float bullet_fragment_agl_vel_rnd = glm::radians(360.0f);

static constexpr int32_t power_up_chance_rem = 5;
static constexpr uint32_t power_up_lifetime_ms = 30'000;
static constexpr uint32_t power_up_min_span_interval_ms = 5'000;

static constexpr float ufo_velocity = 15;
static constexpr float ufo_power_up_velocity = 5;
static constexpr uint32_t ufo_power_ups_at_death = 3;
static constexpr float ufo_bullet_velocity = 12;
static constexpr float ufo_angle_x_rate = 40.0f;
static constexpr uint32_t ufo_fire_rate_interval_ms = 2'500;

// game area based on grid and biggest object
static constexpr float game_area_half_x = grid_size / 2;
static constexpr float game_area_half_y = 10; // screen depth
static constexpr float game_area_half_z = grid_size / 2;

// set game area so the largest object, asteroid_large, just fits outside the
// screen before rollover
static constexpr float game_area_min_x =
    -game_area_half_x - asteroid_large_scale;
static constexpr float game_area_max_x =
    game_area_half_x + asteroid_large_scale;
static constexpr float game_area_min_y =
    -game_area_half_y - asteroid_large_scale;
static constexpr float game_area_max_y =
    game_area_half_y + asteroid_large_scale;
static constexpr float game_area_min_z =
    -game_area_half_z - asteroid_large_scale;
static constexpr float game_area_max_z =
    game_area_half_z + asteroid_large_scale;

// glob indexes (are set by 'application_init()' when loading models)
static uint32_t glob_ix_ship = 0;
static uint32_t glob_ix_ship_engine_on = 0;
static uint32_t glob_ix_bullet = 0;
static uint32_t glob_ix_asteroid_large = 0;
static uint32_t glob_ix_asteroid_medium = 0;
static uint32_t glob_ix_asteroid_small = 0;
static uint32_t glob_ix_fragment = 0;
static uint32_t glob_ix_power_up = 0;
static uint32_t glob_ix_cube = 0;
static uint32_t glob_ix_tetra = 0;
static uint32_t glob_ix_sphere = 0;
static uint32_t glob_ix_skydome = 0;
static uint32_t glob_ix_ufo = 0;
static uint32_t glob_ix_ufo_bullet = 0;

// used when 'debug_multiplayer' is true to give objects unique numbers
static std::atomic<uint32_t> counter = 0;
