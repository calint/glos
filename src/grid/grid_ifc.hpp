#pragma once
// reviewed: 2023-12-22

class cell;
class object;

class grid_ifc {
public:
  cell *main_cell = nullptr;
  unsigned tick = 0; // used when object in several cells to only update once
  unsigned collision_bits = 0;
  unsigned collision_mask = 0;
  std::vector<const object *> checked_collisions{};
  unsigned bits = 0; // 1: overlaps  2: dead
};
