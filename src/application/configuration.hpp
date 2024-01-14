#pragma once
// reviewed: 2023-12-22
// reviewed: 2024-01-04
// reviewed: 2024-01-06
// reviewed: 2024-01-08

//
// definitions used by engine
//

// true if performance test case instead of game
static constexpr bool performance_test = true;
static constexpr float cube_speed = 10;

// multiplayer debugging output
static constexpr bool debug_multiplayer = false;

// multithreaded grid
// note. in some cases multithreaded mode is a degradation of performance
// note. multiplayer mode cannot use 'threaded_grid' because of the
//       non-deterministic behavior
static constexpr bool threaded_grid = true;
static constexpr bool threaded_o1store = threaded_grid;
static constexpr bool threaded_update = false;

// grid dimensions
static constexpr float grid_size =
    performance_test ? 3200 : 40; // square side in e.g. meters
static constexpr unsigned grid_rows = performance_test ? 16 : 4;
static constexpr unsigned grid_columns = grid_rows;
static constexpr float grid_cell_size = grid_size / grid_rows;

// o1store debugging (assertions should be on in development)
static constexpr bool o1store_check_double_free = false;
static constexpr bool o1store_check_free_limits = false;

// metrics print to console
static constexpr bool metrics_print = true;

// render debugging (runtime modifiable)
inline bool debug_object_planes_normals = false;
inline bool debug_object_bounding_sphere = false;

// initially render the hud
static constexpr bool hud_enabled = true;

// window dimensions
static constexpr unsigned window_width = 1024;
static constexpr unsigned window_height = 1024;
static constexpr bool window_vsync = performance_test ? false : true;
// note. vsync should be on when not doing performance tests

// number of players in networked mode
static constexpr unsigned net_players = 2;

// number of preallocated objects
static constexpr unsigned objects_count =
    performance_test ? 64 * 1024 : 1 * 1024;

// maximum size of any object instance in bytes
static constexpr size_t objects_instance_size_B = 512;

//
// definitions used by game
//

// collision bits
static constexpr uint32_t cb_none = 0;
static constexpr uint32_t cb_hero = 1U << 0U;
static constexpr uint32_t cb_hero_bullet = 1U << 1U;
static constexpr uint32_t cb_asteroid = 1U << 2U;
static constexpr uint32_t cb_power_up = 1U << 3U;

// glob indexes (are set by 'application_init()' when loading)
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

// settings
static constexpr unsigned asteroid_level = 2;

static constexpr float asteroid_large_agl_vel_rnd = glm::radians(45.0f);
static constexpr float asteroid_large_speed = 10;
static constexpr float asteroid_large_scale = 2;
static constexpr unsigned asteroid_large_split = 4;
static constexpr float asteroid_large_split_speed = 6;
static constexpr float asteroid_large_split_agl_vel_rnd = glm::radians(120.0f);

static constexpr float asteroid_medium_scale = 1.2f;
static constexpr unsigned asteroid_medium_split = 4;
static constexpr float asteroid_medium_split_speed = 6;
static constexpr float asteroid_medium_split_agl_vel_rnd = glm::radians(200.0f);

static constexpr float asteroid_small_scale = 0.75f;

static constexpr float ship_turn_rate = glm::radians(120.0f);
static constexpr float ship_speed = 6;
static constexpr float ship_bullet_speed = 17;
static constexpr unsigned ship_bullet_fire_interval_ms = 1'000;
static constexpr unsigned ship_bullet_level_1_fire_count = 5;
static constexpr float ship_bullet_spread = 4;

static constexpr float bullet_fragment_agl_vel_rnd = glm::radians(360.0f);

static constexpr int power_up_chance_rem = 5;
static constexpr unsigned power_up_lifetime_ms = 30'000;

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

// used when 'debug_multiplayer' is true to give objects unique numbers
static std::atomic<unsigned> counter = 0;
