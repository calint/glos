#pragma once
// reviewed: 2024-01-04

class fragment : public object {
public:
  uint64_t death_time_ms = 0;

  inline fragment() {
    name = "fragment";
    glob_ix = glob_ix_fragment;
    scale = {0.5f, 0.5f, 0.5f};
    radius = globs.at(glob_ix).bounding_radius * scale.x;
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
