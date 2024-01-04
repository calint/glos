#pragma once
// reviewed: 2024-01-04

class power_up : public object {
public:
  inline power_up() {
    name = "power_up";
    glob_ix = glob_ix_power_up;
    scale = {0.5f, 0.5f, 0.5f};
    radius = globs.at(glob_ix).bounding_radius * scale.x;
    is_sphere = true;
    death_time_ms = frame_context.ms + power_up_lifetime_ms;
    collision_bits = cb_power_up;
    collision_mask = cb_hero;
    mass = 10;
  }

  inline auto update() -> bool override {
    if (object::update()) {
      return true;
    }

    if (death_time_ms < frame_context.ms) {
      return true;
    }

    if (scale_time_ms < frame_context.ms) {
      if (scale_up) {
        scale_up = false;
        scale = {0.75f, 0.75f, 0.75f};
        radius = globs.at(glob_ix).bounding_radius * scale.x;
      } else {
        scale_up = true;
        scale = {0.5f, 0.5f, 0.5f};
        radius = globs.at(glob_ix).bounding_radius * scale.x;
      }
      scale_time_ms = frame_context.ms + 1'000;
    }

    return false;
  }

  inline auto on_collision(object *o) -> bool override { return true; }

private:
  uint32_t death_time_ms = 0;
  uint32_t scale_time_ms = 0;
  bool scale_up = false;
};
