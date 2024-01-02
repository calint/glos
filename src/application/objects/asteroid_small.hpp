#pragma once

#include "../configuration.hpp"

class asteroid_small : public object {
public:
  inline asteroid_small() {
    name = "asteroid_small";
    glob_ix = glob_asteroid_small_ix;
    scale = {0.5f, 0.5f, 0.5f};
    glob const &g = globs.at(glob_ix);
    radius = g.bounding_radius * scale.x;
    mass = 1;
    collision_bits = cb_asteroid;
    collision_mask = cb_hero_bullet | cb_hero;
  }

  inline auto update(frame_context const &fc) -> bool override {
    assert(not is_dead());

    if (object::update(fc)) {
      return true;
    }

    if (position.x < game_area_min_x) {
      position.x = -position.x;
    } else if (position.x > game_area_max_x) {
      position.x = -position.x;
    } else if (position.z < game_area_min_z) {
      position.z = -position.z;
    } else if (position.z > game_area_max_z) {
      position.z = -position.z;
    }

    return false;
  }

  inline auto on_collision(object *o, frame_context const &fc)
      -> bool override {
    assert(not is_dead());
    printf("%u: %s collision with %s\n", fc.tick, name.c_str(),
           o->name.c_str());
    return true;
  }
};
