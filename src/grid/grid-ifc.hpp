#pragma once

#define grid_ifc_overlaps 0
class cell;
class grid_ifc {
public:
  cell *main_cell = nullptr;
  bits bits = 0; // 1: overlaps
  unsigned collide_mask = 0;
  unsigned collide_bits = 0;
  unsigned tick = 0;
  dynp checked_collisions_list = dynp_def;
};
