#pragma once
// reviewed: 2023-12-22

// contains definitions used by engine and application

// grid dimensions
static constexpr unsigned grid_cell_size = 20;
static constexpr unsigned grid_ncells_wide = 8;
static constexpr unsigned grid_ncells_high = 8;

// window dimensions
static constexpr unsigned window_width = 512;
static constexpr unsigned window_height = 512;

// number of players in networked mode
static constexpr unsigned net_players = 2;

// number of preallocated objects
static constexpr unsigned objects_count = 8192;

// maximum size of any object instance in bytes
static constexpr unsigned objects_instance_size_B = 1024;

// collision bits
static constexpr unsigned cb_hero = 1;
