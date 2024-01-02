#pragma once

#include "fragment.hpp"

class bullet : public object {
public:
  inline bullet() {
    name = "bullet";
    glob_ix = glob_bullet_ix;
    scale = {1, 1, 1};
    glob const &g = globs.at(glob_ix);
    radius = g.bounding_radius * 1; // r * scale
    mass = 1;
    collision_bits = cb_hero_bullet;
    collision_mask = cb_asteroid;
  }

  inline auto update(frame_context const &fc) -> bool override {
    assert(not is_dead());

    if (object::update(fc)) {
      return true;
    }

    if (position.x < game_area_min_x or position.x > game_area_max_x or
        position.y < game_area_min_y or position.y > game_area_max_x or
        position.z < game_area_min_z or position.z > game_area_max_z) {
      return true;
    }

    return false;
  }

  inline auto on_collision(object *o, frame_context const &fc)
      -> bool override {
    assert(not is_dead());
    printf("%u: %s collision with %s\n", fc.tick, name.c_str(),
           o->name.c_str());

    fragment *frg = new (objects.alloc()) fragment{};
    frg->position = position;
    frg->angular_velocity = vec3(radians(float(rand() % 360 - 180)));
    frg->death_time = fc.ms + 500;
    frg->scale = {0.5f, 0.5f, 0.5f};
    frg->radius *= frg->scale.x;

    return true;
  }
};
