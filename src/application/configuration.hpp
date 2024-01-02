#pragma once
// reviewed: 2023-12-22

#include <cstdint>

// definitions used by engine and application

// grid dimensions
static constexpr int grid_cell_size = 20;
static constexpr int grid_ncells_wide = 8;
static constexpr int grid_ncells_high = 8;

// multithreaded grid
// note. in some cases multithreaded mode is a degradation of performance
static constexpr bool grid_threaded = true;
static constexpr bool o1store_threaded = grid_threaded;

// o1store debugging
static constexpr bool o1store_check_double_free = true;
static constexpr bool o1store_check_free_limits = true;

// debug volume normals (turn off)
static constexpr bool object_planes_debug_normals = false;

// window dimensions
static constexpr int window_width = 1024;
static constexpr int window_height = 1024;

// number of players in networked mode
static constexpr int net_players = 2;

// number of preallocated objects
static constexpr int objects_count = 32 * 1024;

// maximum size of any object instance in bytes
static constexpr int objects_instance_size_B = 1024;

// dimensions
static constexpr float game_area_min_x = -25;
static constexpr float game_area_max_x = 25;
static constexpr float game_area_min_y = -25;
static constexpr float game_area_max_y = 25;
static constexpr float game_area_min_z = -25;
static constexpr float game_area_max_z = 25;

// collision bits
static constexpr uint32_t cb_none = 0;
static constexpr uint32_t cb_hero = 1;
static constexpr uint32_t cb_hero_bullet = 2;
static constexpr uint32_t cb_asteroid = 4;

// glob indexes (set at 'application.init()' when loading)
static uint32_t glob_skydome_ix = 0;
static uint32_t glob_grid_ix = 0;
static uint32_t glob_ship_ix = 0;
static uint32_t glob_ship_engine_on_ix = 0;
static uint32_t glob_bullet_ix = 0;
static uint32_t glob_asteroid_large_ix = 0;
static uint32_t glob_asteroid_medium_ix = 0;
static uint32_t glob_asteroid_small_ix = 0;
static uint32_t glob_fragment_ix = 0;

// game constants
constexpr int asteroid_large_speed = 10;

constexpr int asteroid_large_split = 4;
constexpr int asteroid_large_split_speed = 10;
constexpr int asteroid_large_split_angular_vel_deg = 60;

constexpr int asteroid_medium_split = 4;
constexpr int asteroid_medium_split_speed = 12;
constexpr int asteroid_medium_split_angular_vel_deg = 100;

constexpr float ship_turn_rate_deg = 120;
constexpr float ship_speed = 6;
constexpr float ship_bullet_speed = 17;
constexpr unsigned ship_bullet_fire_intervall_ms = 500;
