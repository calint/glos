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

// collision bits
static constexpr uint32_t cb_none = 0;
static constexpr uint32_t cb_hero = 1;
static constexpr uint32_t cb_hero_bullet = 2;
static constexpr uint32_t cb_asteroid = 4;

// glob indexes (set at 'application.init()' when loading)
static uint32_t glob_skydome_ix = 0;
static uint32_t glob_grid_ix = 0;
static uint32_t glob_santa_ix = 0;
static uint32_t glob_sphere_ix = 0;
static uint32_t glob_cube_ix = 0;
static uint32_t glob_ship_ix = 0;
static uint32_t glob_bullet_ix = 0;
static uint32_t glob_asteroid_large_ix = 0;
static uint32_t glob_asteroid_medium_ix = 0;
static uint32_t glob_asteroid_small_ix = 0;

static constexpr float game_area_min_x = -25;
static constexpr float game_area_max_x = 25;
static constexpr float game_area_min_y = -25;
static constexpr float game_area_max_y = 25;
static constexpr float game_area_min_z = -25;
static constexpr float game_area_max_z = 25;
