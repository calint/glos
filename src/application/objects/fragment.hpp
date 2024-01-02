#pragma once

class fragment : public object {
public:
  unsigned death_time = 0;

  inline fragment() {
    name = "fragment";
    glob_ix = glob_ix_fragment;
    glob const &g = globs.at(glob_ix);
    radius = g.bounding_radius;
    mass = 1;
    collision_bits = cb_none;
    collision_mask = cb_none;
  }

  inline auto update(frame_context const &fc) -> bool override {
    assert(not is_dead());

    if (object::update(fc)) {
      return true;
    }

    if (death_time < fc.ms) {
      return true;
    }

    return false;
  }
};
