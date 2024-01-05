#pragma once
// reviewed: 2023-12-22
// reviewed: 2024-01-04

#include <cstdint>

// definitions used by engine and application

// grid dimensions
static constexpr unsigned grid_cell_size = 10;
static constexpr unsigned grid_ncells_wide = 4;
static constexpr unsigned grid_ncells_high = 4;

// multithreaded grid
// note. in some cases multithreaded mode is a degradation of performance
static constexpr bool grid_threaded = false;
static constexpr bool o1store_threaded = grid_threaded;
static constexpr bool update_threaded = true;

// o1store debugging
static constexpr bool o1store_check_double_free = false;
static constexpr bool o1store_check_free_limits = false;

// render debugging (runtime modifiable)
inline bool debug_object_planes_normals = false;
inline bool debug_object_bounding_sphere = false;

// debug log
static constexpr bool debug_multiplayer = true;

// window dimensions
static constexpr unsigned window_width = 1024;
static constexpr unsigned window_height = 1024;
// vsync should be 'true' when not doing performance tests
static constexpr bool window_vsync = false;

// number of players in networked mode
static constexpr unsigned net_players = 2;

// number of preallocated objects
static constexpr unsigned objects_count = 1024;

// maximum size of any object instance in bytes
static constexpr size_t objects_instance_size_B = 1024;

// game area
static constexpr float game_area_min_x = -22;
static constexpr float game_area_max_x = 22;
static constexpr float game_area_min_y = -22;
static constexpr float game_area_max_y = 22;
static constexpr float game_area_min_z = -22;
static constexpr float game_area_max_z = 22;

// collision bits
static constexpr uint32_t cb_none = 0;
static constexpr uint32_t cb_hero = 1;
static constexpr uint32_t cb_hero_bullet = 2;
static constexpr uint32_t cb_asteroid = 4;
static constexpr uint32_t cb_power_up = 8;

// glob indexes (are set by 'application_init()' when loading)
static uint32_t glob_ix_skydome = 0;
static uint32_t glob_ix_grid = 0;
static uint32_t glob_ix_ship = 0;
static uint32_t glob_ix_ship_engine_on = 0;
static uint32_t glob_ix_bullet = 0;
static uint32_t glob_ix_asteroid_large = 0;
static uint32_t glob_ix_asteroid_medium = 0;
static uint32_t glob_ix_asteroid_small = 0;
static uint32_t glob_ix_fragment = 0;
static uint32_t glob_ix_power_up = 0;

// game constants
static constexpr unsigned asteroid_level = 10;

static constexpr float asteroid_large_speed = 10;

static constexpr float asteroid_large_scale = 2;
static constexpr unsigned asteroid_large_split = 10;
static constexpr float asteroid_large_split_speed = 6;
static constexpr float asteroid_large_split_agl_vel_deg = 60;

static constexpr float asteroid_medium_scale = 1.2f;
static constexpr unsigned asteroid_medium_split = 10;
static constexpr float asteroid_medium_split_speed = 6;
static constexpr float asteroid_medium_split_agl_vel_deg = 100;

static constexpr float asteroid_small_scale = 0.75f;

static constexpr float ship_turn_rate_deg = 120;
static constexpr float ship_speed = 6;
static constexpr float ship_bullet_speed = 17;
static constexpr uint32_t ship_bullet_fire_interval_ms = 1'000;
static constexpr unsigned ship_bullet_level_1_fire_count = 5;
static constexpr float ship_bullet_spread = 4;

static constexpr float bullet_fragment_agl_vel_rnd = 360;

static constexpr int power_up_chance_rem = 5;
static constexpr uint32_t power_up_lifetime_ms = 30'000;

static std::atomic<unsigned> counter = 0;
