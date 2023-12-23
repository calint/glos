#pragma once
// reviewed: 2023-12-22

class object;

// decoupling circular reference between 'grid' and 'object'
class grid_ifc {
public:
  unsigned collision_bits = 0;
  unsigned collision_mask = 0;
  unsigned updated_at_tick = 0;
  unsigned rendered_at_tick = 0;
  unsigned checked_collision_list_at_tick = 0;
  std::vector<const object *> checked_collisions{};
  unsigned bits = 0; // 1: overlaps cells  2: is dead
};
