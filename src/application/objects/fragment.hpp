#pragma once
// reviewed: 2024-01-04
// reviewed: 2024-01-08
// reviewed: 2024-07-08
// reviewed: 2024-07-15

class fragment final : public object {
public:
  uint64_t death_time_ms = 0;

  inline fragment() {
    if (debug_multiplayer) {
      uint32_t const c = counter.fetch_add(1) + 1;
      name.append("fragment_").append(std::to_string(c));
      printf("%lu: %lu: create %s\n", frame_context.frame_num, frame_context.ms,
             name.c_str());
    }
    glob_ix(glob_ix_fragment);
    scale = {0.5f, 0.5f, 0.5f};
    bounding_radius = glob().bounding_radius * scale.x;
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
    if (!object::update()) {
      return false;
    }

    if (death_time_ms < frame_context.ms) {
      return false;
    }

    return true;
  }
};
