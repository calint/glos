#pragma once

#define grid_ifc_overlaps 0

class cell;
class object;

class grid_ifc {
public:
  cell *main_cell = nullptr;
  unsigned tick = 0;
  unsigned collision_bits = 0;
  unsigned collision_mask = 0;
  std::vector<const object *> checked_collisions_list{};
  bits bits = 0; // 1: overlaps
};
