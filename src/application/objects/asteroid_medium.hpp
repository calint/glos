#pragma once

#include "../configuration.hpp"
//
#include "asteroid_small.hpp"

class asteroid_medium : public object {
public:
  inline asteroid_medium() {
    name = "asteroid_medium";
    glob_ix = glob_asteroid_medium_ix;
    scale = {1, 1, 1};
    glob const &g = globs.at(glob_ix);
    radius = g.bounding_radius * 1; // r * scale
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
    for (int i = 0; i < 5; i++) {
      asteroid_small *ast = new (objects.alloc()) asteroid_small{};
      ast->position = position;

      constexpr int v = 16;
      ast->velocity = velocity + vec3(float(rand() % v - v / 2), 0,
                                      float(rand() % v - v / 2));
      constexpr int r = 100;
      ast->angular_velocity = vec3(radians(float(rand() % r - r / 2)),
                                   radians(float(rand() % r - r / 2)),
                                   radians(float(rand() % r - r / 2)));
    }
    return true;
  }
};
