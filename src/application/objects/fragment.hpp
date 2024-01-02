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

  inline auto update() -> bool override {
    assert(not is_dead());

    if (object::update()) {
      return true;
    }

    if (death_time < frame_context.ms) {
      return true;
    }

    return false;
  }
};