#pragma once

class cell;
class object;

class grid_ifc {
public:
  cell *main_cell = nullptr;
  unsigned tick = 0;
  unsigned collision_bits = 0;
  unsigned collision_mask = 0;
  std::vector<const object *> checked_collisions{};
  unsigned bits = 0; // 1: overlaps
};
