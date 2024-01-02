#pragma once

class power_up : public object {
public:
  uint32_t death_time_ms = 0;

  inline power_up() {
    name = "power_up";
    glob_ix = glob_ix_power_up;
    glob const &g = globs.at(glob_ix);
    scale = {0.5f, 0.5f, 0.5f};
    radius = g.bounding_radius * scale.x;
    is_sphere = true;
    mass = 1;
    death_time_ms = frame_context.ms + 20'000;
    collision_bits = cb_power_up;
    collision_mask = cb_hero;
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

  inline auto on_collision(object *o) -> bool override { return true; }
};
