#pragma once

class power_up : public object {
public:
  inline power_up() {
    name = "power_up";
    glob_ix = glob_ix_power_up;
    glob const &g = globs.at(glob_ix);
    scale = {0.5f, 0.5f, 0.5f};
    radius = g.bounding_radius * scale.x;
    is_sphere = true;
    mass = 1;
    collision_bits = cb_power_up;
    collision_mask = cb_hero;
  }

  inline auto update(frame_context const &fc) -> bool override {
    assert(not is_dead());

    if (object::update(fc)) {
      return true;
    }

    return false;
  }

  inline auto on_collision(object *o, frame_context const &fc)
      -> bool override {
    return true;
  }
};
