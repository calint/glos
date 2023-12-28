#pragma once
// reviewed: 2023-12-22

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

// window dimensions
static constexpr int window_width = 512;
static constexpr int window_height = 512;

// number of players in networked mode
static constexpr int net_players = 2;

// number of preallocated objects
static constexpr int objects_count = 32 * 1024;

// maximum size of any object instance in bytes
static constexpr int objects_instance_size_B = 1024;

// collision bits
static constexpr unsigned cb_none = 0;
static constexpr unsigned cb_hero = 1;

// glo indexes
static constexpr int glo_index_skydome = 0;
static constexpr int glo_index_grid = 1;
static constexpr int glo_index_santa = 2;
