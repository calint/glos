#pragma once
// reviewed: 2024-01-04

class fragment : public object {
public:
  uint64_t death_time_ms = 0;

  inline fragment() {
    name = "fragment_";
    if (debug_multiplayer) {
      ++counter;
      name.append(std::to_string(counter));
      printf("%lu: %lu: create %s\n", frame_context.frame_num, frame_context.ms,
             name.c_str());
    }
    glob_ix = glob_ix_fragment;
    scale = {0.5f, 0.5f, 0.5f};
    radius = globs.at(glob_ix).bounding_radius * scale.x;
    mass = 10;
    collision_bits = cb_none;
    collision_mask = cb_none;
  }

  inline ~fragment() override {
    if (debug_multiplayer) {
      printf("%lu: %lu: free %s\n", frame_context.frame_num, frame_context.ms,
             name.c_str());
    }
  }

  inline auto update() -> bool override {
    if (not object::update()) {
      return false;
    }

    if (death_time_ms < frame_context.ms) {
      return false;
    }

    return true;
  }
};
