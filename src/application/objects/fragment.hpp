#pragma once

class fragment : public object {
public:
  uint32_t death_time_ms = 0;

  inline fragment() {
    name = "fragment";
    glob_ix = glob_ix_fragment;
    radius = globs.at(glob_ix).bounding_radius;
    collision_bits = cb_none;
    collision_mask = cb_none;
    mass = 10;
  }

  inline auto update() -> bool override {
    if (object::update()) {
      return true;
    }

    if (death_time_ms < frame_context.ms) {
      return true;
    }

    return false;
  }
};
